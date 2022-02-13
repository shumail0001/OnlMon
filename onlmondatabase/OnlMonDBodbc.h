#ifndef ONLMONDBODBC_H__
#define ONLMONDBODBC_H__

#include <OnlMonBase.h>

#ifndef __CINT__
#include <odbc++/resultset.h>
#endif
#include <ctime>
#include <map>
#include <string>
#include <vector>

class OnlMonDBVar;

class OnlMonDBodbc: public OnlMonBase
{

 public:
  OnlMonDBodbc(const std::string &tablename);
  virtual ~OnlMonDBodbc();
  void Dump();
  int Info(const char *tablename = NULL);
  int CreateTable();
  int CheckAndCreateTable(const std::map<const std::string, OnlMonDBVar *> &varmap);
  int DropTable(const std::string &name);
  void identify() const;
  int AddRow(const time_t ticks, const int runnumber, const std::map<const std::string, OnlMonDBVar *> &varmap);
  int GetVar(const time_t begin, const time_t end, const std::string &varname, std::vector<time_t> &timestp, std::vector<int> &runnumber, std::vector<float> &var, std::vector<float> &varerr);

 private:
#ifndef __CINT__
  void Dump(odbc::ResultSet *rs) const;
#endif
  int GetConnection();
  std::string dbname;
  std::string dbowner;
  std::string dbpasswd;
  std::string table;
};

#endif
