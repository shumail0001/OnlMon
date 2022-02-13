#ifndef ONLMONTRIGGER_H_
#define ONLMONTRIGGER_H_

//#include <TriggerUtilities.h>

#include <map>
#include <set>
#include <string>

//class TriggerHelper;
//class TrigInfo;

#define NGL1TRIG 32

class OnlMonTrigger
{
 public:
  OnlMonTrigger() {}
  virtual ~OnlMonTrigger() {}
  void Print(const char *what = "ALL") const {return;}
  void RunNumber(int ) {return;}
  void TrigMask(const std::string &, const unsigned int) {return;}
  int getLevel1Bit(const std::string &) {return 0x1;}
};

#endif // __ONLMONTRIGGER_H_
