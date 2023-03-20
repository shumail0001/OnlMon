#ifndef MVTX_MVTXMON_H
#define MVTX_MVTXMON_H

#include <onlmon/OnlMon.h>
#include <map>
#include <cmath>
#include <TString.h>
#include <TPaveText.h>

//#include "/sphenix/u/jkvapil/onlmon/src/subsystems/mvtx/PixelData.h"
#include "PixelData.h"

class Event;
class OnlMonDB;
class TH1;
class TH2;
class TH1I;
class TH2I;
class TH1D;
class TH2D;
class TH2Poly;
class map;
class pair;
//class ChipPixelData;



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
  //TH1 *mvtxhist1 = nullptr;
  //TH2 *mvtxhist2 = nullptr;

  const static int NSTAVE = 48;
  const static int NCHIP = 9;
  const static int NLAYERS = 3;
  const static int IDMVTXV1_MAXRUID = 48;
  const static int IDMVTXV1_MAXRUCHN = 28;
  static constexpr int NFees = 48 * 3;
  static constexpr int NTrigger = 13;
static constexpr int NLanesMax = 27;
  const int NRowMax = 512;
  const int NColMax = 1024;
  int mHitPerChip[NLAYERS][NSTAVE][NCHIP] = {};
int mHitPerChip_evt[NLAYERS][NSTAVE][NCHIP] = {};
  float OccPerChip[NSTAVE][NCHIP] = {};
  const int NBins = 30;
  static constexpr int NFlags = 3;

  int mMaxGeneralAxisRange = -3;  // the range of TH2Poly plots z axis range, pow(10, mMinGeneralAxisRange) ~ pow(10, mMaxGeneralAxisRange)
  int mMinGeneralAxisRange = -12;
  std::string mLaneStatusFlag[NFlags] = { "WARNING", "ERROR", "FAULT" };

 const int StaveBoundary[4] = { 0, 12, 28, 48};
const int LayerBoundaryFEE[NLAYERS - 1] = { 35, 83};
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
 //fee
  TH2I* mTriggerVsFeeId;
  TH1I* mTrigger;
  TH2I* mLaneInfo;
  TH2I* mLaneStatus[NFlags];
  TH2I* mLaneStatusCumulative[NFlags];
  TH1I* mLaneStatusSummary[NLAYERS];
  TH1I* mLaneStatusSummaryIB;
 TString mTriggerType[NTrigger] = { "ORBIT", "HB", "HBr", "HC", "PHYSICS", "PP", "CAL", "SOT", "EOT", "SOC", "EOC", "TF", "INT" };

//raw task
  TH1D* hErrorPlots;
  TH2D* hErrorFile;
  //TH1D* hInfoCanvas;

  TH1D* hOccupancyPlot[NLAYERS];
  TH2I* hEtaPhiHitmap[NLAYERS];
  TH2D* hChipStaveOccupancy[NLAYERS];
  TH2I* hChipHitmap[3][20][9];
  TH2I* hChipHitmap_evt[3][20][9];

  //fhr
  TH2I* mErrorVsFeeid;
  TH2Poly* mGeneralOccupancy;
  TH2Poly* mGeneralNoisyPixel;
  TH2D* mDeadChipPos[NLAYERS];
  TH2D* mAliveChipPos[NLAYERS];
  TH2D* mTotalDeadChipPos;
  TH2D* mTotalAliveChipPos;
  TH2D* mChipStaveOccupancy[NLAYERS];
  TH1D* mOccupancyPlot[NLAYERS];

 static constexpr int NCols = 1024;
  static constexpr int NRows = 512;
  static constexpr int NPixels = NRows * NCols;
float mOccupancyCutForNoisyPixel = 0.1;
int mNoisyPixelNumber[3][20][9] = { { 0 } };

static constexpr int nbinsetaIB = 9;
  static constexpr int nbinsphiIB = 16;
 int mEtabins = 130;
  int mPhibins = 240;
  float etabinsIB[3][nbinsetaIB + 1] = { { -2.34442, -2.06375, -1.67756, -1.0732, 0., 1.0732, 1.67756, 2.06375, 2.34442, 2.6 }, { -2.05399, -1.77866, -1.40621, -0.851972, 0., 0.851972, 1.40621, 1.77866, 2.05399, 2.4 }, { -1.8412, -1.57209, -1.21477, -0.707629, 0., 0.707629, 1.21477, 1.57209, 1.8412, 2.1 } };
  float phibinsIB[3][nbinsphiIB + 1] = { { -2.86662, -2.34302, -1.81942, -1.29582, -0.772222, -0.248623, 0.203977, 0.274976, 0.596676, 0.798574, 0.989375, 1.32217, 1.38207, 1.84577, 2.36937, 2.89297, 3.2 }, { -2.93762, -2.54492, -2.15222, -1.75952, -1.36682, -0.974121, -0.581422, -0.188722, 0.159761, 0.47392, 0.78808, 1.10224, 1.77477, 2.16747, 2.56017, 2.95287, 3.2 }, { -2.98183, -2.66767, -2.35351, -2.03935, -1.72519, -1.41104, -1.09688, -0.782717, -0.468558, -0.154398, 1.4164, 1.73056, 2.04472, 2.35888, 2.67304, 2.98719, 3.2 } };


