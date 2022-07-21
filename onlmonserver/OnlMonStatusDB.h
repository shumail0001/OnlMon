#ifndef ONLMONSTATUSDB_H__
#define ONLMONSTATUSDB_H__

#include <string>

class OnlMonStatusDB
{
 public:
  OnlMonStatusDB(const std::string &tablename = "onlmonstatus");
  virtual ~OnlMonStatusDB();
  int CheckAndCreateTable();
  int CheckAndCreateMonitor(const std::string &name);
  int UpdateStatus(const std::string &name, const int runnumber, const int status);
  int FindAndInsertRunNum(const int runnumber);
  int findRunNumInDB(const int runno);

 private:
  int GetConnection();
  std::string dbname = "OnlMonDB";
  std::string dbowner = "phnxrc";
  std::string dbpasswd = "";
  std::string table;
};

#endif
