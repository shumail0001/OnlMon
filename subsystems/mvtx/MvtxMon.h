#ifndef MVTX_MVTXMON_H
#define MVTX_MVTXMON_H

#include <onlmon/OnlMon.h>
#include <map>
#include <cmath>

class Event;
class OnlMonDB;
class TH1;
class TH2;
class TH2Poly;
class map;
class pair;


class MvtxMon : public OnlMon
{
 public:
  MvtxMon(const std::string &name);
  virtual ~MvtxMon();

  int process_event(Event *evt);
  int Init();
  int BeginRun(const int runno);
  int Reset();
  

 protected:
  int DBVarInit();
  int evtcnt = 0;
  int idummy = 0;
  OnlMonDB *dbvars = nullptr;
  TH1 *mvtxhist1 = nullptr;
  TH2 *mvtxhist2 = nullptr;

  const static int NSTAVE = 4;
  const static int NCHIP = 9;
  const static int IDMVTXV1_MAXRUID = 4;
  const static int IDMVTXV1_MAXRUCHN = 28;
  const int NRowMax = 512;
  const int NColMax = 1024;
  int HitPerChip[NSTAVE][NCHIP] = {};
  float OccPerChip[NSTAVE][NCHIP] = {};
  const int NBins = 30;
static constexpr int NFlags = 3;

  int mMaxGeneralAxisRange = -3;  // the range of TH2Poly plots z axis range, pow(10, mMinGeneralAxisRange) ~ pow(10, mMaxGeneralAxisRange)
  int mMinGeneralAxisRange = -12;
  std::string mLaneStatusFlag[NFlags] = { "WARNING", "ERROR", "FAULT" };

 const int StaveBoundary[4] = { 0, 12, 28, 48};
double** mOccupancyLane;





  //TH2 * HitMap[NSTAVE][NCHIP] =nullptr;
  TH2	*mvtxmon_ChipStaveOcc= nullptr;
  TH1	*mvtxmon_ChipStave1D= nullptr;
  TH1	*mvtxmon_ChipFiredHis= nullptr;
  TH1	*mvtxmon_EvtHitChip= nullptr;
  TH1 *mvtxmon_EvtHitDis= nullptr;
  TH2 *mvtxmon_HitMap[NSTAVE][NCHIP] = {nullptr}; 
  TH2Poly* mvtxmon_GeneralOccupancy = nullptr;
  TH2Poly* mvtxmon_LaneStatusOverview[NFlags] = {nullptr}; 

const int NStaves[3] = { 12, 16, 20 };
const float StartAngle[3] = { 16.997 / 360 * (M_PI * 2.), 17.504 / 360 * (M_PI * 2.), 17.337 / 360 * (M_PI * 2.)}; // start angle of first stave in each layer
const float MidPointRad[3] = { 23.49, 31.586, 39.341}; 
  //TH1	*InfoCanvas= nullptr;

  std::map<std::pair<int,int>,std::pair<int,int>> chipmap = {
	{{1,1}, {0,0}},
	{{1,2}, {0,1}},
	{{1,3}, {0,2}},
	{{1,4}, {0,3}},
	{{1,5}, {0,4}},
	{{1,6}, {0,5}},
	{{1,7}, {0,6}},
	{{1,8}, {0,7}},
	{{1,9}, {3,8}},
	{{1,10}, {3,7}},
	{{1,11}, {3,6}},
	{{1,12}, {3,5}},
	{{1,13}, {3,4}},
	{{1,14}, {3,3}},
	{{1,15}, {3,2}},
	{{1,16}, {3,1}},
	{{1,17}, {3,0}},
	{{1,18}, {2,8}},
	{{1,19}, {2,7}},
	{{1,20}, {2,6}},
	{{1,21}, {2,5}},
	{{1,22}, {2,4}},
	{{1,23}, {2,3}},
	{{1,24}, {2,2}},
	{{1,25}, {2,1}},
	{{1,26}, {2,0}},
	{{1,27}, {0,8}},
	{{2,1}, {1,2}},
	{{2,2}, {1,1}},
	{{2,3}, {1,0}},
	{{2,4}, {1,3}},
	{{2,5}, {1,4}},
	{{2,6}, {1,5}},
	{{2,7}, {1,6}},
	{{2,8}, {1,7}},
	{{2,9}, {1,8}}
}; //<ruid, ruchn> to <stave, chipID>

private:
unsigned short decode_row(int hit){	return hit >> 16;}

unsigned short decode_col(int hit){	return hit & 0xffff;}

void getStavePoint(int layer, int stave, double* px, double* py);


};

#endif /* MVTX_MVTXMON_H */
