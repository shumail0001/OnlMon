#ifndef ZDC_ZDCMONDRAW_H
#define ZDC_ZDCMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <string>  // for allocator, string

class TH1;
class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;
class ZdcMonDraw : public OnlMonDraw
{
 public:
  ZdcMonDraw(const std::string &name);

  ~ZdcMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

 private:
  static constexpr int NUM_CANV = 2 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1;
  static constexpr int NUM_PAD = 4 + 3 + 12 + 2 + 4 + 15 + 15 + 4 + 1 + 4 + 5 + 4;


  TH1 *smd_adc_n_hor_means = nullptr;
  TH1 *smd_adc_s_hor_means = nullptr;
  TH1 *smd_adc_n_ver_means = nullptr;
  TH1 *smd_adc_s_ver_means = nullptr;

  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawSmdValues(const std::string &what = "ALL");
  int DrawSmdNorthandSouth(const std::string &what = "ALL");
  int DrawSmdXY(const std::string &what = "ALL");
  int DrawSmdAdcNorthIndividual(const std::string &what = "ALL");
  int DrawSmdAdcSouthIndividual(const std::string &what = "ALL");
  int Drawveto(const std::string &what = "ALL");
  int DrawWaveForm(const std::string &what = "ALL");
  int DrawSmdMultiplicities(const std::string &what = "ALL");
  int DrawSmdAdcMeans(const std::string &what = "ALL");

  TCanvas *TC[NUM_CANV] = {nullptr};
  TPad *transparent[NUM_CANV] = {nullptr};
  TPad *Pad[NUM_PAD] = {nullptr};
};

#endif /* ZDC_ZDCMONDRAW_H */
