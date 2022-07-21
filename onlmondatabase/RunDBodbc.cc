#include "RunDBodbc.h"

#include <phool/phool.h>

#include <boost/tokenizer.hpp>

#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/statement.h>  // for Statement
#include <odbc++/types.h>      // for SQLException

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <odbc++/resultset.h>
#pragma GCC diagnostic pop

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

//#define VERBOSE

void RunDBodbc::identify() const
{
  std::cout << "DB Name: " << dbname << std::endl;
  std::cout << "DB Owner: " << dbowner << std::endl;
  std::cout << "DB Pwd: " << dbpasswd << std::endl;
  return;
}

std::string
RunDBodbc::RunType(const int runno) const
{
  std::string runtype = "UNKNOWN";
  odbc::Connection *con = nullptr;
  odbc::Statement *query = nullptr;
  odbc::ResultSet *rs = nullptr;
  std::ostringstream cmd;

  try
  {
    con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
  }
  catch (odbc::SQLException &e)
  {
    std::cout << PHWHERE
              << " Exception caught during DriverManager::getConnection" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
    goto noopen;
  }

  query = con->createStatement();
  cmd << "SELECT runtype,runstate,eventsinrun,brunixtime,erunixtime FROM RUN WHERE RUNNUMBER = "
      << runno;
  if (verbosity > 0)
  {
    std::cout << "command: " << cmd.str() << std::endl;
  }
  try
  {
    rs = query->executeQuery(cmd.str());
  }
  catch (odbc::SQLException &e)
  {
    std::cout << "Exception caught" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
  }
  if (rs && rs->next())
  {
    runtype = rs->getString("runtype");
    if (runtype == "PHYSICS")
    {
      std::string runstate = rs->getString("runstate");
      unsigned int brunixtime = rs->getInt("brunixtime");
      unsigned int erunixtime = rs->getInt("erunixtime");
      if (erunixtime - brunixtime < 300 && runstate == "ENDED")  // 5 min limit
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
            std::cout << "Run not ended and eventsinrun : " << eventsinrun << std::endl;
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
          catch (odbc::SQLException &e)
          {
            std::cout << "Exception caught" << std::endl;
            std::cout << "Message: " << e.getMessage() << std::endl;
          }
          if (rs1 && rs1->next())
          {
            eventsinrun = rs1->getLong(1);
          }
          if (verbosity > 0)
          {
            std::cout << "Run not ended and eventsinrun < 500000, sum of scaled triggers: "
                      << eventsinrun << std::endl;
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
      std::cout << "Run unknown in DB trying from file" << std::endl;
    }
    runtype = RunTypeFromFile(runno, runtype);
  }

  if (verbosity > 0)
  {
    std::cout << "Run Type is " << runtype << std::endl;
  }

  return runtype;
}

std::string
RunDBodbc::RunTypeFromFile(const int runno, const std::string &runtype) const
{
  std::ostringstream runfilename;
  std::string returnruntype = runtype;
  if (getenv("ONLINE_LOG"))
  {
    runfilename << getenv("ONLINE_LOG") << "/runinfo/";
  }
  runfilename << "beginrun_" << std::setw(7) << std::setfill('0') << runno << ".sql";
  std::cout << "file: " << runfilename.str() << std::endl;
  std::ifstream infile(runfilename.str());
  if (infile.fail())
  {
    std::cout << "Failed to open file " << runfilename.str() << std::endl;
    return returnruntype;
  }
  std::string FullLine;
  getline(infile, FullLine);
  boost::char_separator<char> sep("'");
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  while (!infile.eof())
  {
    if (FullLine.find("INSERT INTO run VALUES") != std::string::npos)
    {
      tokenizer tokens(FullLine, sep);
      tokenizer::iterator tok_iter = tokens.begin();
      ++tok_iter;
      //	 	  std::cout << "run type: " << *tok_iter << std::endl;
      returnruntype = *tok_iter;
      break;
    }
    getline(infile, FullLine);
  }
  return returnruntype;
}

int RunDBodbc::GetRunNumbers(std::set<int> &result, const std::string &type, const int nruns, const int lastrunexclusive) const
{
  odbc::Connection *con = 0;
  odbc::Statement *query = 0;
  odbc::ResultSet *rs = 0;
  std::ostringstream cmd;

  try
  {
    con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
  }
  catch (odbc::SQLException &e)
  {
    std::cout << PHWHERE
              << " Exception caught during DriverManager::getConnection" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
    return -1;
  }

  query = con->createStatement();
  cmd << "SELECT runnumber FROM RUN WHERE eventsinrun > 100000 and runtype = '"
      << type << "' and runnumber < "
      << lastrunexclusive
      << " order by runnumber desc limit " << nruns;
  if (verbosity > 0)
  {
    std::cout << "command: " << cmd.str() << std::endl;
  }
  try
  {
    rs = query->executeQuery(cmd.str());
  }
  catch (odbc::SQLException &e)
  {
    std::cout << "Exception caught" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
    delete con;
  }
  while (rs->next())
  {
    int runnumber = rs->getInt("runnumber");
    result.insert(runnumber);
    if (verbosity > 0)
    {
      std::cout << "Choosing " << runnumber << std::endl;
    }
  }
  delete rs;
  delete con;
  return 0;
}
