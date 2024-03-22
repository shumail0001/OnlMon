#ifndef LOCALPOL_LOCALPOLMONDRAW_H
#define LOCALPOL_LOCALPOLMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <string>  // for allocator, string
#include <TH2.h>

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TGraphPolar;
class TPad;

class LocalPolMonDraw : public OnlMonDraw
{
 public:
  LocalPolMonDraw(const std::string &name);

  ~LocalPolMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  double* ComputeAsymmetries(double L_U, double R_D, double L_D, double R_U);
  TCanvas *TC[2] = {nullptr};
  TPad *transparent[2] = {nullptr};
  TPad ***Pad = {nullptr};
  TGraphErrors**** g_Asym          = nullptr;
  TGraphErrors**** g_AsymScramble  = nullptr;
  TGraphPolar*** g_Polar          = nullptr;
  int iPoint;
  bool NewPoint;
};

#endif /* LOCALPOL_LOCALPOLMONDRAW_H */
