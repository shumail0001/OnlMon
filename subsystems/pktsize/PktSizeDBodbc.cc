#include "PktSizeDBodbc.h"

#include <phool/phool.h>

#include <odbc++/connection.h>
#include <odbc++/setup.h>
#include <odbc++/types.h>
#include <odbc++/errorhandler.h>
#include <sql.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>
#include <odbc++/resultsetmetadata.h>
#include <iostream>

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <set>
#include <sstream>

using namespace odbc;
using namespace std;

static Connection *con = NULL;

PktSizeDBodbc::PktSizeDBodbc(const string &name): OnlMonBase(name)
{
  dbname = "OnlMonDB";
  dbowner = "phnxrc";
  tableprefix = name;
  dbpasswd = "";
}

PktSizeDBodbc::~PktSizeDBodbc()
{
  delete con;
  con = NULL;
}

int
PktSizeDBodbc::CheckAndCreateTable(const string &name, const map<unsigned int, unsigned int> &packetsize)
{
  if (GetConnection())
    {
      return -1;
    }

  //Postgres version
  //cout << con->getMetaData()-> getDatabaseProductVersion() << endl;
  Statement* stmt = con->createStatement();
  ostringstream cmd;
  //  cmd << "SELECT COUNT(*) FROM " << name << " WHERE 1 = 2" ;
  //cmd << "select " << name << " from pg_tables where schemaname='public";
  string lowname = name;
// The bizarre cast here is needed for newer gccs
  transform(lowname.begin(), lowname.end(), lowname.begin(), (int(*)(int))tolower);
  cmd << "select * from pg_tables where tablename = '" << lowname << "'";
  //  cmd << "SELECT * FROM " << name << " LIMIT 1" ;
  if (verbosity > 0)
    {
      cout << "cmd: " << cmd.str() << endl;
    }

  ResultSet *rs = 0;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << name << " does not exist, creating it" << endl;
    }
  int iret = 0;

  cmd.str("");
  if (! rs->next())
    {
      delete rs;
      rs = 0;
      cmd << "CREATE TABLE " << name << "(runnumber int NOT NULL, events int NOT NULL";
      map<unsigned int, unsigned int>::const_iterator iter;
      for (iter = packetsize.begin(); iter != packetsize.end(); iter++)
        {
          cmd << ", p_" << iter->first << " float DEFAULT 0";
        }
      cmd << ", primary key(runnumber))";
      if (verbosity > 0)
        {
          cout << "Executing " << cmd.str() << endl;
        }
      try
        {
          iret = stmt->executeUpdate(cmd.str());
        }
      catch (SQLException& e)
        {
          cout << "Exception caught, Message: " << e.getMessage() << endl;
        }
    }
  delete stmt;
  return iret;
}


int
PktSizeDBodbc::AddRow(const string &granulename, const int runnumber, const int nevnts, const map<unsigned int, unsigned int> &packetsize)
{
  string table = tableprefix + granulename;

  CheckAndCreateTable(table, packetsize);


  int iret = 0;
  ostringstream cmd;

  if (GetConnection())
    {
      return -1;
    }

  Statement* stmt = con->createStatement();

  // check if an entry for this run exists already
  cmd << "SELECT events FROM " << table << " where runnumber = "
      << runnumber ;
  ResultSet *rs;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, Message: " << e.getMessage() << endl;
      return -1;
    }
  if (rs->next())
    {
      int events = rs->getInt("events");
      if (nevnts <= events)
        {
          cout << "Run " << runnumber << " already in table "
	       << table << " extracted from " << events << " Events"
	       << endl;
          cout << "Run more events than " << events
	       << " if you want to overwrite this entry" << endl;
          delete rs;
          return 0;
        }
      else
        {
          cmd.str("");
          cmd << "DELETE FROM " << table << " WHERE runnumber = " << runnumber;
          Statement* stmt2 = con->createStatement();
          stmt2->executeUpdate(cmd.str());
        }
    }
  delete rs;
  // update columns in table in case there is a new packet
  CheckAndAddColumns(table, packetsize);


  // now add the content
  map<unsigned int, unsigned int>::const_iterator iter;
  cmd.str("");
  ostringstream cmd1, cmd2;
  cmd1 << "INSERT INTO " << table
       << " (runnumber, events";
  cmd2 << " VALUES(" << runnumber << ", " << nevnts;
  float size_in_bytes;
  for (iter = packetsize.begin(); iter != packetsize.end(); iter++)
    {
      cmd1 << ", p_" << iter->first;
      size_in_bytes = (float) (iter->second) / (float) (nevnts);
      size_in_bytes *= 4; // convert from 32 bit words to bytes
      cmd2 << ", " << size_in_bytes;
    }
  cmd1 << ")";
  cmd2 << ")";
  cmd << cmd1.str() << cmd2.str();
  try
    {
      stmt->executeUpdate(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, Message: " << e.getMessage() << endl;
    }

  return iret;
}


