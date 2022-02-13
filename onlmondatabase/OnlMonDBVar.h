#ifndef __ONLMONDBVAR_H__
#define __ONLMONDBVAR_H__

#include <string>

class OnlMonDBVar
{
 public:
  OnlMonDBVar();
  virtual ~OnlMonDBVar() {};

  int SetVar(const float rval[3]);
  void Print() const;
  int wasupdated() {return updated;}
  void resetupdated() {updated = 0;}
  float GetVar(const int i) {return val[i];}

 protected:
  int updated;
  float val[3];

};

#endif
