#ifndef __ONLMONDB_H__
#define __ONLMONDB_H__

#include <onlmon/OnlMonBase.h>

#include <ctime>
#include <map>
#include <string>
#include <vector>

class OnlMonDBVar;
class OnlMonDBodbc;

class OnlMonDB : public OnlMonBase
{
 public:
  OnlMonDB(const std::string &thisname = "DUMMY");
  virtual ~OnlMonDB();

  int registerVar(const std::string &varname);
  int SetVar(const std::string &varname, const float var, const float varerr, const float varqual);
  int SetVar(const std::string &varname, const float var[3]);
  int DBcommit();
  int DBcommitTest();
  int DBInit();
  void Print() const;
  int GetVar(const time_t begin, const time_t end, const std::string &varname, std::vector<time_t> &timestp, std::vector<int> &runnumber, std::vector<float> &var, std::vector<float> &varerr);
  void Reset();  // reset variables (set update flag to 0 to not mix runs)

 protected:
  std::map<const std::string, OnlMonDBVar *> varmap;
  OnlMonDBodbc *db;
};

#endif
