#ifndef __MYMONDRAW_H__
#define __MYMONDRAW_H__

#include <OnlMonDraw.h>

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;

class MyMonDraw: public OnlMonDraw
{

 public: 
  MyMonDraw(const char *name = "MYMON");
  virtual ~MyMonDraw() {}

  int Init();
  int Draw(const char *what = "ALL");
  int MakePS(const char *what = "ALL");
  int MakeHtml(const char *what = "ALL");

 protected:
  int MakeCanvas(const char *name);
  int DrawFirst(const char *what = "ALL");
  int DrawSecond(const char *what = "ALL");
  int DrawHistory(const char *what = "ALL");
  int DrawDeadServer(TPad *transparent);
  int TimeOffsetTicks;
  TCanvas *TC[3];
  TPad *transparent[3];
  TPad *Pad[6];
  TGraphErrors *gr[2];
  OnlMonDB *dbvars;
};

#endif /*__MYMONDRAW_H__ */
