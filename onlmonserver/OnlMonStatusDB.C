#include "OnlMonStatusDB.h"

#include <phool/phool.h>

#include <odbc++/connection.h>
#include <odbc++/errorhandler.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>

#include <iostream>
#include <sstream>

using namespace std;

static odbc::Connection *con = NULL;

OnlMonStatusDB::OnlMonStatusDB(const std::string &tablename):
  dbname("OnlMonDB"),
  dbowner("phnxrc"),
  dbpasswd(""),
  table(tablename)
{}

OnlMonStatusDB::~OnlMonStatusDB()
{
  delete con;
  con = NULL;
}

int
OnlMonStatusDB::CheckAndCreateTable()
{
  if (GetConnection())
    {
      return -1;
    }

  //Postgres version
  //cout << con->getMetaData()-> getDatabaseProductVersion() << endl;
  odbc::Statement* stmt = con->createStatement();
  ostringstream cmd;
  //  cmd << "SELECT COUNT(*) FROM " << name << " WHERE 1 = 2" ;
 cmd << "SELECT * FROM pg_tables where tablename = '" << table << "'" ;
#ifdef VERBOSE

  cout << cmd.str() << endl;
#endif

  odbc::ResultSet *rs = NULL;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      string message = e.getMessage();
      if (message.find("does not exist") == string::npos)
        {
          cout << "Exception caught" << endl;
          cout << "Message: " << e.getMessage() << endl;
        }
    }
  cmd.str("");
  if (! rs->next())
    {
      delete rs;
      cmd << "CREATE TABLE " << table << "(runnumber int, primary key(runnumber))" ;
      try
	{
	  stmt->executeUpdate(cmd.str());
	}
      catch (odbc::SQLException& e)
	{
	  cout << "caught exception Message: " << e.getMessage() << endl;
	}
    }
  return 0;
}

int
OnlMonStatusDB::CheckAndCreateMonitor(const string &name)
{
  ostringstream cmd;
  if (CheckAndCreateTable())
    {
      cout << "Problem creating " << table << endl;
      return -1;
    }
  cmd << "SELECT * FROM " << table << " LIMIT 1";
  odbc::ResultSet *rs = NULL;
  odbc::Statement* stmt = con->createStatement();
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      cout << PHWHERE << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
      return -1;
    }
  try
    {
      rs->findColumn(name);
    }
  catch (odbc::SQLException& e)
    {
      string exceptionmessage = e.getMessage();
      if (exceptionmessage.find("not found in result set") != string::npos)
        {
          cmd.str("");
          cmd << "ALTER TABLE "
	      << table
	      << " ADD "
	      << name
	      << " int default 0";
          try
            {
              odbc::Statement* stmtup = con->createStatement();
              stmtup->executeUpdate(cmd.str());
            }
          catch (odbc::SQLException& e)
            {
              cout << "Exception caught: " << e.getMessage() << endl;
	      return -1;
            }
        }
    }
  return 0;
}

int
OnlMonStatusDB::FindAndInsertRunNum(const int runnumber)
{
  if (GetConnection() != 0)
    {
      cout << "problem" << endl;
      return -1;
    }
  if (findRunNumInDB(runnumber) == 0)
    {
      return 0;
    }
  odbc::Statement *statement = con->createStatement();
  ostringstream cmd;
  cmd << "INSERT INTO "
      << table
      << " (runnumber) VALUES ("
      << runnumber << ")";
  try
    {
      statement->executeUpdate(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      cout << e.getMessage() << endl;
      return -1;
    }
  return 0;
}

int
OnlMonStatusDB::findRunNumInDB(const int runnumber)
{
  odbc::Statement *statement = 0;
  odbc::ResultSet *rs = 0;
  ostringstream cmd;
  cmd << "SELECT runnumber FROM "
      << table
      << " WHERE runnumber = "
      << runnumber;

  statement = con->createStatement();

  try
    {
      rs = statement->executeQuery(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      cout << "exception caught: " << e.getMessage() << endl;
      return -1;
    }

  if (rs->next())
    {
      try
        {
           rs->getInt("runnumber");
        }
      catch (odbc::SQLException& e)
        {
          cout << "exception caught: " << e.getMessage() << endl;
          return -1;
        }
    }
  else
    {
      return -1;
    }
  return 0;
}


int
OnlMonStatusDB::UpdateStatus(const string &name, const int runnumber, const int status)
{
  if (CheckAndCreateMonitor(name))
    {
      cout << PHWHERE << "Problem encountered, cannot do update" << endl;
      return -1;
    }
  if (FindAndInsertRunNum(runnumber) != 0)
    {
      cout << PHWHERE << "Problem updating runnumber encountered, cannot do update" << endl;
      return -1;
    }

  ostringstream cmd;
  cmd << "Update "
      << table
      << " set " << name
      << " = " << status
      << " where runnumber = "
      << runnumber;
  odbc::Statement *stmtupd = 0;
  try
    {
      stmtupd = con->createStatement();
    }
  catch (odbc::SQLException& e)
    {
      cout << "Cannot create statement" << endl;
      cout << e.getMessage() << endl;
      return -1;
    }

  try
    {
      stmtupd->executeUpdate(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      cout << PHWHERE << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
      return -1;
    }
  return 0;
}

int
OnlMonStatusDB::GetConnection()
{
  if (con)
    {
      return 0;
    }
  try
    {
      con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
    }
  catch (odbc::SQLException& e)
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
  return 0;
}
