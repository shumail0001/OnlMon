#ifndef INTT_MON_H
#define INTT_MON_H

#include "InttConstants.h"

TH1D* NumEvents;

TH2D** HitMap;
TH2D** HitRateMap;

TH1D***** ChipHists;

void InitPtrs();
void DeletePtrs();
void FillHists();

TH2D* GetHitMap(int);
TH2D* GetHitRateMap(int);
TH1D* GetChipHitMap(int, int, int, int);

bool CheckIndexes(int, int, int, int);
void GetBin(int&, int, int, int, int);
void GetLadderNorthSouthChip(int, int, int&, int&, int&);

#endif
