#ifndef INTT_EXECS_H
#define INTT_EXECS_H

#include "InttConstants.h"

#include <TCanvas.h>
#include <TLine.h>
#include <TPad.h>
#include <TStyle.h>
#include <TText.h>

#include <TH1D.h>
#include <TH2D.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//===	Constants	===//
const int MAX_EXEC_PADS = 20;
//===	~Constants	===//

//===	Pointers	===//
//Pointers to draw HitMap at the chip/channel level
TStyle* InttHitMapStyle = 0x0;
TCanvas* InttHitMapCanvas = 0x0;
TPad* InttHitMapPad[MAX_EXEC_PADS] = {0x0};
TH1D* InttHitMap = 0x0;

//Pointers to draw ADC at the chip/channel level
TStyle* InttADCStyle = 0x0;
TCanvas* InttADCCanvas = 0x0;
TPad* InttADCPad[MAX_EXEC_PADS] = {0x0};
TH2D* InttADC = 0x0;

//..
//===	~Pointers	===//

//===	Methods		===//
void HitMapExec(int layer);
void ADCExec(int layer);

//...
//===	~Methods	===//

#endif
