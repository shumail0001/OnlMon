#include "OnlMonDBodbc.h"
#include "OnlMonDBVar.h"
#include "OnlMonDBReturnCodes.h"

#include <phool/phool.h>

#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/errorhandler.h>
#include <odbc++/databasemetadata.h>
#include <odbc++/preparedstatement.h>
#include <odbc++/resultset.h>
#include <odbc++/resultsetmetadata.h>

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace odbc;
using namespace std;

static const unsigned int DEFAULTCOLUMNS = 2;
static const unsigned int COLUMNSPARVAR = 3;
static const string addvarname[3] = {"", "err", "qual"};
static const unsigned int MINUTESINTERVAL = 4;

static Connection *con = NULL;

// #define VERBOSE

OnlMonDBodbc::OnlMonDBodbc(const std::string &tablename):
  OnlMonBase(tablename),
  dbname("OnlMonDB"),
  dbowner("phnxrc"),
  dbpasswd(""),
  table(tablename)
{
  // table names are lower case only, so convert string to lowercase
  // The bizarre cast here is needed for newer gccs
  transform(table.begin(), table.end(), table.begin(), (int(*)(int))tolower);
}

OnlMonDBodbc::~OnlMonDBodbc()
{
  delete con;
  con = NULL;
}


int
OnlMonDBodbc::CreateTable()
{
  if (GetConnection())
    {
      return -1;
    }

  ostringstream cmd;
  cmd << "SELECT * FROM pg_tables where tablename = '" << table << "'" ;
  ResultSet *rs = NULL;
  Statement* stmt = con->createStatement();
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "caught exception Message: " << e.getMessage() << endl;
    }
  int iret = 0;
  if (! rs->next())
    {
      delete rs;
      rs = NULL;
      if (verbosity > 0)
        {
          cout << "need to create table" << endl;
        }
      cmd.str("");
      cmd << "CREATE TABLE " << table << "(date timestamp(0) with time zone NOT NULL, run int NOT NULL, primary key(date,run))";
      try
        {
          stmt->executeUpdate(cmd.str());
        }
      catch (SQLException& e)
        {
          cout << "caught exception Message: " << e.getMessage() << endl;
        }

    }
  else
    {
      if (verbosity > 0)
        {
          cout << "table " << table << " exists" << endl;
        }
    }
  delete rs;
  delete stmt;
  return iret;
}

int
OnlMonDBodbc::CheckAndCreateTable(const map<const string, OnlMonDBVar *> &varmap)
{
  if (GetConnection())
    {
      return -1;
    }
  if (CreateTable()) // check and create the table
    {
      return -1;
    }
  Statement* stmt = con->createStatement();
  ResultSet *rs = NULL;
  ostringstream cmd;
  int iret = 0;
  cmd << "select * from " << table << " limit 1";
  map<const string, OnlMonDBVar *>::const_iterator iter;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "caught exception Message: " << e.getMessage() << endl;
    }
  ResultSetMetaData* meta = rs->getMetaData();
  unsigned int nocolumn = rs->getMetaData()->getColumnCount();
  for (iter = varmap.begin(); iter != varmap.end(); ++iter)
    {
      string varname = iter->first;
      // column names are lower case only, so convert string to lowercase
      // The bizarre cast here is needed for newer gccs
      transform(varname.begin(), varname.end(), varname.begin(), (int(*)(int))tolower);
      for (short int j = 0; j < 3; j++)
        {
          string thisvar = varname + addvarname[j];
          for (unsigned int i = DEFAULTCOLUMNS + 1; i <= nocolumn; i++)
            {
              if (meta->getColumnName(i) == thisvar)
                {
                  if (verbosity > 0)
                    {
                      cout << thisvar << " is in table" << endl;
                    }
                  goto foundvar;
                }
            }
          cmd.str("");
          cmd << "ALTER TABLE " << table << " ADD COLUMN "
	      << thisvar << " real";
          if (verbosity > 0)
            {
              cout << "executing sql command: " << cmd.str() << endl;
            }

          try
            {
              Statement* chgtable = con->createStatement();
              iret = chgtable->executeUpdate(cmd.str());
	      delete chgtable;
            }
          catch (SQLException& e)
            {
              cout << "Table " << table << " update failed" << endl;
              cout << "Message: " << e.getMessage() << endl;
            }

foundvar:
          continue;
        }
    }
  delete rs;
  delete stmt;
  // check columns
  return iret;
}

