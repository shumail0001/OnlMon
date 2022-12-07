#include <CommonFuncs.C>

#include <onlmon/bbc/BbcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlbbcmon_client.so)

void bbcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("bbc_adc", "BBCMON");
  cl->registerHisto("bbc_tdc", "BBCMON");
  cl->registerHisto("bbc_tdc_overflow", "BBCMON");
  cl->registerHisto("bbc_tdc_armhittime", "BBCMON");
  cl->registerHisto("bbc_zvertex", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1", "BBCMON");
  // cl->registerHisto("bbc_zvertex_zdc", "BBCMON");
  // cl->registerHisto("bbc_zvertex_zdc_scale3", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1_novtx", "BBCMON");
  cl->registerHisto("bbc_zvertex_bbll1_narrowvtx", "BBCMON");
  cl->registerHisto("bbc_nevent_counter", "BBCMON");
  cl->registerHisto("bbc_tzero_zvtx", "BBCMON");
  cl->registerHisto("bbc_prescale_hist", "BBCMON");
  cl->registerHisto("bbc_avr_hittime", "BBCMON");
  cl->registerHisto("bbc_north_hittime", "BBCMON");
  cl->registerHisto("bbc_south_hittime", "BBCMON");
  cl->registerHisto("bbc_north_chargesum", "BBCMON");
  cl->registerHisto("bbc_south_chargesum", "BBCMON");
  // cl->registerHisto("bbc_zvertex_bbll1_zdc", "BBCMON");

  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);

  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("BBCMON", 1);
  OnlMonDraw *bbcmon = new BbcMonDraw();  // create Drawing Object
  cl->registerDrawer(bbcmon);             // register with client framework
}

void bbcDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("BBCMON");        // update histos
  cl->Draw("BBCMON", what);                     // Draw Histos of registered Drawers
}

void bbcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("BBCMON");                         // Create PS files
  return;
}

void bbcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("BBCMON");                       // Create html output
  return;
}
