#ifndef __TRIGINFO_H__
#define __TRIGINFO_H__

#include <string>

class TrigInfo
{
 public:
  TrigInfo();
  virtual ~TrigInfo() {}

  void Reset();
  void SetInfo(const std::string &name, const unsigned short ibit, const unsigned short ibitmask, const unsigned short iindex, const unsigned int iscaledown);
  std::string Name() const { return trigname; }
  unsigned short Bit() const { return bit; }
  unsigned short BitMask() const { return bitmask; }
  unsigned short Index() const { return index; }
  unsigned int ScaleDown() const { return scaledown; }

 protected:
  std::string trigname;
  unsigned short bit;
  unsigned short bitmask;
  unsigned short index;
  unsigned int scaledown;
};

#endif  //__TRIGINFO_H__
