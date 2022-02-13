#ifndef __RUNDBODBC_H__
#define __RUNDBODBC_H__

#include <set>
#include <string>

class RunDBodbc
{

 public:
  RunDBodbc();
  virtual ~RunDBodbc() {}
  //  void Dump(const int nrows = 0) const;
  void identify() const;
  std::string RunType(const int runno = 0) const;
  void Verbosity(const int i) {verbosity = i;}
  std::string RunTypeFromFile(const int runno, const std::string &runtype) const;
  int GetRunNumbers(std::set<int> &result, const std::string &type, const int nruns, const int lastrunexclusive) const;

 private:
  int verbosity;
  std::string dbname;
  std::string dbowner;
  std::string dbpasswd;
};

#endif
