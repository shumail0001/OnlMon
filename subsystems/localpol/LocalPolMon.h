#ifndef LOCALPOL_LOCALPOLMON_H
#define LOCALPOL_LOCALPOLMON_H

#include <onlmon/OnlMon.h>
#include <cmath>
#include <map>
#include <vector>

class CaloWaveformFitting;
class Event;
class TH1D;
class TH2;
class TProfile;
class Packet;
class TRandom;
class eventReceiverClient;

class LocalPolMon : public OnlMon
{
 public:
  explicit LocalPolMon(const std::string &name = "LOCALPOLMON");
  virtual ~LocalPolMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();

 protected:
  double *ComputeAsymmetries(double L_U, double R_D, double L_D, double R_U);
  float anaWaveformFast(Packet *p, const int channel);
  CaloWaveformFitting *WaveformProcessingFast = nullptr;

  const int packetid_gl1 = 14001;  // could be ported to config
  const int packetid_smd = 12001;  // could be ported to config

  const int UP = 1;
  const int DN = -1;

  const int BLUE = 0;
  const int YELLOW = 1;

  const int ZDCN2 = 10;
  const int ZDCS2 = 2;

  const float nchannelsY = 8.0;
  const float nchannelsX = 7.0;
  const float PI = 3.14159;
  const float pitchY = 2.0 /*cm plastic scint.*/ * (11.0 / 10.5) /*(pitch correction for gap and wrapping)*/ * sin(PI / 4) /*(correct for the tilt)*/;
  const float pitchX = 1.5 /*cm plastic scint.*/ * (11.0 / 10.5) /*(pitch correction for gap and wrapping)*/;

  bool goodtrigger[16] = {false};        // selection from config
  bool fake = false;                     // config
  bool verbosity = false;                // config
  int ExpectedsPhenixGapPosition = 117;  // from config
  int EventCountThresholdGap = 6000;     // from config
  int EventsAsymmetryNewPoint = 10000;   // from config
  int evtcnt = 0;
  int StartAbortGapPattern = 111;
  int StartAbortGapData = 117;
  int CrossingShift = -6;
  int iPoint = 0;
  int ConversionSign[14] = {1, -1, 1, 1};  // NorthY (bottom to top), NorthX (left to righ from IR->ZDC, while x points outside the ring), SouthY (bottom to top), SouthX (left to right from IR->ZDC and x points outside ring)
  float smd_north_rgain[16] = {0.};
  float smd_south_rgain[16] = {0.};
  float signalZDCN2 = 0;
  float signalZDCS2 = 0;

  float ZeroPosition[4] = {0., 0., 0., 0.};  // from config

  std::map<int, int> SpinPatterns[2];
  std::map<int, long long> gl1_counter[16];
  std::map<int, int> Chmapping;

  TH1D **h_Counts = nullptr;
  TH1D **h_CountsScramble = nullptr;

  TProfile *h_time = nullptr;
  TH1D ****h_Asym = nullptr;
  TH1D ****h_AsymScramble = nullptr;

  TRandom *myRandomBunch = nullptr;
  std::map<int, int> stored_gl1p_files;
  eventReceiverClient *erc = {nullptr};
};

#endif /* LOCALPOL_LOCALPOLMON_H */
