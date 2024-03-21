#ifndef LOCALPOL_LOCALPOLMON_H
#define LOCALPOL_LOCALPOLMON_H

#include <onlmon/OnlMon.h>
#include <map>
#include <cmath>
#include <vector>

class CaloWaveformFitting;
class Event;
class TH1D;
class TH2;
class Packet;



class LocalPolMon : public OnlMon
{
 public:
  LocalPolMon(const std::string &name = "LOCALPOLMON");
  virtual ~LocalPolMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  float anaWaveformFast(Packet *p, const int channel);
  CaloWaveformFitting *WaveformProcessingFast = nullptr;

  int evtcnt = 0;
  const int packetid_gl1 = 14001;
  const int packetid_smd = 12001;

  const int UP= 1;
  const int DN=-1;

  const int BLUE=0;
  const int YELLOW=1;

  const float pitchY= 2.0 /*cm plastic scint.*/ * (11.0 / 10.5) /*(pitch correction for gap and wrapping)*/ * sin(PI/4)/*(correct for the tilt)*/;
  const float pitchX=1.5 /*cm plastic scint.*/ * (11.0 / 10.5) /*(pitch correction for gap and wrapping)*/;

  const float nchannelsY=8.0;
  const float nchannelsX=7.0;
  const float PI=3.14159;
  
  std::map<int, int> SpinPatterns[2];
  int StartAbortGapPattern;
  int StartAbortGapData;
  int CrossingShift;

  float smd_north_rgain[16];
  float smd_south_rgain[16];
  

  TH1D **h_Counts         = nullptr;
  TH1D **h_CountsScramble = nullptr;

  
};
    
#endif /* LOCALPOL_LOCALPOLMON_H */