int
OnlMonDBodbc::DropTable(const string &name)
{
  if (GetConnection())
    {
      return -1;
    }
  string tablename = name;
  // table names are lower case only, so convert string to lowercase
  // The bizarre cast here is needed for newer gccs
  transform(tablename.begin(), tablename.end(), tablename.begin(), (int(*)(int))tolower);
  cout << con->getMetaData()-> getDatabaseProductVersion() << endl;
  cout << con->getCatalog() << endl;
  Statement* stmt = con->createStatement();
  ostringstream cmd;
  cmd << "DROP TABLE " << tablename ;

  int iret = stmt->executeUpdate(cmd.str());
  cout << "iret: " << iret << endl;
  return iret;

}

void
OnlMonDBodbc::Dump()
{
  if (GetConnection())
    {
      return;
    }

  //cout << con->getMetaData()-> getDatabaseProductVersion() << endl;
  Statement* stmt = con->createStatement();
  ostringstream cmd;
  cmd << "SELECT * FROM " << table ;
  cout << "Executing " << cmd.str() << endl;
  ResultSet* rs = stmt->executeQuery(cmd.str());
  Dump(rs);
  return ;
}

void
OnlMonDBodbc::Dump(ResultSet *rs) const
{
  ResultSetMetaData* meta = rs->getMetaData();
  unsigned int ncolumn = meta->getColumnCount();
  while (rs->next())
    {

      Timestamp id = rs->getTimestamp(1);
      int Name = rs->getInt(2);
      cout << "TimeStamp: " << (id.toString()) << " Run #= " << Name << endl;
      for (unsigned int i = DEFAULTCOLUMNS + 1; i <= ncolumn;i++)
	{
	  float rval = rs->getFloat(i);
	  if (rs->wasNull())
	    {
	      cout << meta->getColumnName(i) << ": NULL" << endl;
	    }
	  else
	    {
	      cout << meta->getColumnName(i) << ": " << rval << endl;
	    }
	}
    }
  try
    {
      rs->first();
    }
  catch (SQLException& e)
    {
      cout << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
    }

  return ;
}

int
OnlMonDBodbc::Info(const char *name)
{
  if (GetConnection())
    {
      return -1;
    }

  cout << con->getMetaData()-> getDatabaseProductVersion() << endl;
  // if no argument is given print out list of tables
  if (! name)
    {
      cout << "Driver: " << con->getMetaData()->getDriverName() << endl;
      cout << "User: " << con->getMetaData()->getUserName() << endl;
      string catalog = con->getMetaData()->getCatalogTerm();
      cout << "Catalog: " << catalog << endl;
      string schemapattern = con->getMetaData()->getSchemaTerm();
      cout << "Schema: " << schemapattern << endl;
      string tablenamepattern = con->getMetaData()->getTableTerm();
      cout << "Table: " << tablenamepattern << endl;
      vector<string> types;
      ResultSet* rs = con->getMetaData()->getTableTypes();
      while (rs->next())
	{
	  cout << "1: " << rs->getString(1) << endl;
	  cout << "2: " << rs->getString(2) << endl;
	  cout << "3: " << rs->getString(3) << endl;
	  cout << "4: " << rs->getString(4) << endl;
	  cout << "5: " << rs->getString(5) << endl;
	}
      ResultSet* rs1 = con->getMetaData()->getTables(catalog, schemapattern, tablenamepattern, types );

      cout << "rs1: " << rs1 << endl;
      //        cout << rs->getMetaData()->getTableName(1) << endl;
      while (rs1->next())
	{
	  cout << "Table: " << rs->getString("TABLE_CAT") << endl;
	  cout << "Table: " << rs->getString(3) << endl;
	}
    }
  else
    {
      Statement* stmt = con->createStatement();
  string tablename = name;
  // table names are lower case only, so convert string to lowercase
  // The bizarre cast here is needed for newer gccs
  transform(tablename.begin(), tablename.end(), tablename.begin(), (int(*)(int))tolower);
      ostringstream cmd;
      cmd << "select * from " << tablename ;
      ResultSet* rs = stmt->executeQuery("select * from inttest");
      cout << rs->getMetaData()->getColumnCount() << endl;
      while (rs->next())
	{

	  int id = rs->getInt(1);
	  float Name = rs->getFloat(2);
	  cout << "Row: " << rs->getRow() << ", id=" << id << "   Name= " << Name << endl;
	}
    }
  return 0;

}

