#ifndef SPIN_SPINMONDRAW_H
#define SPIN_SPINMONDRAW_H

#include <onlmon/OnlMonDraw.h>

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

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  TCanvas *TC[2] = {nullptr};
  TPad *transparent[2] = {nullptr};
  TPad *Pad[19] = {nullptr};
};

#endif /* SPIN_SPINMONDRAW_H */
