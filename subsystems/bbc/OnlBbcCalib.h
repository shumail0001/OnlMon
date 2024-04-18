#ifndef ONLBBCCALIB_H
#define ONLBBCCALIB_H

#include <array>
#include <vector>
#include <string>
#include <memory>

#include <BbcMonDefs.h>
#include <mbd/MbdGeom.h>

class TTree;

class OnlBbcCalib
{
 public:
  OnlBbcCalib();

  // OnlBbcCalib(OnlBbcCalib &other) = delete;
  // void operator=(const OnlBbcCalib &) = delete;

  virtual ~OnlBbcCalib() {}

  float get_qgain(const int ipmt) const { return _qfit_mpv[ipmt]; }
  float get_tq0(const int ipmt) const { return _tqfit_t0mean[ipmt]; }
  float get_tt0(const int ipmt) const { return _ttfit_t0mean[ipmt]; }
  int get_sampmax(const int ifeech) const { return _sampmax[ifeech]; }
  float get_tcorr(const int ifeech, const int tdc) const {
    return _tcorr_y_interp[ifeech][tdc];
  }
  float get_scorr(const int ifeech, const int adc) const {
    if (adc<0||adc>=16000) return std::numeric_limits<float>::quiet_NaN();
    return _scorr_y_interp[ifeech][adc];
  }

  std::vector<float> get_shape(const int ifeech) const { return _shape_y[ifeech]; }
  std::vector<float> get_sherr(const int ifeech) const { return _sherr_yerr[ifeech]; }

  void set_sampmax(const int ifeech, const int val) { _sampmax[ifeech] = val; }

  int Download_Gains(const std::string& dbfile);
  int Download_TQT0(const std::string& dbfile);
  int Download_TTT0(const std::string& dbfile);
  int Download_SampMax(const std::string& dbfile);
  int Download_Shapes(const std::string& dbfile);
  int Download_TimeCorr(const std::string& dbfile);
  int Download_SlewCorr(const std::string& dbfile);
  int Download_All();

  int Write_SampMax(const std::string& dbfile);
  int Write_TQT0(const std::string& dbfile);
  int Write_TTT0(const std::string& dbfile);

  void Reset_TQT0();
  void Reset_TTT0();
  void Reset_Gains();

  void Update_TQT0(const float dz); // update with new z-vertex
  void Update_TTT0(const float dz);

  // void Dump_to_file(const std::string& what = "ALL");

  void Reset();
  // void Print(Option_t* option) const;

  int Verbosity() { return _verbosity; }
  void Verbosity(int v) { _verbosity = v; }

 private:
  std::unique_ptr<MbdGeom> _mbdgeom{nullptr};

  int _status{0};
  // int          _run_number {0};
  // uint64_t     _timestamp {0};
  std::string _dbfilename;

  int _verbosity{0};

  // Assumes Landau fit
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_integ{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_mpv{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_sigma{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_integerr{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_mpverr{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_sigmaerr{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _qfit_chi2ndf{};

  // T0 offsets, time channels
  std::array<float, bbc_onlmon::MBD_N_PMT> _ttfit_t0mean{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _ttfit_t0meanerr{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _ttfit_t0sigma{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _ttfit_t0sigmaerr{};

  // T0 offsets, charge channels
  std::array<float, bbc_onlmon::MBD_N_PMT> _tqfit_t0mean{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _tqfit_t0meanerr{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _tqfit_t0sigma{};
  std::array<float, bbc_onlmon::MBD_N_PMT> _tqfit_t0sigmaerr{};

  // Peak of waveform
  std::array<int, bbc_onlmon::MBD_N_FEECH> _sampmax{};

  // Waveform Template
  int do_templatefit{0};
  std::array<int, bbc_onlmon::MBD_N_FEECH>   _shape_npts{};      // num points in template
  std::array<float, bbc_onlmon::MBD_N_FEECH> _shape_minrange{};  // in template units (samples)
  std::array<float, bbc_onlmon::MBD_N_FEECH> _shape_maxrange{};  // in template units (samples)
  std::array<std::vector<float>, bbc_onlmon::MBD_N_FEECH> _shape_y{};

  std::array<int, bbc_onlmon::MBD_N_FEECH>   _sherr_npts{};      // num points in template
  std::array<float, bbc_onlmon::MBD_N_FEECH> _sherr_minrange{};  // in template units (samples)
  std::array<float, bbc_onlmon::MBD_N_FEECH> _sherr_maxrange{};  // in template units (samples)
  std::array<std::vector<float>, bbc_onlmon::MBD_N_FEECH> _sherr_yerr{};

  // Fine Timing Corrections
  std::array<int, bbc_onlmon::MBD_N_FEECH>   _tcorr_npts{};      // num points in template
  std::array<float, bbc_onlmon::MBD_N_FEECH> _tcorr_minrange{};  // in template units (delta-TDC)
  std::array<float, bbc_onlmon::MBD_N_FEECH> _tcorr_maxrange{};  // in template units (detta-TDC)
  std::array<std::vector<float>, bbc_onlmon::MBD_N_FEECH> _tcorr_y{};
  std::array<std::vector<float>, bbc_onlmon::MBD_N_FEECH> _tcorr_y_interp{}; // interpolated tcorr

  // Slew Correction
  std::array<int, bbc_onlmon::MBD_N_FEECH>   _scorr_npts{};      // num points in template
  std::array<float, bbc_onlmon::MBD_N_FEECH> _scorr_minrange{};  // in template units (delta-TDC)
  std::array<float, bbc_onlmon::MBD_N_FEECH> _scorr_maxrange{};  // in template units (detta-TDC)
  std::array<std::vector<float>, bbc_onlmon::MBD_N_FEECH> _scorr_y{};
  std::array<std::vector<float>, bbc_onlmon::MBD_N_FEECH> _scorr_y_interp{}; // interpolated tcorr

};

#endif  // ONLBBCCALIB_H

