#include "TpotMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <phool/phool.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

namespace
{
  //! make canvas editable in creator, and non-editable in destructor
  class CanvasEditor
  {
    public:
    CanvasEditor( TCanvas* cv ):m_cv(cv)
    { if( m_cv ) m_cv->SetEditable(true); }

    ~CanvasEditor()
    { if( m_cv ) m_cv->SetEditable(false); }

    private:
    TCanvas* m_cv = nullptr;
  };

  TPad* create_transparent_pad( const std::string& name )
  {
    auto transparent = new TPad( (name+"_transparent").c_str(), "", 0, 0, 1, 1);
    transparent->SetFillStyle(4000);
    transparent->Draw();
    return transparent;
  };

  TPad* get_transparent_pad( const std::string& name )
  { return static_cast<TPad*>( gROOT->FindObject( (name+"_transparent").c_str() ) ); }

}

//__________________________________________________________________________________
TpotMonDraw::TpotMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars.reset( new OnlMonDB(ThisName) );
  return;
}

//__________________________________________________________________________________
int TpotMonDraw::Init()
{ return 0; }

//__________________________________________________________________________________
TCanvas* TpotMonDraw::get_canvas(const std::string& name, bool clear )
{
  auto cv = dynamic_cast<TCanvas*>( gROOT->FindObject( name.c_str() ) );
  if( !cv ) cv = create_canvas( name );
  if( cv && clear ) cv->Clear("D");
  return cv;
}