static constexpr int NError = 11;
static constexpr int NErrorExtended = 19;
TPaveText* pt[NError];
  TString ErrorType[NError] = { "Error ID 1: ErrPageCounterDiscontinuity", "Error ID 2: ErrRDHvsGBTHPageCnt",
                                "Error ID 3: ErrMissingGBTHeader", "Error ID 4: ErrMissingGBTTrailer", "Error ID 5: ErrNonZeroPageAfterStop",
                                "Error ID 6: ErrUnstoppedLanes", "Error ID 7: ErrDataForStoppedLane", "Error ID 8: ErrNoDataForActiveLane",
                                "Error ID 9: ErrIBChipLaneMismatch", "Error ID 10: ErrCableDataHeadWrong",
                                "Error ID 11: Jump in RDH_packetCounter" };

 const float etaCoverage[NLAYERS] = { 2.5, 2.3, 2.0};
  const double PhiMin = 0;
  const double PhiMax = 3.284;
  const int NFiles = 6;
int ntriggers = 0;


const int NStaves[3] = { 12, 16, 20 };
const float StartAngle[3] = { 16.997 / 360 * (M_PI * 2.), 17.504 / 360 * (M_PI * 2.), 17.337 / 360 * (M_PI * 2.)}; // start angle of first stave in each layer
const float MidPointRad[3] = { 23.49, 31.586, 39.341}; 
  //TH1	*InfoCanvas= nullptr;

const int mapstave[3][20] = {{10,11,12,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0},{25,26,27,28,13,14,15,16,17,18,19,20,21,22,23,24,0,0,0,0},{44,45,46,47,48,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43}};

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
void drawLayerName(TH2* histo2D);
void createPoly(TH2Poly *h);


};

