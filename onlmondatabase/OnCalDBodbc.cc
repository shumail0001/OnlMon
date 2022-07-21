#include "OnCalDBodbc.h"

#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/errorhandler.h>
#include <phool/phool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <odbc++/preparedstatement.h>
#include <odbc++/resultset.h>
#pragma GCC diagnostic pop

#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>

//#define VERBOSE

void OnCalDBodbc::identify() const
{
  std::cout << "DB Name: " << dbname << std::endl;
  std::cout << "DB Owner: " << dbowner << std::endl;
  std::cout << "DB Pwd: " << dbpasswd << std::endl;
  return;
}

int OnCalDBodbc::GetLastCalibratedRun(const int runno) const
{
  odbc::Connection* con = nullptr;
  odbc::Statement* query = nullptr;
  std::ostringstream cmd;
  int closestgoodrun = 329640;
  try
  {
    con = odbc::DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
  }
  catch (odbc::SQLException& e)
  {
    std::cout << PHWHERE
              << " Exception caught during DriverManager::getConnection" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
    return closestgoodrun;
  }

  query = con->createStatement();
  cmd << "SELECT runnumber FROM OnCal_status WHERE RUNNUMBER <= " << runno
      << " and dchcal > 0 and padcal > 0 and pbscgainscal > 0 and pbglqa>0 and pbglcal > 0"
      << " order by runnumber desc limit 1";
  if (verbosity > 0)
  {
    std::cout << "command: " << cmd.str() << std::endl;
  }
  odbc::ResultSet* rs = nullptr;
  try
  {
    rs = query->executeQuery(cmd.str());
  }
  catch (odbc::SQLException& e)
  {
    std::cout << "Exception caught" << std::endl;
    std::cout << "Message: " << e.getMessage() << std::endl;
  }
  if (rs && rs->next())
  {
    closestgoodrun = rs->getInt("runnumber");
  }
  delete con;
  return closestgoodrun;
}
