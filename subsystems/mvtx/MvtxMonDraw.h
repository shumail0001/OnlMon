#ifndef MVTX_MVTXMONDRAW_H
#define MVTX_MVTXMONDRAW_H

#include <onlmon/OnlMonDraw.h>

#include <string>  // for allocator, string

class OnlMonDB;
class TCanvas;
class TGraphErrors;
class TPad;
class OnlMonClient;

class MvtxMonDraw : public OnlMonDraw
{
 public:
  MvtxMonDraw(const std::string &name);

  ~MvtxMonDraw() override {}

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakePS(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;

     const static int NSTAVE = 48;
  const static int NCHIP = 9;
      const static int NLAYERS = 3;
  static constexpr int NFlags = 3;
  const int NStaves[3] = { 12, 16, 20 };
  std::string mLaneStatusFlag[NFlags] = { "WARNING", "ERROR", "FAULT" };
  template <typename T>
  int PublishHistogram(TCanvas *c, int pad, T h, const char* opt = "");
  template <typename T>
  int PublishHistogram(TPad *p, T h, const char* opt = "");
  template <typename T>
  int PublishHistogram(TPad *p, int pad, T h, const char* opt = "");
  void PublishStatistics(TCanvas *c,OnlMonClient *cl );

 protected:
  int MakeCanvas(const std::string &name);
  int DrawFirst(const std::string &what = "ALL");
  int DrawSecond(const std::string &what = "ALL");
  int DrawHitMap(const std::string &what = "ALL");
  int DrawGeneral(const std::string &what = "ALL");
  int DrawFEE(const std::string &what = "ALL");
  int DrawOCC(const std::string &what = "ALL");
  int DrawFHR(const std::string &what = "ALL");
  int DrawHistory(const std::string &what = "ALL");
  int TimeOffsetTicks = -1;
  TCanvas *TC[8] = {nullptr};
  TPad *transparent[3] = {nullptr};
  TPad *Pad[9] = {nullptr};
  TGraphErrors *gr[2] = {nullptr};
  OnlMonDB *dbvars = nullptr;

 private:
const int NFlx = 6;
 static constexpr int NCols = 1024;
  static constexpr int NRows = 512;
  static constexpr int NPixels = NRows * NCols;



};

#endif /* MVTX_MVTXMONDRAW_H */
