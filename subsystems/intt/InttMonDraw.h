#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include "InttMonDrawBase.cc" //change to .h

TStyle* HitMapStyle;
TCanvas* HitMapCanvas;
TCanvas* ChipHitMapCanvas;

TH2D* HitMapDrawHist;
TH1D* ChipHitMapDrawHist;

void DrawHitMap();
void PrepHitMap(int);

void HitMapExec();

#endif
