#include <CommonFuncs.C>
#include <onlmon/tpot/TpotMonDraw.h>
#include <onlmon/tpot/TpotDefs.h>
#include <onlmon/OnlMonClient.h>

#include <array>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpotmon_client.so)

void tpotDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  // register all histograms
  cl->registerHisto("m_hv_onoff_phi", "TPOTMON");
  cl->registerHisto("m_hv_onoff_z", "TPOTMON");

  cl->registerHisto("m_fee_onoff_phi", "TPOTMON");
  cl->registerHisto("m_fee_onoff_z", "TPOTMON");

  for( const std::string& hname: { "m_adc_sample", "m_hit_charge", "m_hit_multiplicity", "m_hit_vs_channel" } )
  {
    for( const auto& detname : TpotDefs::detector_names )
    { cl->registerHisto( hname+"_"+detname, "TPOTMON" ); }
  }
  
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("TPOTMON", 1);
  OnlMonDraw *tpotmon = new TpotMonDraw();  // create Drawing Object
  cl->registerDrawer(tpotmon);              // register with client framework
}

void tpotDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("TPOTMON");       // update histos
  cl->Draw("TPOTMON", what);                    // Draw Histos of registered Drawers
}

void tpotPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("TPOTMON");                        // Create PS files
  return;
}

void tpotHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("TPOTMON");                      // Create html output
  return;
}
