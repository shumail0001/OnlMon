#include "RunDBodbc.h"

#include <phool/phool.h>

#include <boost/tokenizer.hpp>

#include <odbc++/connection.h>
#include <odbc++/errorhandler.h>
#include <odbc++/drivermanager.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <odbc++/resultset.h>
#include <odbc++/preparedstatement.h>
#pragma GCC diagnostic pop

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

//using namespace odbc;
using namespace std;

//#define VERBOSE

RunDBodbc::RunDBodbc():
  verbosity(0),
  dbname("daq"),
  dbowner("phnxrc"),
  dbpasswd("")
{}


void
RunDBodbc::identify() const
{
  cout << "DB Name: " << dbname << endl;
  cout << "DB Owner: " << dbowner << endl;
  cout << "DB Pwd: " << dbpasswd << endl;
  return ;
}

string
RunDBodbc::RunType(const int runno) const
{
  string runtype = "UNKNOWN";
  odbc::Connection *con = NULL;
  odbc::Statement* query = NULL;
  odbc::ResultSet *rs = NULL;
  ostringstream cmd;

  try
    {
      con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
    }
  catch (odbc::SQLException& e)
    {
      cout << PHWHERE
	   << " Exception caught during DriverManager::getConnection" << endl;
      cout << "Message: " << e.getMessage() << endl;
      goto noopen;
    }

  query = con->createStatement();
  cmd << "SELECT runtype,runstate,eventsinrun,brunixtime,erunixtime FROM RUN WHERE RUNNUMBER = "
      << runno;
  if (verbosity > 0)
    {
      cout << "command: " << cmd.str() << endl;
    }
  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      cout << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
    }
  if (rs && rs->next())
    {
      runtype = rs->getString("runtype");
      if (runtype == "PHYSICS")
	{
	  string runstate = rs->getString("runstate");
	  unsigned int brunixtime = rs->getInt("brunixtime");
	  unsigned int erunixtime = rs->getInt("erunixtime");
	  if (erunixtime - brunixtime < 300 && runstate == "ENDED") // 5 min limit
	    {
	      runtype = "PREJECTED";
	    }
	  else
	    {
	      int eventsinrun = rs->getInt("eventsinrun");
	      if (eventsinrun <= 100 && runstate != "ENDED")
		{
		  if (verbosity > 0)
		    {
		      cout << "Run not ended and eventsinrun : " << eventsinrun << endl;
		    }
		  cmd.str("");
		  cmd << "SELECT sum(scalerupdatescaled) FROM trigger WHERE RUNNUMBER = "
		      << runno;

		  odbc::ResultSet *rs1 = 0;

		  odbc::Statement *query1 = con->createStatement();
		  try
		    {
		      rs1 = query1->executeQuery(cmd.str());
		    }
		  catch (odbc::SQLException& e)
		    {
		      cout << "Exception caught" << endl;
		      cout << "Message: " << e.getMessage() << endl;
		    }
		  if (rs1 && rs1->next())
		    {
		      eventsinrun = rs1->getLong(1);
		    }
		  if (verbosity > 0)
		    {
		      cout << "Run not ended and eventsinrun < 500000, sum of scaled triggers: "
			   << eventsinrun << endl;
		    }
		}
	      if (eventsinrun <= 100)
		{
		  runtype = "PREJECTED";
		}
	    }
	}
    }
 noopen:
  if (con)
    {
      delete con;
    }

  // try to get this info from the beginrun sql command saved in $ONLINE_LOG/runinfo
  if (runtype == "UNKNOWN")
    {
      if (verbosity > 0)
	{
	  cout << "Run unknown in DB trying from file" << endl;
	}
      runtype = RunTypeFromFile(runno, runtype);
    }

  if (verbosity > 0)
    {
      cout << "Run Type is " << runtype << endl;
    }

  return runtype;
}

string
RunDBodbc::RunTypeFromFile(const int runno, const string &runtype) const
{
  ostringstream runfilename;
  string returnruntype = runtype;
  if (getenv("ONLINE_LOG"))
    {
      runfilename << getenv("ONLINE_LOG") << "/runinfo/";
    }
  runfilename << "beginrun_" << setw(7) << setfill('0') << runno << ".sql";
  cout << "file: " << runfilename.str() << endl;
  ifstream infile(runfilename.str().c_str());
  if (infile.fail ())
    {
      cout << "Failed to open file " << runfilename.str() << endl;
      return returnruntype;
    }
  string FullLine;
  getline(infile, FullLine);
  boost::char_separator<char> sep("'");
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  while ( !infile.eof() )
    {
      if (FullLine.find("INSERT INTO run VALUES") != string::npos)
	{
	  tokenizer tokens(FullLine,sep);
	  tokenizer::iterator tok_iter = tokens.begin();
	  ++tok_iter;
	  //	 	  cout << "run type: " << *tok_iter << endl;
	  returnruntype = *tok_iter;
	  break;
	}
      getline(infile, FullLine);
    }
  return returnruntype;
}

int
RunDBodbc::GetRunNumbers(std::set<int> &result, const std::string &type, const int nruns, const int lastrunexclusive) const
{
  odbc::Connection *con = 0;
  odbc::Statement* query = 0;
  odbc::ResultSet *rs = 0;
  ostringstream cmd;

  try
    {
      con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
    }
  catch (odbc::SQLException& e)
    {
      cout << PHWHERE
	   << " Exception caught during DriverManager::getConnection" << endl;
      cout << "Message: " << e.getMessage() << endl;
      return -1;
    }

  query = con->createStatement();
  cmd << "SELECT runnumber FROM RUN WHERE eventsinrun > 100000 and runtype = '"
      << type << "' and runnumber < "
      << lastrunexclusive
      << " order by runnumber desc limit " << nruns;
  if (verbosity > 0)
    {
      cout << "command: " << cmd.str() << endl;
    }
  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (odbc::SQLException& e)
    {
      cout << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
      delete con;
    }
  while (rs->next())
    {
      int runnumber = rs->getInt("runnumber");
      result.insert(runnumber);
      if (verbosity > 0)
	{
	  cout << "Choosing " << runnumber << endl;
	}
    }
  delete rs;
  delete con;
  return 0;
}