void
OnlMonDBodbc::identify() const
{
  cout << "DB Name: " << dbname << endl;
  cout << "DB Owner: " << dbowner << endl;
  cout << "DB Pwd: " << dbpasswd << endl;
  cout << "DB table: " << table << endl;
  return ;
}

int
OnlMonDBodbc::AddRow(const time_t ticks, const int runnumber, const map<const string, OnlMonDBVar *> &varmap)
{
  // bail out when restarted before a run was taken - run=0, ticks=0
  if (ticks == 0 || runnumber <= 0)
    {
      return -1;
    }
  map<const string, OnlMonDBVar *>::const_iterator iter;
  int iret = 0;
  int minutesinterval = MINUTESINTERVAL;
  ostringstream cmd, cmd1, datestream, datestreamhalf;
  Timestamp thistime(ticks);
  Timestamp mintime;
  Timestamp maxtime;

  if (GetConnection())
    {
      return -1;
    }

  Statement* query = con->createStatement();
 searchagain:
  mintime.setTime(ticks - minutesinterval*60);
  maxtime.setTime(ticks + minutesinterval*60);
#ifdef VERBOSE

  cout << "mintime stp: " << mintime.toString() << endl;
  cout << "maxtime stp: " << maxtime.toString() << endl;
#endif

  cmd.str("");
  datestream.str("");
  datestream << "date > '" << mintime.toString()
	     << "' and date < '" << maxtime.toString() << "'";
  cmd << "SELECT COUNT(*) FROM " << table << " WHERE run = "
      << runnumber << " and " << datestream.str() ;
#ifdef VERBOSE

  cout << "cmd: " << cmd.str() << endl;
#endif

  ResultSet *rs = 0;

  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
      cout << "sql cmd: " << cmd.str() << endl;
    }
  int haverow = 0;
  if (rs)
    {
      while (rs->next())
	{
	  haverow = rs->getInt(1);
#ifdef VERBOSE

	  cout << "found rows: " << haverow << endl;
#endif

	}
      delete rs;
    }
  if (haverow > 1)
    {
      minutesinterval = minutesinterval / 2;
      goto searchagain;
    }
  else if (haverow == 1)
    {
      cmd.str("");
      cmd << "SELECT * FROM " << table << " WHERE " << datestream.str() ;
#ifdef VERBOSE

      cout << "command: " << cmd.str() << endl;
#endif

      rs = query->executeQuery(cmd.str());
      // if the following works, the query returned one or more rows
      // in the given time interval
      while (rs->next())
        {
          cmd.str("");
          for (iter = varmap.begin(); iter != varmap.end(); ++iter)
            {
              if (iter->second->wasupdated())
                {
                  string varqualname = iter->first + addvarname[2];
                  float varqual = iter->second->GetVar(2);
                  float sqlvarqual = rs->getFloat(varqualname);
                  if (varqual > sqlvarqual || rs->wasNull())
                    {
                      Statement* upd = con->createStatement();
                      // if there is only 1 row we do not need to worry which one to update
                      for (unsigned int j = 0; j < 3; j++)
                        {
                          cmd.str("");
                          cmd << "UPDATE " << table << " SET "
			      << iter->first << addvarname[j] << " = "
			      << iter->second->GetVar(j) << " WHERE "
			      << datestream.str() ;
#ifdef VERBOSE

                          cout << "Command: " << cmd.str() << endl;
#endif

                          int iret = upd->executeUpdate(cmd.str());
                          if (!iret)
                            {
                              cout << PHWHERE << "Update failed please send mail to pinkenburg@bnl.gov"
				   << endl;
                              cout << "And include the macro and the following info" << endl;
                              cout << "TableName: " << table << endl;
                              cout << "Variable: " << iter->first << addvarname[j] << endl;
                              cout << "Value: " << iter->second->GetVar(j) << endl;
                              cout << "TimeStamp: " << rs->getTimestamp(1).toString() << endl;
                            }
                        }

                    }
                }
            }
        }
    }
  else
    {
      cmd.str("");
      cmd << "INSERT INTO " << table << "(date, run";
      cmd1 << "VALUES('" << thistime.toString() << "'," << runnumber;
      int newval = 0;
      for (iter = varmap.begin(); iter != varmap.end(); ++iter)
        {
          if (iter->second->wasupdated())
            {
              for (unsigned int j = 0; j < 3; j++)
                {
                  cmd << ", " << iter->first << addvarname[j];
                  cmd1 << ", " << iter->second->GetVar(j);
                  newval++;
                }
            }
        }
      if (!newval)
        {
          printf("No updated values\n");
          return -1;
        }
      cmd << ") ";
      cmd1 << ")";
      cmd << cmd1.str() ;

#ifdef VERBOSE

      cout << cmd.str() << endl;
#endif

      Statement* stmt = con->createStatement();
      try
        {
          stmt->executeUpdate(cmd.str());
        }
      catch (SQLException& e)
        {
          string errmsg = e.getMessage();
          if (errmsg.find("Cannot insert a duplicate key into unique index") != string::npos)
            {
#ifdef VERBOSE
              cout << "Identical entry already in DB" << endl;
#endif
	      iret = 0;
            }
          else
            {
              cout << PHWHERE << " DB Error in execute stmt: " << e.getMessage() << endl;
	      ofstream savesql("lostupdates.sql",ios_base::app);
	      savesql << cmd.str() << endl;
              savesql.close();
              iret = -1;
            }
        }
    }
  return iret;
}

