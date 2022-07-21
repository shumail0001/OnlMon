#ifndef ONLMONDATABASE_ONCALDBODBC_H
#define ONLMONDATABASE_ONCALDBODBC_H

#include <string>

class OnCalDBodbc
{
 public:
  OnCalDBodbc() {}
  virtual ~OnCalDBodbc() {}
  //  void Dump(const int nrows = 0) const;
  void identify() const;
  int GetLastCalibratedRun(const int runno) const;
  void Verbosity(const int i) { verbosity = i; }

 private:
  int verbosity = 0;
  std::string dbname = "Phenix";
  std::string dbowner = "phnxrc";
  std::string dbpasswd = "";
};

#endif
