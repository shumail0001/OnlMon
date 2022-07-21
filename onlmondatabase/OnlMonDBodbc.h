#ifndef ONLMONDBODBC_H__
#define ONLMONDBODBC_H__

#include <onlmon/OnlMonBase.h>

#include <ctime>
#include <map>
#include <string>
#include <vector>

class OnlMonDBVar;
namespace odbc
{
  class ResultSet;
}

class OnlMonDBodbc : public OnlMonBase
{
 public:
  OnlMonDBodbc(const std::string &tablename);
  virtual ~OnlMonDBodbc();
  void Dump();
  int Info(const char *tablename = nullptr);
  int CreateTable();
  int CheckAndCreateTable(const std::map<const std::string, OnlMonDBVar *> &varmap);
  int DropTable(const std::string &name);
  void identify() const;
  int AddRow(const time_t ticks, const int runnumber, const std::map<const std::string, OnlMonDBVar *> &varmap);
  int GetVar(const time_t begin, const time_t end, const std::string &varname, std::vector<time_t> &timestp, std::vector<int> &runnumber, std::vector<float> &var, std::vector<float> &varerr);

 private:
  void Dump(odbc::ResultSet *rs) const;
  int GetConnection();
  std::string dbname;
  std::string dbowner;
  std::string dbpasswd;
  std::string table;
};

#endif