int
OnlMonDBodbc::GetVar(const time_t begin, const time_t end, const std::string &varname, std::vector<time_t> &timestp, std::vector<int> &runnumber, std::vector<float> &var, std::vector<float> &varerr)
{
  if (GetConnection())
    {
      return DBNOCON;
    }
  int iret = 0;

  Statement* query = con->createStatement();
  Timestamp mintime(begin);
  Timestamp maxtime(end);
  string varnameerr = varname + addvarname[1];
  ostringstream cmd, datestream;
  datestream << "date > '" << mintime.toString()
	     << "' and date < '" << maxtime.toString() << "'";
  cmd << "SELECT COUNT(*) FROM " << table << " WHERE " << datestream.str() ;

#ifdef VERBOSE
  cout << "command: " << cmd.str() << endl;
#endif

  ResultSet *rs;
  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, probably your table "
	   << table
	   << " does not exist" << endl;
      cout << "Message: " << e.getMessage() << endl;
      varerr.resize(0);
      var.resize(0);
      timestp.resize(0);
      runnumber.resize(0);
      return -1;
    }
  int nrows = 0;
  while (rs->next())
    {
      nrows = rs->getInt(1);
    }
  timestp.resize(nrows);
  runnumber.resize(nrows);
  var.resize(nrows);
  varerr.resize(nrows);
  if (!nrows)
    {
      return DBNOENTRIES;
    }
  delete rs;
  cmd.str("");
  // get data in ascending date order (earliest time first)
  cmd << "SELECT date, run, " << varname << ", " << varnameerr
      << " FROM " << table << " WHERE "
      << datestream.str() << " ORDER BY date ASC" ;
#ifdef VERBOSE

  cout << "Command: " << cmd.str() << endl;
#endif

  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, probably your variable "
	   << varname << " or the table " << table
	   << " does not exist" << endl;
      cout << "Message: " << e.getMessage() << endl;
      varerr.resize(0);
      var.resize(0);
      timestp.resize(0);
      runnumber.resize(0);
      return -3;
    }
  unsigned int index = 0;
  int filledrows = nrows;
  while (rs->next())
    {
      float val = rs->getFloat(3);
      if (rs->wasNull())
        {
          filledrows--;
          continue;
        }
      timestp[index] = rs->getTimestamp(1).getTime();
      runnumber[index] = rs->getInt(2);
      var[index] = val;
      varerr[index] = rs->getFloat(4);
      index++;
    }
  if (filledrows != nrows)
    {
      varerr.resize(filledrows);
      var.resize(filledrows);
      timestp.resize(filledrows);
      runnumber.resize(filledrows);
    }
  return iret;
}

int
OnlMonDBodbc::GetConnection()
{
  if (con)
    {
      return 0;
    }
  try
    {
      con = DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
    }
  catch (SQLException& e)
    {
      cout << PHWHERE
	   << " Exception caught during DriverManager::getConnection" << endl;
      cout << "Message: " << e.getMessage() << endl;
      if (con)
        {
          delete con;
	  con = 0;
        }
      return -1;
    }
  printf("opened DB connection\n");
  return 0;
}