//__________________________________________________________________________________
TCanvas* TpotMonDraw::create_canvas(const std::string &name)
{
  
  std::cout << "TpotMonDraw::create_canvas - name: " << name << std::endl;
  
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  if (name == "TPOT_hv_onoff")
  {

    // xpos (-1) negative: do not draw menu bar
    auto cv = m_canvas[0] = new TCanvas(name.c_str(), "TpotMon HV On/Off monitor", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();

    cv->Divide( 1, 2 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_fee_onoff") {

    auto cv = m_canvas[1] = new TCanvas(name.c_str(), "TpotMon FEE On/Off monitor", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();

    cv->Divide( 1, 2 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_adc_vs_sample") {

    auto cv = m_canvas[2] = new TCanvas(name.c_str(), "TpotMon adc vs sample" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_hit_charge") {

    auto cv = m_canvas[3] = new TCanvas(name.c_str(), "TpotMon hit charge" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_hit_multiplicity") {

    auto cv = m_canvas[4] = new TCanvas(name.c_str(), "TpotMon hit multiplicity" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_hit_vs_channel") {

    auto cv = m_canvas[5] = new TCanvas(name.c_str(), "TpotMon hit vs channel" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_cluster_size") {

    auto cv = m_canvas[6] = new TCanvas(name.c_str(), "TpotMon cluster size" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_cluster_charge") {

    auto cv = m_canvas[7] = new TCanvas(name.c_str(), "TpotMon cluster charge" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  } else if (name == "TPOT_cluster_multiplicity") {

    auto cv = m_canvas[8] = new TCanvas(name.c_str(), "TpotMon cluster multiplicity" );
    cv->Divide( 4, 4 );

    // this one is used to plot the run number on the canvas
    create_transparent_pad(name)->Draw();
    cv->SetEditable(false);
    return cv;
  }
  return nullptr;
}

//_______________________________________________________________________________
int TpotMonDraw::Draw(const std::string &what)
{
  std::cout << "TpotMonDraw::Draw - what: " << what << std::endl;

  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "TPOT_hv_onoff")
  {
    iret += draw_hv_onoff();
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_fee_onoff")
  {
    iret += draw_fee_onoff();
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_adc_vs_sample")
  {
    iret += draw_array("TPOT_adc_vs_sample", get_histograms( "m_adc_sample" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_charge")
  {
    iret += draw_array("TPOT_hit_charge", get_histograms( "m_hit_charge" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_multiplicity")
  {
    iret += draw_array("TPOT_hit_multiplicity", get_histograms( "m_hit_multiplicity" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_hit_vs_channel")
  {
    iret += draw_array("TPOT_hit_vs_channel", get_histograms( "m_hit_vs_channel" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_cluster_size")
  {
    iret += draw_array("TPOT_cluster_size", get_histograms( "m_cluster_size" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_cluster_charge")
  {
    iret += draw_array("TPOT_cluster_charge", get_histograms( "m_cluster_charge" ) );
    ++idraw;
  }

  if (what == "ALL" || what == "TPOT_cluster_multiplicity")
  {
    iret += draw_array("TPOT_cluster_multiplicity", get_histograms( "m_cluster_multiplicity" ) );
    ++idraw;
  }

  if (!idraw)
  {
    std::cout << PHWHERE << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

//_______________________________________________________________________________
int TpotMonDraw::MakePS(const std::string &what)
{
  auto cl = OnlMonClient::instance();
  const int iret = Draw(what);
  if (iret) return iret;

  for( std::size_t i =0; i < m_canvas.size(); ++i )
  {
    const auto& cv = m_canvas[i];
    if( cv )
    {
      std::ostringstream filename;
      filename << ThisName << "_" << i+1 << "_" << cl->RunNumber() << ".ps";
      cv->Print( filename.str().c_str() );
    }
  }

  return 0;
}

//__________________________________________________________________________________
int TpotMonDraw::MakeHtml(const std::string &what)
{
  const int iret = Draw(what);
  if (iret) return iret;

  auto cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  for( std::size_t i =0; i < m_canvas.size(); ++i )
  {
    const auto& cv = m_canvas[i];
    if( cv )
    {
      const auto pngfile = cl->htmlRegisterPage(*this, cv->GetName(), Form("%lu", i+1), "png");
      cl->CanvasToPng(cv, pngfile);
    }
  }

  // log
  {
    const std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
    std::ofstream out(logfile.c_str());
    out
      << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
      << "</TITLE></HEAD>"
      << std::endl
      << "<P>Some log file output would go here."
      << std::endl;
    out.close();
  }

  // status
  {
    const std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
    std::ofstream out(status.c_str());
    out
      << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
      << "</TITLE></HEAD>"
      << std::endl
      << "<P>Some status output would go here."
      << std::endl;
    out.close();
    cl->SaveLogFile(*this);
  }

  return 0;
}

//__________________________________________________________________________________
void TpotMonDraw::draw_time( TPad* pad )
{
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment

  std::ostringstream runnostream;
  auto cl = OnlMonClient::instance();
  time_t evttime = cl->EventTime("CURRENT");

  runnostream
    << ThisName << "_1 Run " << cl->RunNumber()
    << ", Time: " << ctime(&evttime);

  pad->cd();
  PrintRun.DrawText(0.5, 1., runnostream.str().c_str());
}

//__________________________________________________________________________________
int TpotMonDraw::draw_hv_onoff()
{
  // get histograms
  auto cl = OnlMonClient::instance();
  auto m_hv_onoff_phi = cl->getHisto("m_hv_onoff_phi");
  auto m_hv_onoff_z = cl->getHisto("m_hv_onoff_z");

  auto cv = get_canvas("TPOT_hv_onoff");
  auto transparent = get_transparent_pad("TPOT_hv_onoff");
  if( !cv ) return -1;

  CanvasEditor cv_edit(cv);

  if( m_hv_onoff_phi && m_hv_onoff_z )
  {
    cv->cd(1);
    m_hv_onoff_phi->DrawCopy();

    cv->cd(2);
    m_hv_onoff_z->DrawCopy();

    draw_time(transparent);
    return 0;

  } else {

    DrawDeadServer(transparent);
    return -1;

  }
}

//__________________________________________________________________________________
int TpotMonDraw::draw_fee_onoff()
{
  // get histograms
  auto cl = OnlMonClient::instance();
  auto m_fee_onoff_phi = cl->getHisto("m_fee_onoff_phi");
  auto m_fee_onoff_z = cl->getHisto("m_fee_onoff_z");

  auto cv = get_canvas("TPOT_fee_onoff");
  auto transparent = get_transparent_pad("TPOT_fee_onoff");
  if( !cv ) return -1;

  CanvasEditor cv_edit(cv);

  if( m_fee_onoff_phi && m_fee_onoff_z )
  {
    cv->cd(1);
    m_fee_onoff_phi->DrawCopy();

    cv->cd(2);
    m_fee_onoff_z->DrawCopy();

    draw_time(transparent);
    return 0;

  } else {

    DrawDeadServer(transparent);
    return -1;

  }
}

//__________________________________________________________________________________
TpotMonDraw::histogram_array_t TpotMonDraw::get_histograms( const std::string& name )
{
  histogram_array_t out{{nullptr}};

  // detector names (ordered by tile_id (0 to 8) and layer (P or Z)
  static const std::array<std::string, n_detectors> detector_names =
  {
    "M5P",  "M5Z",
    "M8P",  "M8Z",
    "M4P",  "M4Z",
    "M10P", "M10Z",
    "M9P",  "M9Z",
    "M2P",  "M2Z",
    "M6P",  "M6Z",
    "M7P",  "M7Z"
  };

  auto cl = OnlMonClient::instance();
  for( int i = 0; i < n_detectors; ++i )
  { out[i] = cl->getHisto( Form( "%s_%s", name.c_str(), detector_names[i].c_str() ) ); }

  return out;
}

//__________________________________________________________________________________
int TpotMonDraw::draw_array( const std::string& name, const TpotMonDraw::histogram_array_t& histograms )
{
  auto cv = get_canvas(name);
  auto transparent = get_transparent_pad(name);
  if( !cv ) return -1;


  bool drawn = false;
  CanvasEditor cv_edit(cv);
  for( size_t i = 0l; i < histograms.size(); ++i )
  {
    if( histograms[i] )
    {
      cv->cd(i+1);
      histograms[i+1]->DrawCopy();
      drawn = true;
    }
  }

  if( drawn )
  {
    draw_time(transparent);
    return 0;
  } else {
    DrawDeadServer(transparent);
    return -1;
  }
}
