#ifndef __ONCALDBODBC_H__
#define __ONCALDBODBC_H__

#include <string>

class OnCalDBodbc
{
 public:
  OnCalDBodbc();
  virtual ~OnCalDBodbc() {}
  //  void Dump(const int nrows = 0) const;
  void identify() const;
  int GetLastCalibratedRun(const int runno) const;
  void Verbosity(const int i) { verbosity = i; }

 private:
  int verbosity;
  std::string dbname;
  std::string dbowner;
  std::string dbpasswd;
};

#endif
