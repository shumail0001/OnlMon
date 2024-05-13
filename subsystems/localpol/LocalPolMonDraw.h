#ifndef LOCALPOL_LOCALPOLMONDRAW_H
#define LOCALPOL_LOCALPOLMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <string>  // for allocator, string
#include <TH2.h>
#include <map>

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TGraphPolar;
class TGraphPolargram;
class TPad;

class LocalPolMonDraw : public OnlMonDraw
{
 public:
  explicit LocalPolMonDraw(const std::string &name);

  ~LocalPolMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");

  TCanvas *TC[2] = {nullptr};

  TPad *transparent[2] = {nullptr};
  TPad ***Pad = nullptr;

  TH1  * hframe                   = nullptr;
  TH1  * hscrambleframe           = nullptr;
  TH1D **h_PreviousCounts         = nullptr;
  TH1D **h_PreviousCountsScramble = nullptr;

  TGraphErrors**** g_Asym         = nullptr;
  TGraphErrors**** g_AsymScramble = nullptr;
  TGraphPolar***   g_Polar        = nullptr;
  TGraphPolargram* gpolargram     = nullptr;

  TH1D**** h_Asym         = nullptr;
  TH1D**** h_ScrambleAsym = nullptr;
  std::map<int, long int> m_time;

  //int myRun;
  //int iPoint;
  //bool NewPoint;
  double thresholdNewPoint=5e3;//To be tuned by config file
};

#endif /* LOCALPOL_LOCALPOLMONDRAW_H */