/*
class PixelData
{

 public:
  PixelData(uint16_t r = 0, uint16_t c = 0) : mRow(r), mCol(c) {}
  uint16_t getRow() const { return mRow & RowMask; }
  uint16_t getCol() const { return mCol; }
  bool isMasked() const { return mRow & MaskBit; }
  void setMask() { mRow |= MaskBit; }
  void unsetMask() { mRow &= RowMask; }

  ///< for faster access when the pixel is guaranteed to not be masked
  uint16_t getRowDirect() const { return mRow; }

  bool operator==(const PixelData& dt) const
  {
    ///< check if one pixel is equal to another
    return (getCol() == dt.getCol()) && (getRow() == dt.getRow());
  }

  bool operator>(const PixelData& dt) const
  {
    ///< check if one pixel is greater than another (first column then row)
    if (getCol() == dt.getCol()) {
      return getRow() > dt.getRow();
    }
    return getCol() > dt.getCol();
  }

  bool operator<(const PixelData& dt) const
  {
    ///< check if one pixel is lesser than another (first column then row)
    if (getCol() == dt.getCol()) {
      return getRow() < dt.getRow();
    }
    return getCol() < dt.getCol();
  }

  bool isNeighbour(const PixelData& dt, int maxDist) const
  {
    ///< check if one pixel is in proximity of another
    return (std::abs(static_cast<int>(getCol()) - static_cast<int>(dt.getCol())) <= maxDist &&
            std::abs(static_cast<int>(getRow()) - static_cast<int>(dt.getRow())) <= maxDist);
  }

  int compare(const PixelData& dt) const
  {
    ///< compare to pixels (first column then row)
    return operator==(dt) ? 0 : (operator>(dt) ? 1 : -1);
  }

  static constexpr uint32_t DummyROF = 0xffffffff;
  static constexpr uint32_t DummyChipID = 0xffff;

 private:
  void sanityCheck() const;
  static constexpr int RowMask = 0x7FFF; ///< 32768 rows are supported
  static constexpr int MaskBit = 0x8000; ///< 16-th bit is used to flag masked pixel
  uint16_t mRow = 0;                    ///< pixel row
  uint16_t mCol = 0;                    ///< pixel column

  // ClassDefNV(PixelData, 1);
};

namespace mvtx{

namespace lhcConstants
{
constexpr int LHCMaxBunches = 3564;                              // max N bunches
constexpr double LHCRFFreq = 400.789e6;                          // LHC RF frequency in Hz
constexpr double LHCBunchSpacingNS = 10 * 1.e9 / LHCRFFreq;      // bunch spacing in ns (10 RFbuckets)
constexpr double LHCOrbitNS = LHCMaxBunches * LHCBunchSpacingNS; // orbit duration in ns
constexpr double LHCRevFreq = 1.e9 / LHCOrbitNS;                 // revolution frequency
constexpr double LHCBunchSpacingMUS = LHCBunchSpacingNS * 1e-3;  // bunch spacing in \mus (10 RFbuckets)
constexpr double LHCOrbitMUS = LHCOrbitNS * 1e-3;                // orbit duration in \mus
} // namespace lhcConstants

//!< TODO: Add RHIC constants

struct InteractionRecord {
  // information about bunch crossing and orbit
  static constexpr uint16_t DummyBC = 0xffff;
  static constexpr uint64_t DummyOrbit = 0xffffffffff;
  static constexpr double DummyTime = DummyBC * mvtx::lhcConstants::LHCBunchSpacingNS + DummyOrbit * mvtx::lhcConstants::LHCOrbitNS;

  uint16_t bc = DummyBC;       ///< bunch crossing ID of interaction
  uint64_t orbit = DummyOrbit; ///< LHC orbit

  InteractionRecord() = default;

  InteractionRecord(double tNS)
  {
    setFromNS(tNS);
  }

  InteractionRecord(uint16_t b, uint64_t orb) : bc(b), orbit(orb){};

  InteractionRecord(const InteractionRecord& src) = default;

  InteractionRecord& operator=(const InteractionRecord& src) = default;

  void clear()
  {
    bc = DummyBC;
    orbit = DummyOrbit;
  }

  bool isDummy() const
  {
    return bc > mvtx::lhcConstants::LHCMaxBunches;
  }

  void setFromNS(double ns)
  {
    bc = ns2bc(ns, orbit);
  }

  static double bc2ns(int bc, uint64_t orbit)
  {
    return bc * mvtx::lhcConstants::LHCBunchSpacingNS + orbit * mvtx::lhcConstants::LHCOrbitNS;
  }

  static int ns2bc(double ns, uint64_t& orb)
  {
    orb = ns > 0 ? ns / mvtx::lhcConstants::LHCOrbitNS : 0;
    ns -= orb * mvtx::lhcConstants::LHCOrbitNS;
    return std::round(ns / mvtx::lhcConstants::LHCBunchSpacingNS);
  }

  double bc2ns() const
  {
    return bc2ns(bc, orbit);
  }

  bool operator==(const InteractionRecord& other) const
  {
    return (bc == other.bc) && (orbit == other.orbit);
  }

  bool operator!=(const InteractionRecord& other) const
  {
    return (bc != other.bc) || (orbit != other.orbit);
  }

  int64_t differenceInBC(const InteractionRecord& other) const
  {
    // return difference in bunch-crossings
    int64_t diffBC = int(bc) - other.bc;
    if (orbit != other.orbit) {
      diffBC += (int64_t(orbit) - other.orbit) * mvtx::lhcConstants::LHCMaxBunches;
    }
    return diffBC;
  }

  float differenceInBCNS(const InteractionRecord& other) const
  {
    // return difference in bunch-crossings in ns
    return differenceInBC(other) * mvtx::lhcConstants::LHCBunchSpacingNS;
  }

  float differenceInBCMS(const InteractionRecord& other) const
  {
    // return difference in bunch-crossings in ms
    return differenceInBC(other) * mvtx::lhcConstants::LHCBunchSpacingMUS;
  }

  int64_t toLong() const
  {
    // return as single long number
    return (int64_t(orbit) * mvtx::lhcConstants::LHCMaxBunches) + bc;
  }

  void setFromLong(int64_t l)
  {
    // set from long BC counter
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
  }

  static InteractionRecord long2IR(int64_t l)
  {
    // set from long BC counter
    return {uint16_t(l % mvtx::lhcConstants::LHCMaxBunches), uint32_t(l / mvtx::lhcConstants::LHCMaxBunches)};
  }

  bool operator>(const InteractionRecord& other) const
  {
    return (orbit == other.orbit) ? (bc > other.bc) : (orbit > other.orbit);
  }

  bool operator>=(const InteractionRecord& other) const
  {
    return !((*this) < other);
  }

  bool operator<(const InteractionRecord& other) const
  {
    return (orbit == other.orbit) ? (bc < other.bc) : (orbit < other.orbit);
  }

  bool operator<=(const InteractionRecord& other) const
  {
    return !((*this) > other);
  }

  InteractionRecord operator--()
  {
    // prefix decrement operator, no check for orbit wrap
    if (!bc--) {
      orbit--;
      bc = mvtx::lhcConstants::LHCMaxBunches - 1;
    }
    return InteractionRecord(*this);
  }

  InteractionRecord operator--(int)
  {
    // postfix decrement operator, no check for orbit wrap
    InteractionRecord tmp(*this);
    if (!bc--) {
      orbit--;
      bc = mvtx::lhcConstants::LHCMaxBunches - 1;
    }
    return tmp;
  }

  InteractionRecord operator++()
  {
    // prefix increment operator,no check for orbit wrap
    if ((++bc) == mvtx::lhcConstants::LHCMaxBunches) {
      orbit++;
      bc = 0;
    }
    return InteractionRecord(*this);
  }

  InteractionRecord operator++(int)
  {
    // postfix increment operator, no check for orbit wrap
    InteractionRecord tmp(*this);
    if ((++bc) == mvtx::lhcConstants::LHCMaxBunches) {
      orbit++;
      bc = 0;
    }
    return tmp;
  }

  InteractionRecord& operator+=(int64_t dbc)
  {
    // bc self-addition operator, no check for orbit wrap
    auto l = toLong() + dbc;
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
    return *this;
  }

  InteractionRecord& operator-=(int64_t dbc)
  {
    // bc self-subtraction operator, no check for orbit wrap
    return operator+=(-dbc);
  }

  InteractionRecord& operator+=(const InteractionRecord& add)
  {
    // InteractionRecord self-addition operator, no check for orbit wrap
    auto l = this->toLong() + add.toLong();
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
    return *this;
  }

  InteractionRecord& operator-=(const InteractionRecord& add)
  {
    // InteractionRecord self-subtraction operator, no check for orbit wrap
    auto l = this->toLong() - add.toLong();
    bc = l % mvtx::lhcConstants::LHCMaxBunches;
    orbit = l / mvtx::lhcConstants::LHCMaxBunches;
    return *this;
  }

  InteractionRecord operator+(int64_t dbc) const
  {
    // bc addition operator, no check for orbit wrap
    auto l = toLong() + dbc;
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  InteractionRecord operator-(int64_t dbc) const
  {
    // bc subtraction operator, no check for orbit wrap
    auto l = toLong() - dbc;
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  InteractionRecord operator+(const InteractionRecord& add) const
  {
    // InteractionRecord addition operator, no check for orbit wrap
    auto l = this->toLong() + add.toLong();
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  InteractionRecord operator-(const InteractionRecord& add) const
  {
    // InteractionRecord subtraction operator, no check for orbit wrap
    auto l = this->toLong() - add.toLong();
    return InteractionRecord(l % mvtx::lhcConstants::LHCMaxBunches, l / mvtx::lhcConstants::LHCMaxBunches);
  }

  void print() const;
  std::string asString() const;
  friend std::ostream& operator<<(std::ostream& stream, InteractionRecord const& ir);

// ClassDefNV(InteractionRecord, 3);
};


struct InteractionTimeRecord : public InteractionRecord {
  double timeInBCNS = 0.; ///< time in NANOSECONDS relative to orbit/bc

  InteractionTimeRecord() = default;

  /// create from the interaction record and time in the bunch (in ns)
  InteractionTimeRecord(const InteractionRecord& ir, double t_in_bc) : InteractionRecord(ir), timeInBCNS(t_in_bc)
  {
  }

  /// create from the abs. (since orbit=0/bc=0) time in NS
  InteractionTimeRecord(double tNS) : InteractionRecord(tNS)
  {
    timeInBCNS = tNS - bc2ns();
  }

  /// set the from the abs. (since orbit=0/bc=0) time in NS
  void setFromNS(double tNS)
  {
    InteractionRecord::setFromNS(tNS);
    timeInBCNS = tNS - bc2ns();
  }

  void clear()
  {
    InteractionRecord::clear();
    timeInBCNS = 0.;
  }

  double getTimeOffsetWrtBC() const
  {
    return timeInBCNS;
  }

  /// get time in ns from orbit=0/bc=0
  double getTimeNS() const
  {
    return timeInBCNS + bc2ns();
  }

  bool operator==(const InteractionTimeRecord& other) const
  {
    return this->InteractionRecord::operator==(other) && (timeInBCNS == other.timeInBCNS);
  }

  bool operator!=(const InteractionTimeRecord& other) const
  {
    return this->InteractionRecord::operator!=(other) || (timeInBCNS != other.timeInBCNS);
  }

  bool operator>(const InteractionTimeRecord& other) const
  {
    return (this->InteractionRecord::operator>(other)) || (this->InteractionRecord::operator==(other) && (timeInBCNS > other.timeInBCNS));
  }

  bool operator>=(const InteractionTimeRecord& other) const
  {
    return !((*this) < other);
  }

  bool operator<(const InteractionTimeRecord& other) const
  {
    return (this->InteractionRecord::operator<(other)) || (this->InteractionRecord::operator==(other) && (timeInBCNS < other.timeInBCNS));
  }

  bool operator<=(const InteractionTimeRecord& other) const
  {
    return !((*this) > other);
  }

  void print() const;
  std::string asString() const;
  friend std::ostream& operator<<(std::ostream& stream, InteractionTimeRecord const& ir);

//  ClassDefNV(InteractionTimeRecord, 1);
};
} // namespace mvtx

namespace std
{
// defining std::hash for InteractionRecord to be used with std containers
template <>
struct hash<mvtx::InteractionRecord> {
 public:
  size_t operator()(const mvtx::InteractionRecord& ir) const
  {
    return ir.toLong();
  }
};
} // namespace std

//< Transient data for single chip fired pixeld
///< Assumes that the digits data is sorted in chip/col/row
class ChipPixelData
{
 public:
  // total number of raw data bytes to save in case of error and number of bytes (if any) after problematic one
  static constexpr size_t MAXDATAERRBYTES = 16, MAXDATAERRBYTES_AFTER = 2;
  ChipPixelData() = default;
  ~ChipPixelData() = default;
  uint8_t getROFlags() const { return mROFlags; }
  uint16_t getChipID() const { return mChipID; }
  uint32_t getROFrame() const { return mROFrame; }
  uint32_t getStartID() const { return mStartID; }
  uint32_t getFirstUnmasked() const { return mFirstUnmasked; }
  uint32_t getTrigger() const { return mTrigger; }
  const mvtx::InteractionRecord& getInteractionRecord() const { return mInteractionRecord; }
  void setInteractionRecord(const mvtx::InteractionRecord& r) { mInteractionRecord = r; }
  const std::vector<PixelData>& getData() const { return mPixels; }
  std::vector<PixelData>& getData() { return (std::vector<PixelData>&)mPixels; }

  void setROFlags(uint8_t f = 0) { mROFlags = f; }
  void setChipID(uint16_t id) { mChipID = id; }
  void setROFrame(uint32_t r) { mROFrame = r; }
  void setStartID(uint32_t id) { mStartID = id; }
  void setFirstUnmasked(uint32_t n) { mFirstUnmasked = n; }
  void setTrigger(uint32_t t) { mTrigger = t; }

  void setError(ChipStat::DecErrors i) { mErrors |= 0x1 << i; }
  void addErrorInfo(uint64_t b) { mErrorInfo |= b; }
  void setErrorFlags(uint32_t f) { mErrors |= f; }
  bool isErrorSet(ChipStat::DecErrors i) const { return mErrors & (0x1 << i); }
  bool isErrorSet() const { return mErrors != 0; }
  auto getErrorFlags() const { return mErrors; }
  auto getErrorInfo() const { return mErrorInfo; }
  auto getNBytesInRawBuff() const { return int(mErrorInfo >> 32) & 0xff; }
  void setNBytesInRawBuff(int n) { mErrorInfo |= (uint64_t(n & 0xff)) << 32; }
  auto& getRawErrBuff() { return mRawBuff; }
  auto& getRawErrBuff() const { return mRawBuff; }
  std::string getErrorDetails(int pos) const;

  void resetChipID()
  {
    mChipID = -1;
  }

  void clear()
  {
    resetChipID();
    mPixels.clear();
    mROFlags = 0;
    mFirstUnmasked = 0;
    mErrors = 0;
    mErrorInfo = 0;
    mPixelsOrder.clear();
  }

  void swap(ChipPixelData& other)
  {
    // swap content of two objects
    std::swap(mROFlags, other.mROFlags);
    std::swap(mChipID, other.mChipID);
    std::swap(mROFrame, other.mROFrame);
    std::swap(mFirstUnmasked, other.mFirstUnmasked); // strictly speaking, not needed
    std::swap(mStartID, other.mStartID);             // strictly speaking, not needed
    std::swap(mTrigger, other.mTrigger);
    std::swap(mErrors, other.mErrors);
    std::swap(mInteractionRecord, other.mInteractionRecord);
    mPixels.swap(other.mPixels);
  }

  void maskFiredInSample(const ChipPixelData& sample)
  {
    ///< mask in the current data pixels fired in provided sample
    const auto& pixelsS = sample.getData();
    uint32_t nC = mPixels.size();
    if (!nC) {
      return;
    }
    uint32_t nS = pixelsS.size();
    if (!nS) {
      return;
    }
    uint32_t itC = 0, itS = 0;
    while (itC < nC && itS < nS) {
      auto& pix0 = mPixels[itC];
      const auto& pixC = pixelsS[itS];
      if (pix0 == pixC) { // same
        pix0.setMask();
        if (mFirstUnmasked == itC++) { // mFirstUnmasked should flag 1st unmasked pixel entry
          mFirstUnmasked = itC;
        }
        itS++;
      } else if (pix0 < pixC) {
        itC++;
      } else {
        itS++;
      }
    }
  }

  void maskFiredInSample(const ChipPixelData& sample, int maxDist)
  {
    ///< mask in the current data pixels (or their neighbours) fired in provided sample
    const auto& pixelsS = sample.getData();
    int nC = mPixels.size();
    if (!nC) {
      return;
    }
    int nS = pixelsS.size();
    if (!nS) {
      return;
    }
    for (int itC = 0, itS = 0; itC < nC; itC++) {
      auto& pix0 = mPixels[itC];

      // seek to itS which is inferior than itC - maxDist
      auto mincol = pix0.getCol() > maxDist ? pix0.getCol() - maxDist : 0;
      auto minrow = pix0.getRowDirect() > maxDist ? pix0.getRowDirect() - maxDist : 0;
      if (itS == nS) { // in case itS lool below reached the end
        itS--;
      }
      while ((pixelsS[itS].getCol() > mincol || pixelsS[itS].getRow() > minrow) && itS > 0) {
        itS--;
      }
      for (; itS < nS; itS++) {
        const auto& pixC = pixelsS[itS];

        auto drow = static_cast<int>(pixC.getRow()) - static_cast<int>(pix0.getRowDirect());
        auto dcol = static_cast<int>(pixC.getCol()) - static_cast<int>(pix0.getCol());

        if (dcol > maxDist || (dcol == maxDist && drow > maxDist)) {
          break; // all higher itS will not match to this itC also
        }
        if (dcol < -maxDist || (drow > maxDist || drow < -maxDist)) {
          continue;
        } else {
          pix0.setMask();
          if (int(mFirstUnmasked) == itC) { // mFirstUnmasked should flag 1st unmasked pixel entry
            mFirstUnmasked = itC + 1;
          }
          break;
        }
      }
    }
  }

  void print() const;
  std::vector<int>& getPixelsOrder() { return mPixelsOrder; }

 private:
  uint8_t mROFlags = 0;                            // readout flags from the chip trailer
  uint16_t mChipID = 0;                            // chip id within the detector
  uint32_t mROFrame = 0;                           // readout frame ID
  uint32_t mFirstUnmasked = 0;                     // first unmasked entry in the mPixels
  uint32_t mStartID = 0;                           // entry of the 1st pixel data in the whole detector data, for MCtruth access
  uint32_t mTrigger = 0;                           // trigger pattern
  uint32_t mErrors = 0;                            // errors set during decoding
  uint64_t mErrorInfo = 0;                         // optional extra info on the error
  std::array<uint8_t, MAXDATAERRBYTES> mRawBuff{}; // buffer for raw data showing an error
  mvtx::InteractionRecord mInteractionRecord = {}; // interaction record
  std::vector<PixelData> mPixels;                  // vector of pixels
  std::vector<int> mPixelsOrder;                   // vector to get ordered access to pixel ids

  // ClassDefNV(ChipPixelData, 1);
};
struct ChipStat {
  enum ActionOnError : int {
    ErrActNone = 0x0,      // do nothing
    ErrActPropagate = 0x1, // propagate to decoded data
    ErrActDump = 0x2       // produce raw data dump
  };

  enum DecErrors : int {
    BusyViolation,                // Busy violation
    DataOverrun,                  // Data overrun
    Fatal,                        // Fatal (?)
    BusyOn,                       // Busy On
    BusyOff,                      // Busy Off
    TruncatedChipEmpty,           // Data was truncated after ChipEmpty
    TruncatedChipHeader,          // Data was truncated after ChipHeader
    TruncatedRegion,              // Data was truncated after Region record
    TruncatedLondData,            // Data was truncated in the LongData record
    WrongDataLongPattern,         // LongData pattern has highest bit set
    NoDataFound,                  // Region is not followed by Short or Long data
    UnknownWord,                  // Unknown word was seen
    RepeatingPixel,               // Same pixel fired more than once
    WrongRow,                     // Non-existing row decoded
    APE_STRIP,                    // lane data stripped for this chip event (behaviour changed with RU FW v1.16.0, for general APE behaviour see  https://alice.its.cern.ch/jira/browse/O2-1717)
    APE_RESERVED_F3,              // reserved F3
    APE_DET_TIMEOUT,              // detector timeout (FATAL)
    APE_OOT_START,                // 8b10b OOT (FATAL, start)
    APE_PROTOCOL_ERROR,           // event protocol error marker (FATAL, start)
    APE_LANE_FIFO_OVERFLOW_ERROR, // lane FIFO overflow error (FATAL)
    APE_FSM_ERROR,                // FSM error (FATAL, SEU error, reached an unknown state)
    APE_OCCUPANCY_RATE_LIMIT,     // pending detector events limit (FATAL)
    APE_OCCUPANCY_RATE_LIMIT_2,   // pending detector events limit in packager(FATAL)
    APE_LANE_PROTOCOL_ERROR,      // lane protocol error
    APE_RESERVED_FC,              // reserved FC
    APE_ERROR_NON_CRITICAL_BYTE,  // Error in non critical byte
    APE_OOT_NON_CRITICAL,         // OOT non-critical
    WrongDColOrder,               // DColumns non increasing
    InterleavedChipData,          // Chip data interleaved on the cable
    TruncatedBuffer,              // truncated buffer, 0 padding
    TrailerAfterHeader,           // trailer seen after header w/o FE of FD set
    NErrorsDefined
  };

  static constexpr std::array<std::string_view, NErrorsDefined> ErrNames = {
    "BusyViolation flag ON",                        // BusyViolation
    "DataOverrun flag ON",                          // DataOverrun
    "Fatal flag ON",                                // Fatal
    "BusyON",                                       // BusyOn
    "BusyOFF",                                      // BusyOff
    "Data truncated after ChipEmpty",               // TruncatedChipEmpty
    "Data truncated after ChipHeader",              // TruncatedChipHeader
    "Data truncated after Region",                  // TruncatedRegion
    "Data truncated after LongData",                // TruncatedLondData
    "LongData pattern has highest bit set",         // WrongDataLongPattern
    "Region is not followed by Short or Long data", // NoDataFound
    "Unknown word",                                 // UnknownWord
    "Same pixel fired multiple times",              // RepeatingPixel
    "Non-existing row decoded",                     // WrongRow
    "APE_STRIP",                                    // lane data stripped for this chip event (behaviour changed with RU FW v1.16.0, for general APE behaviour see  https://alice.its.cern.ch/jira/browse/O2-1717)
    "APE_RESERVED_F3",                              // reserved F3
    "APE_DET_TIMEOUT",                              // detector timeout (FATAL)
    "APE_OOT_START",                                // 8b10b OOT (FATAL, start)
    "APE_PROTOCOL_ERROR",                           // event event protocol error marker (FATAL, start)
    "APE_LANE_FIFO_OVERFLOW_ERROR",                 // lane FIFO overflow error (FATAL)
    "APE_FSM_ERROR",                                // FSM error (FATAL, SEU error, reached an unknown state)
    "APE_OCCUPANCY_RATE_LIMIT",                     // pending detector events limit (FATAL)
    "APE_OCCUPANCY_RATE_LIMIT_2",                   // pending detector events limit in packager(FATAL)
    "APE_LANE_PROTOCOL_ERROR",                      // lane protocol error
    "APE_RESERVED_FC",                              // reserved
    "APE_ERROR_IN_NON_CRITICAL_BYTE",               // Error in non critical byte
    "APE_OOT_NON_CRITICAL",                         // OOT non-critical
    "DColumns non-increasing",                      // DColumns non increasing
    "Chip data interleaved on the cable",           // Chip data interleaved on the cable
    "TruncatedBuffer",                              // truncated buffer, 0 padding
    "TrailerAfterHeader"                            // trailer seen after header w/o FE of FD set
  };

  static constexpr std::array<uint32_t, NErrorsDefined> ErrActions = {
    ErrActPropagate | ErrActDump, // Busy violation
    ErrActPropagate | ErrActDump, // Data overrun
    ErrActPropagate | ErrActDump, // Fatal (?)
    ErrActNone,                   // Busy On
    ErrActNone,                   // Busy Off
    ErrActPropagate | ErrActDump, // Data was truncated after ChipEmpty
    ErrActPropagate | ErrActDump, // Data was truncated after ChipHeader
    ErrActPropagate | ErrActDump, // Data was truncated after Region record
    ErrActPropagate | ErrActDump, // Data was truncated in the LongData record
    ErrActPropagate | ErrActDump, // LongData pattern has highest bit set
    ErrActPropagate | ErrActDump, // Region is not followed by Short or Long data
    ErrActPropagate | ErrActDump, // Unknown word was seen
    ErrActPropagate,              // Same pixel fired more than once
    ErrActPropagate | ErrActDump, // Non-existing row decoded
    ErrActPropagate | ErrActDump, // lane data stripped for this chip event (behaviour changed with RU FW v1.16.0, for general APE behaviour see  https://alice.its.cern.ch/jira/browse/O2-1717)
    ErrActPropagate | ErrActDump, // reserved F3
    ErrActPropagate | ErrActDump, // detector timeout (FATAL)
    ErrActPropagate | ErrActDump, // 8b10b OOT (FATAL, start)
    ErrActPropagate | ErrActDump, // event protocol error marker (FATAL, start)
    ErrActPropagate | ErrActDump, // lane FIFO overflow error (FATAL)
    ErrActPropagate | ErrActDump, // FSM error (FATAL, SEU error, reached an unknown state)
    ErrActPropagate | ErrActDump, // pending detector events limit (FATAL)
    ErrActPropagate | ErrActDump, // pending detector events limit in packager(FATAL)
    ErrActPropagate | ErrActDump, // lane protocol error
    ErrActPropagate | ErrActDump, // reserved FC
    ErrActPropagate | ErrActDump, // Error in non critical byte
    ErrActPropagate | ErrActDump, // OOT non-critical
    ErrActPropagate | ErrActDump, // DColumns non increasing
    ErrActPropagate | ErrActDump, // Chip data interleaved on the cable
    ErrActPropagate | ErrActDump, // Truncated buffer while something was expected
    ErrActPropagate | ErrActDump  // trailer seen after header w/o FE of FD set
  };
  uint16_t feeID = -1;
  size_t nHits = 0;
  std::array<uint32_t, NErrorsDefined> errorCounts = {};
  ChipStat() = default;
  ChipStat(uint16_t _feeID) : feeID(_feeID) {}

  void clear()
  {
    memset(errorCounts.data(), 0, sizeof(uint32_t) * errorCounts.size());
    nHits = 0;
  }

  static int getAPENonCritical(uint8_t c)
  {
    if (c == 0xfd || c == 0xfe) {
      return APE_STRIP + c - 0xf2;
    }
    return -1;
  }

  // return APE DecErrors code or -1 if not APE error, set fatal flag if needd
  static int getAPECode(uint8_t c, bool& ft)
  {
    if (c < 0xf2 || c > 0xfe) {
      ft = false;
      return -1;
    }
    ft = c >= 0xf2 && c <= 0xfe;
    return APE_STRIP + c - 0xf2;
  }
  uint32_t getNErrors() const;
  uint32_t addErrors(uint32_t mask, uint16_t chID, int verbosity);
  uint32_t addErrors(const ChipPixelData& d, int verbosity);
  void print(bool skipNoErr = true, const std::string& pref = "FEEID") const;

//  ClassDefNV(ChipStat, 1);
};

struct ChipError {
  uint32_t id = -1;
  uint32_t nerrors = 0;
  uint32_t errors = 0;

  int16_t getChipID() const { return int16_t(id & 0xffff); }
  uint16_t getFEEID() const { return uint16_t(id >> 16); }
  static uint32_t composeID(uint16_t feeID, int16_t chipID) { return uint32_t(feeID) << 16 | uint16_t(chipID); }
//  ClassDefNV(ChipError, 1);
};

/// Statistics for per-link decoding
struct GBTLinkDecodingStat {
  /// counters for format checks

  enum DecErrors : int {
    ErrNoRDHAtStart,             // page does not start with RDH
    ErrPageNotStopped,           // RDH is stopped, but the time is not matching the ~stop packet
    ErrStopPageNotEmpty,         // Page with RDH.stop is not empty
    ErrPageCounterDiscontinuity, // RDH page counters for the same RU/trigger are not continuous
    ErrRDHvsGBTHPageCnt,         // RDH and GBT header page counters are not consistent
    ErrMissingGBTTrigger,        // GBT trigger word was expected but not found
    ErrMissingGBTHeader,         // GBT payload header was expected but not found
    ErrMissingGBTTrailer,        // GBT payload trailer was expected but not found
    ErrNonZeroPageAfterStop,     // all lanes were stopped but the page counter in not 0
    ErrUnstoppedLanes,           // end of FEE data reached while not all lanes received stop
    ErrDataForStoppedLane,       // data was received for stopped lane
    ErrNoDataForActiveLane,      // no data was seen for lane (which was not in timeout)
    ErrIBChipLaneMismatch,       // chipID (on module) was different from the lane ID on the IB stave
    ErrCableDataHeadWrong,       // cable data does not start with chip header or empty chip
    ErrInvalidActiveLanes,       // active lanes pattern conflicts with expected for given RU type
    ErrPacketCounterJump,        // jump in RDH.packetCounter
    ErrPacketDoneMissing,        // packet done is missing in the trailer while CRU page is not over
    ErrMissingDiagnosticWord,    // missing diagnostic word after RDH with stop
    ErrGBTWordNotRecognized,     // GBT word not recognized
    ErrWrongeCableID,            // Invalid cable ID
    NErrorsDefined
  };
  static constexpr std::array<std::string_view, NErrorsDefined> ErrNames = {
    "Page data not start with expected RDH",                             // ErrNoRDHAtStart
    "RDH is stopped, but the time is not matching the stop packet",      // ErrPageNotStopped
    "Page with RDH.stop does not contain diagnostic word only",          // ErrStopPageNotEmpty
    "RDH page counters for the same RU/trigger are not continuous",      // ErrPageCounterDiscontinuity
    "RDH and GBT header page counters are not consistent",               // ErrRDHvsGBTHPageCnt
    "GBT trigger word was expected but not found",                       // ErrMissingGBTTrigger
    "GBT payload header was expected but not found",                     // ErrMissingGBTHeader
    "GBT payload trailer was expected but not found",                    // ErrMissingGBTTrailer
    "All lanes were stopped but the page counter in not 0",              // ErrNonZeroPageAfterStop
    "End of FEE data reached while not all lanes received stop",         // ErrUnstoppedLanes
    "Data was received for stopped lane",                                // ErrDataForStoppedLane
    "No data was seen for lane (which was not in timeout)",              // ErrNoDataForActiveLane
    "ChipID (on module) was different from the lane ID on the IB stave", // ErrIBChipLaneMismatch
    "Cable data does not start with chip header or empty chip",          // ErrCableDataHeadWrong
    "Active lanes pattern conflicts with expected for given RU type",    // ErrInvalidActiveLanes
    "Jump in RDH_packetCounter",                                         // ErrPacketCounterJump
    "Packet done is missing in the trailer while CRU page is not over",  // ErrPacketDoneMissing
    "Missing diagnostic GBT word after RDH with stop",                   // ErrMissingDiagnosticWord
    "GBT word not recognized",                                           // ErrGBTWordNotRecognized
    "Wrong cable ID"                                                     // ErrWrongeCableID
  };

  uint16_t feeID = 0; // FeeID
  // Note: packet here is meant as a group of CRU pages belonging to the same trigger
  uint32_t nPackets = 0;                                                        // total number of packets (RDH pages)
  uint32_t nTriggers = 0;                                                       // total number of triggers (ROFs)
  std::array<uint32_t, NErrorsDefined> errorCounts = {};                        // error counters
  std::array<uint32_t, GBTDataTrailer::MaxStateCombinations> packetStates = {}; // packet status from the trailer

  void clear()
  {
    nPackets = 0;
    nTriggers = 0;
    errorCounts.fill(0);
    packetStates.fill(0);
  }

  void print(bool skipNoErr = true) const;

//  ClassDefNV(GBTLinkDecodingStat, 2);
};
}
*/

#endif /* MVTX_MVTXMON_H */
