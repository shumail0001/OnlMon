#ifndef ZDC_ZDCMONDRAW_H
#define ZDC_ZDCMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <string>  // for allocator, string

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;
const int NUM_CANV = 3 +1 + 1;
const int NUM_PAD = 7 + 12 + 2;

class ZdcMonDraw : public OnlMonDraw
{
 public:
  ZdcMonDraw(const std::string &name);

  ~ZdcMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

 protected:
  int MakeCanvas1(const std::string &name);
  // int MakeCanvas2(const std::string &name);
  // int MakeCanvas3(const std::string &name);
  // int MakeCanvas4(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawSmdValues(const std::string &what = "ALL");
  int DrawSmdNorthandSouth(const std::string &what = "ALL");
  int DrawSmdXY(const std::string &what = "ALL");

  TCanvas *TC[NUM_CANV] = {nullptr};
  TPad *transparent[NUM_CANV] = {nullptr};
  TPad *Pad[NUM_PAD] = {nullptr};
};

#endif /* ZDC_ZDCMONDRAW_H */
