#ifndef __MYMONDRAW_H__
#define __MYMONDRAW_H__

#include <onlmon/OnlMonDraw.h>

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;

class MyMonDraw : public OnlMonDraw
{
 public:
  MyMonDraw(const std::string &name = "MYMON");
  ~MyMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakePS(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawHistory(const std::string &what = "ALL");
  int DrawDeadServer(TPad *transparent);
  int TimeOffsetTicks;
  TCanvas *TC[3];
  TPad *transparent[3];
  TPad *Pad[6];
  TGraphErrors *gr[2];
  OnlMonDB *dbvars;
};

#endif /*__MYMONDRAW_H__ */
