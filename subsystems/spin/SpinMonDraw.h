#ifndef SPIN_SPINMONDRAW_H
#define SPIN_SPINMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <map>
#include <string>  // for allocator, string
#include <TH2.h>

class OnlMonDB;
class TCanvas;
class TGraphErrors;
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
  TCanvas *TC[2] = {nullptr};
  TPad *transparent[2] = {nullptr};
  TPad *Pad[29] = {nullptr};


  std::map<std::string,std::string> preset_pattern_blue;
  std::map<std::string,std::string> preset_pattern_yellow;

  std::string TH1_to_string(TH1* histo);

  int DrawGL1pRatio(std::string t1, std::string t2);

  //std::map<std::string, int> gl1ptriggers;

  std::map<std::string, TH1*> gl1ptriggers;

  TH1I* gl1_counter[NTRIG];


};

#endif /* SPIN_SPINMONDRAW_H */
