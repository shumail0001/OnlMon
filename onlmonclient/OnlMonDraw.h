#ifndef __ONLMONDRAW_H__
#define __ONLMONDRAW_H__

#include <string>

class Event;

class OnlMonDraw
{
 public:
  OnlMonDraw(const std::string &name = "NONE");
  virtual ~OnlMonDraw() {}

  virtual int Init() {return 0;}
  virtual int Draw(const char *what = "ALL");
  virtual int MakePS(const char *what = "ALL");
  virtual int MakeHtml(const char *what = "ALL");
  const char *Name() const {return ThisName.c_str();}
  void Verbosity(const int i) {verbosity = i;}
  int Verbosity() const {return verbosity;}

 protected:
  std::string ThisName;
  int verbosity;
};

#endif /* __ONLMONDRAW_H__ */

