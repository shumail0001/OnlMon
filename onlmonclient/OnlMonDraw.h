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
  virtual int Draw(const std::string &what = "ALL");
  virtual int MakePS(const std::string &what = "ALL");
  virtual int MakeHtml(const std::string &what = "ALL");
  const std::string Name() const {return ThisName;}
  void Verbosity(const int i) {verbosity = i;}
  int Verbosity() const {return verbosity;}

 protected:
  std::string ThisName;
  int verbosity;
};

#endif /* __ONLMONDRAW_H__ */

