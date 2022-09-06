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
 protected:
  // for testing/debugging without unpacker, remove later
  TRandom* rng = 0x0;
  const double HITS_PER_EVENT = 16.0;  //assuming on average 16 total hits per each event
  int MiscDebug();
  int InitExpectationHists();
  //~for testing/debugging without unpacker, remove later

  int DBVarInit();
  OnlMonDB* dbvars = 0x0;

  //some of these (NumEvents) can be replaced with DBVars
  TH1D* NumEvents = 0x0;

  TH2D** HitMap = 0x0;
  TH2D** HitRateMap = 0x0;

  TH1D***** ChipHitMap = 0x0;

  bool CheckIndexes(int, int, int, int);
  int GetBin(int&, int, int, int, int);
  int GetLadderNorthSouthChip(int, int, int&, int&, int&);

 public:
  InttMon(const std::string& name = "INTTMON");
  virtual ~InttMon();

  int Init();
  int BeginRun(const int);
  int process_event(Event*);
  int Reset();
};

#endif
