#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include "InttFelixMap.h"
#include "InttMon.h"
#include "InttMonConstants.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonDraw.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <TLine.h>
#include <TPolyLine.h>
#include <TText.h>

#include <TH1D.h>
#include <TH2D.h>

#include <cctype>
#include <cmath>
#include <ctime>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class InttMonDraw : public OnlMonDraw
{
 public:
  // InttMonDraw.cc
  InttMonDraw(std::string const&);
  ~InttMonDraw() override;

  int Init() override;
  int Draw(std::string const& = "ALL") override;
  int MakeHtml(std::string const& = "ALL") override;
  int SavePlot(std::string const& = "ALL", std::string const& = "png") override;

  typedef void (*HeadFunc_t)(std::string const&);
  typedef void (*GlobalHistsPrepFunc_t)(std::string const&, TH2D**);
  typedef void (*LocalHistPrepFunc_t)(std::string const&, TH2D**, struct INTT::Indexes_s&);
  struct Option_s
  {
    HeadFunc_t head = nullptr;
    GlobalHistsPrepFunc_t global = nullptr;
    LocalHistPrepFunc_t local = nullptr;
  };
  typedef std::map<std::string, struct Option_s> Options_t;
  static Options_t OPTIONS;

  // Intt_o_Helpers.cc
  struct Margin_s
  {
    double t = 0.1;
    double b = 0.1;
    double l = 0.1;
    double r = 0.1;
  };
  void static DrawPad(TPad*, TPad*, struct Margin_s const& = (struct Margin_s){
    .t = 0.1, .b = 0.1, .l = 0.1, .r = 0.1});
  void static CdPad(TPad*);

  Color_t static GetFeeColor(int const&);

  // Intt_o_FelixBcoFphxBco.cc
  struct FelixBcoFphxBco_s
  {
	double cnvs_width, cnvs_height;
	double disp_frac, lgnd_frac;
	double disp_text_size;
	double lgnd_box_width, lgnd_box_height, lgnd_text_size;
	std::string name;
  } static const m_FelixBcoFphxBco;
  void static DrawFelixBcoFphxBco(std::string const&);
  void static DrawFelixBcoFphxBco_DispPad();
  void static DrawFelixBcoFphxBco_LgndPad();
  void static DrawFelixBcoFphxBco_SubPads();
  void static DrawFelixBcoFphxBco_SubPad(int);

  // In Intt_o_Whatever.cc
  // To be removed
  static void DrawBcoDiff(std::string const&);
  static void DrawHitsVsEvt(std::string const&);

  static void GlobalChipLocalChannelHead(std::string const&);
  static void DrawGlobalChipMap(std::string const&);
  static void InttGlobalChipExec(const std::string&, int);
  static void DrawLocalChannelMap(std::string const&, struct INTT::Indexes_s);
  static void InttLocalChannelExec(const std::string&, int, int, int, int);

  static void GlobalLadderLocalChipHead(std::string const&);
  static void DrawGlobalLadderMap(std::string const&);
  static void InttGlobalLadderExec(const std::string&, int);
  static void DrawLocalChipMap(std::string const&, struct INTT::Indexes_s);
  static void InttLocalChipExec(const std::string&, int, int, int);

  static void PrepGlobalChipHists_Hitmap(std::string const&, TH2D**);
  static void PrepLocalChannelHists_Hitmap(std::string const&, TH2D**, struct INTT::Indexes_s&);
  static void PrepGlobalChipHists_NLL(std::string const&, TH2D**);

  static void PrepGlobalLadderHists_Interface(std::string const&, TH2D**);
  static void PrepLocalChipHists_Hitmap(std::string const&, TH2D**, struct INTT::Indexes_s&);

 private:
  // InttMonDraw.cc
  int MakeCanvas(const std::string& name);
  int DrawServerStats();
  TCanvas* TC[1] = {nullptr};
  TPad* transparent[1] = {nullptr};

  //===  Constants for Drawing  ===//
  static constexpr double NUM_SIG = 2.0;

  static constexpr int CNVS_WIDTH = 1280;
  static constexpr int CNVS_HEIGHT = 720;

  static constexpr double T_MARGIN = 0.01;
  static constexpr double B_MARGIN = 0.01;
  static constexpr double L_MARGIN = 0.01;
  static constexpr double R_MARGIN = 0.08;

  static constexpr double TOP_FRAC = 0.05;
  static constexpr double DISP_FRAC = 0.1;
  static constexpr double DISP_TEXT_SIZE = 0.20;
  static constexpr double LEGEND_TEXT_SIZE = 0.10;

  static constexpr double KEY_FRAC = 0.0;
  static constexpr double LEGEND_FRAC = 0.1;
  static constexpr double Y_LABEL_FRAC = 0.05;
  static constexpr double X_LABEL_FRAC = 0.15;
  static constexpr double Y_LABEL_TEXT_SIZE = 0.5;
  static constexpr double X_LABEL_TEXT_SIZE = 0.5;
  //===  ~Constants for Drawing  ===//
};

#endif
