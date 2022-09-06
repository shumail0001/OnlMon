#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include "InttConstants.h"
#include "InttExecs.cc"  //maybe change to .h later

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <phool/phool.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>

#include <TLine.h>
#include <TText.h>

#include <TH1D.h>
#include <TH2D.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class InttMonDraw
{
 protected:
  //===	Constants for Drawing	===//
  constexpr static double NUM_SIG = 2.0;

  const static int CNVS_WIDTH = 1280;
  const static int CNVS_HEIGHT = 1440;

  constexpr static double T_MARGIN = 0.01;
  constexpr static double B_MARGIN = 0.01;
  constexpr static double L_MARGIN = 0.03;
  constexpr static double R_MARGIN = 0.10;

  constexpr static double KEY_TEXT_SIZE1 = 0.10;
  constexpr static double KEY_TEXT_SIZE2 = 0.05;
  constexpr static double LABEL_TEXT_SIZE1 = 0.35;
  constexpr static double LABEL_TEXT_SIZE2 = 0.25;
  constexpr static double TITLE_TEXT_SIZE = 0.50;

  constexpr static double KEY_FRAC = 0.1;
  constexpr static double LABEL_FRAC = 0.1;
  constexpr static double TITLE_FRAC = 0.1;

  const static int OPT = 3;
  const static int MAX_PADS = 20;
  const static int MAX_LINES = 2 * CHIP + 1;

  std::string WHAT[OPT] = {"HitMap", "ADC"};
  std::string EXEC_NAME[OPT] = {"hit_map_exec", "adc_exec"};
  std::string CMND_NAME[OPT] = {"HitMapExec", "ADCExec"};
  //===	~Constants for Drawing	===//

  //===	Drawing Methods		===//
  void DrawIntt(int);
  void DrawPad(TPad*);
  void DrawKey(TPad*, int);
  void DrawGrid(TPad*, int);
  void DrawLabels(TPad*, int);
  void DrawTitle(TPad*, int);

  void SetExec(TPad*, int, int);

  void DeletePtrs();
  //===	~Drawing Methods	===//

  //===	Pointers for Drawing	===//
  TStyle* InttStyle[OPT] = {0x0};
  TCanvas* InttCanvas[OPT] = {0x0};
  TPad* InttPad[OPT * LAYER * MAX_PADS] = {0x0};
  TH2D* InttHist[OPT * LAYER] = {0x0};

  TLine** key_vline[LAYER] = {0x0};
  TLine** key_hline[LAYER] = {0x0};
  TText** key_nlabel[LAYER] = {0x0};
  TText** key_slabel[LAYER] = {0x0};

  TLine** grid_vline[LAYER] = {0x0};
  TLine** grid_hline[LAYER] = {0x0};

  TText** label[LAYER] = {0x0};

  TText* title[LAYER] = {0x0};
  //===	~Pointers for Drawing	===//

 public:
  int Draw(const std::string&);
};

#endif
