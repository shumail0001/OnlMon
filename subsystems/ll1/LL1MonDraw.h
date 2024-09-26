#ifndef LL1_LL1MONDRAW_H
#define LL1_LL1MONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <string>  // for allocator, string
#include <TLatex.h>
#include <tuple>

class TCanvas;
class TGraphErrors;
class TPad;

class LL1MonDraw : public OnlMonDraw
{
 public:
  LL1MonDraw(const std::string &name);

  ~LL1MonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawThird(const std::string &what = "ALL");
  int DrawFourth(const std::string &what = "ALL");
  int DrawFifth(const std::string &what = "ALL");
  int DrawSixth(const std::string &what = "ALL");
  int DrawSeventh(const std::string &what = "ALL");
  int DrawHistory(const std::string &what = "ALL");
  TCanvas *TC[7] = {nullptr};
  TPad *transparent[7] = {nullptr};
  TPad *Pad[6] = {nullptr};
  TPad *emcalpad[16] = {nullptr};
  const int nPad1 = 4;
  double edgediff = 0.03;
  double padx1[4] = {edgediff, 0.5+edgediff, edgediff, 0.5 +edgediff};
  double pady1[4] = {0.5+edgediff, 0.5+edgediff, edgediff, edgediff};
  double padx2[4] = {0.5-edgediff, 1-edgediff, 0.5-edgediff, 1-edgediff};
  double pady2[4] = {1-edgediff,1-edgediff,0.5-edgediff,0.5-edgediff};

};

#endif /* LL1_LL1MONDRAW_H */