int
PktSizeDBodbc::GetPacketContent(map<unsigned int, float> &packetsize, const int runnumber, const string &granulename)
{
  if (GetConnection())
    {
      return -1;
    }
  int iret = 0;
  string table = tableprefix + granulename;

  Statement* query = con->createStatement();
  ostringstream cmd;
  cmd << "SELECT * FROM " << table << " WHERE runnumber = " << runnumber ;

  if (verbosity > 0)
    {
      cout << "command: " << cmd.str() << endl;
    }

  ResultSet *rs;
  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      string errmsg = e.getMessage();
      if (errmsg.find("does not exist") == string::npos)
        {
          cout << "Exception caught, when accessing table "
	       << table << endl;
          cout << "Message: " << e.getMessage() << endl;
        }
      return -1;
    }
  if (rs->next())
    {
      for (int i = 1; i <= rs->getMetaData()->getColumnCount(); i++)
        {
          string colname = rs->getMetaData()->getColumnName(i);
          if (colname == "runnumber" || colname == "events")
            {
              continue;
            }
          string packet = colname.substr(colname.find_last_of("_") + 1);
          istringstream istr(packet);
          unsigned int ipkt;
          istr >> ipkt;
          float size = rs->getFloat(colname);
          if (rs->wasNull() || size <= 1.)
            {
              continue;
            }
          packetsize[ipkt] = size/4.; // convert from bytes to long words
        }
      delete rs;
    }
  return iret;
}

int
PktSizeDBodbc::GetConnection()
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

int
PktSizeDBodbc::CheckAndAddColumns(const string &table, const map<unsigned int, unsigned int> &packetsize)
{
  if (GetConnection())
    {
      return -1;
    }
  Statement* stmt = con->createStatement();
  ostringstream cmd;
  cmd << "SELECT * FROM " << table << " limit 1 ";
  ResultSet *rs;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, Message: " << e.getMessage() << endl;
      return -1;
    }
  map<unsigned int, unsigned int>::const_iterator iter;
  set
    <unsigned int> packetids;
  for (iter = packetsize.begin(); iter != packetsize.end(); iter++)
    {
      packetids.insert(iter->first);
    }
  for (int i = 1; i <= rs->getMetaData()->getColumnCount(); i++)
    {

      string colname = rs->getMetaData()->getColumnName(i);
      if (colname == "runnumber" || colname == "events")
        {
          continue;
        }
      string packet = colname.substr(colname.find_last_of("_") + 1);
      istringstream istr(packet);
      unsigned int ipkt;
      istr >> ipkt;
      packetids.erase(ipkt);
    }
  delete rs;
  set
    <unsigned int>::const_iterator siter;
  for (siter = packetids.begin(); siter != packetids.end(); siter++)
    {
      ostringstream newcol;
      newcol.str("");
      newcol << "p_" << *siter;
      cmd.str("");
      cmd << "ALTER TABLE " << table << " ADD COLUMN " << newcol.str()
	  << " float DEFAULT 0";
      try
        {
          stmt->executeUpdate(cmd.str());
        }
      catch (SQLException& e)
        {
          cout << "Exception caught, Message: " << e.getMessage() << endl;
        }

    }
  return 0;
}
