#ifndef SPIN_SPINMONDRAW_H
#define SPIN_SPINMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <map>
#include <string>  // for allocator, string

class TCanvas;
class TH1;
class TPad;

class SpinMonDraw : public OnlMonDraw
{
 public:
  SpinMonDraw(const std::string &name);

  ~SpinMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

  static const int NTRIG = 16;

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawThird(const std::string &what = "ALL");
  TCanvas *TC[3] = {nullptr};
  TPad *transparent[3] = {nullptr};
  TPad *Pad[30] = {nullptr};

  std::map<std::string, std::string> preset_pattern_blue;
  std::map<std::string, std::string> preset_pattern_yellow;

  std::string TH1_to_string(TH1 *histo);

  int DrawGL1pRatio(const std::string &t1, const std::string &t2);

  // std::map<std::string, int> gl1ptriggers;

  std::map<std::string, TH1 *> gl1ptriggers;

  TH1 *gl1_counter[NTRIG] = {nullptr};
};

#endif /* SPIN_SPINMONDRAW_H */
