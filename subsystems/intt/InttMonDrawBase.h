#ifndef INTT_MON_DRAW_BASE_H
#define INTT_MON_DRAW_BASE_H

#include "InttConstants.h"

//===	Constants for Drawing	===//
const double NUM_SIG = 2.0;

const double T_MARGIN = 0.01;
const double B_MARGIN = 0.01;
const double L_MARGIN = 0.03;
const double R_MARGIN = 0.10;

const double KEY_TEXT_SIZE1 = 0.10;
const double KEY_TEXT_SIZE2 = 0.05;
const double LABEL_TEXT_SIZE1 = 0.35;
const double LABEL_TEXT_SIZE2 = 0.25;
const double TITLE_TEXT_SIZE = 0.50;

const double KEY_FRAC = 0.1;
const double LABEL_FRAC = 0.1;
const double TITLE_FRAC = 0.1;

const int OPT = 3;
const int MAX_PADS = 20;
const int MAX_LINES = 2 * CHIP + 1;
//===	~Constants for Drawing	===//

//===	Pointers for Drawing	===//
	TPad* INTTPads[OPT * LAYER * MAX_PADS];

	TLine** key_vline[LAYER];
	TLine** key_hline[LAYER];
	TText** key_nlabel[LAYER];
	TText** key_slabel[LAYER];

	TLine** grid_vline[LAYER];
	TLine** grid_hline[LAYER];

	TText** label[LAYER];

	TText* title[LAYER];
//===	~Pointers for Drawing	===//


//===	Drawing Methods		===//
	void DrawLayer(TCanvas*, int, int, TH2D*);
	void DrawPad(TPad*);
	void DrawKey(TPad*, int);
	void DrawGrid(TPad*, int);
	void DrawLabels(TPad*, int);
	void DrawTitle(TPad*, int);

	int DeleteDrawBasePtrs();
//===	~Drawing Methods	===//

#endif
