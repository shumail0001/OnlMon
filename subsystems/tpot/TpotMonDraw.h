#ifndef TPOT_TPOTMONDRAW_H
#define TPOT_TPOTMONDRAW_H

#include "MicromegasGeometry.h"

#include <micromegas/MicromegasDefs.h>
#include <micromegas/MicromegasMapping.h>
#include <onlmon/OnlMonDraw.h>

#include <TFile.h>

#include <array>
#include <memory>
#include <string>

class OnlMonDB;
class TCanvas;
class TH1;
class TH2Poly;
class TPad;

class TpotMonDraw : public OnlMonDraw
{
  public:

  /// constructor
  TpotMonDraw(const std::string &name);

  /// destructor
  ~TpotMonDraw() override = default;

  int Init() override;
  int Draw(const std::string &what = "ALL") override;
  int MakeHtml(const std::string &what = "ALL") override;
  int SavePlot(const std::string &what = "ALL", const std::string &type = "png") override;

  // get detector names
  std::vector<std::string> get_detnames_sphenix() const
  { return m_detnames_sphenix; }

  //! calibration file
  void set_calibration_file( const std::string& value )
  { m_calibration_filename = value; }


  // define sample window
  using sample_window_t = std::pair<int, int>;
  void set_sample_window( const sample_window_t& value )
  { m_sample_window = value; }

  // define signal sample window
  void set_sample_window_signal( const sample_window_t& value )
  { m_sample_window_signal = value; }

  /// set number of RMS sigma used to defined static threshold on a given channel
  void set_n_sigma( double value ) { m_n_sigma = value; }

  private:

  // draw message to specify that server is dead
  int DrawDeadServer(TPad*) override;

  // draw run and time in a given pad
  void draw_time( TPad*);

  TCanvas* get_canvas(const std::string& name, bool clear = true );
  TCanvas* create_canvas(const std::string &name);

  int draw_counters();
  int draw_detector_occupancy();
  int draw_resist_occupancy();
  int draw_server_statistics();

  using histogram_array_t = std::array<TH1*, MicromegasDefs::m_nfee>;

  using threshold_array_t = std::array<double, MicromegasDefs::m_nfee>;

  // drawing options
  enum DrawOptions
  {
    None = 0,
    Logx = 1<<0,
    Logy = 1<<1,
    Logz = 1<<2,
    Colz = 1<<3,
    MatchRange = 1<<4,
    Normalize = 1<<5
  };

  /// get histogram by name
  TH1* get_histogram( const std::string& name ) const;

  /// get detector dependent histogram array from base name
  histogram_array_t get_histograms( const std::string& name ) const;

  /// get histogram by name
  TH1* get_ref_histogram( const std::string& name ) const;

  /// get detector dependent histogram array from base name
  histogram_array_t get_ref_histograms( const std::string& name ) const;

  /// get detector dependent histogram array from base name
  histogram_array_t get_ref_histograms_scaled( const std::string& name ) const;

  /// get scale factor for reference histograms
  /** the normaliztion factor is based on the ratio of number of events in the event counters histogram */
  double get_ref_scale_factor() const;

  /// normalize reference histogram
  /*
   * a copy of the source histogram is done. It must be deleted after the fact
   */
  TH1* normalize( TH1*, double scale = 1 ) const;

  /// draw histogram array
  int draw_array( const std::string& name, const histogram_array_t& array, unsigned int options = DrawOptions::None, double norm_factor = 1 )
  { return draw_array( name, array, {{nullptr}}, options, norm_factor ); }

  /// draw histogram array and reference histgorams
  int draw_array( const std::string& name, const histogram_array_t&, const histogram_array_t& /*reference*/, unsigned int /*options*/ = DrawOptions::None, double /*norm_factor*/ = 1 );

  /// draw detector names in current canvas
  /** only works if canvas contains one of the properly formated TH2Poly histograms */
  void draw_detnames_sphenix( const std::string& suffix = std::string());

  /// draw occupancy
  void draw_occupancy( TH2Poly* );

  /// keep track of trigger count
  double m_triggercnt = 0;

  /// keep track of heartbeat count
  double m_heartbeatcnt = 0;

  /// mapping
  MicromegasMapping m_mapping;

  /// geometry
  MicromegasGeometry m_geometry;

  //! calibration filename
  std::string m_calibration_filename;

  //! keep track on per channel thresholds, used to define signal hits
  histogram_array_t m_threshold_histograms;

  //! keep track of mean threshold per FEE
  threshold_array_t m_mean_thresholds;

  /// detector names
  std::vector<std::string> m_detnames_sphenix;

  /// needed to get time axis right
  int TimeOffsetTicks = -1;

  // sample window
  sample_window_t m_sample_window = {0, 50};

  // sample window
  sample_window_t m_sample_window_signal = {3, 18};

  //! number of RMS sigma used to define threshold
  double m_n_sigma = 5;

  // reference histograms filename
  std::string m_ref_histograms_filename;

  // reference histograms TFile
  std::unique_ptr<TFile> m_ref_histograms_tfile;

  // canvases
  std::vector<TCanvas*> m_canvas;
  std::unique_ptr<OnlMonDB> dbvars;
};

#endif /* TPOT_TPOTMONDRAW_H */
