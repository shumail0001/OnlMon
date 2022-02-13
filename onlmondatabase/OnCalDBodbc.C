#include "OnCalDBodbc.h"

#include <phool/phool.h>
#include <odbc++/connection.h>
#include <odbc++/errorhandler.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>
#include <odbc++/preparedstatement.h>

#include <sstream>
#include <ctime>
#include <algorithm>
#include <cctype>

using namespace odbc;
using namespace std;

//#define VERBOSE

OnCalDBodbc::OnCalDBodbc():
  verbosity(0),
  dbname("Phenix"),
  dbowner("phnxrc"),
  dbpasswd("")
{}


void
OnCalDBodbc::identify() const
{
  cout << "DB Name: " << dbname << endl;
  cout << "DB Owner: " << dbowner << endl;
  cout << "DB Pwd: " << dbpasswd << endl;
  return ;
}

int
OnCalDBodbc::GetLastCalibratedRun(const int runno) const
{
  Connection *con = NULL;
  Statement* query = NULL;
  ostringstream cmd;
  int closestgoodrun = 329640;
  try
    {
      con = DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
    }
  catch (SQLException& e)
    {
      cout << PHWHERE
	   << " Exception caught during DriverManager::getConnection" << endl;
      cout << "Message: " << e.getMessage() << endl;
      return closestgoodrun;
    }

  query = con->createStatement();
  cmd << "SELECT runnumber FROM OnCal_status WHERE RUNNUMBER <= " << runno
      << " and dchcal > 0 and padcal > 0 and pbscgainscal > 0 and pbglqa>0 and pbglcal > 0"
      << " order by runnumber desc limit 1";
  if (verbosity > 0)
    {
      cout << "command: " << cmd.str() << endl;
    }
  ResultSet *rs = 0;
  try
    {
      rs = query->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught" << endl;
      cout << "Message: " << e.getMessage() << endl;
    }
  if (rs && rs->next())
    {
      closestgoodrun = rs->getInt("runnumber");
    }
  delete con;
  return closestgoodrun;
}
