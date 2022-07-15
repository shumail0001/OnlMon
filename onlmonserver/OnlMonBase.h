#ifndef __ONLMONBASE_H__
#define __ONLMONBASE_H__

#include <string>

class OnlMonBase
{
 public:
  OnlMonBase(const std::string &name = "NONE");
  virtual ~OnlMonBase() {}

  const std::string Name() const { return ThisName; }
  virtual void Verbosity(const int i) { verbosity = i; }
  virtual int Verbosity() const { return verbosity; }

 protected:
  std::string ThisName;
  int verbosity = 0;
};

#endif /* __ONLMONBASE_H__ */
