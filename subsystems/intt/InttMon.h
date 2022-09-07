#ifndef INTT_MON_H
#define INTT_MON_H

#include "InttConstants.h"

#include <onlmon/OnlMon.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TRandom.h>  //for rng; remove later

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class InttMon : public OnlMon
{
 public:
  InttMon(const std::string &name = "INTTMON"): OnlMon(name){}
  virtual ~InttMon();

  int Init();
  int BeginRun(const int);
  int process_event(Event*);
  int Reset();

 protected:
  // for testing/debugging without unpacker, remove later
  TRandom* rng = nullptr;
  const double HITS_PER_EVENT = 16.0;  //assuming on average 16 total hits per each event
  int MiscDebug();
  int InitExpectationHists();
  //~for testing/debugging without unpacker, remove later

  int DBVarInit();
  OnlMonDB* dbvars = nullptr;

  //some of these (NumEvents) can be replaced with DBVars
  TH1D* NumEvents = nullptr;

  TH2D** HitMap = nullptr;
  TH2D** HitRateMap = nullptr;

  TH1D***** ChipHitMap = nullptr;

  bool CheckIndexes(int, int, int, int);
  int GetBin(int&, int, int, int, int);
  int GetLadderNorthSouthChip(int, int, int&, int&, int&);

};

#endif
