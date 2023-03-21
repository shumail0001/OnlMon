#include <CommonFuncs.C>

#include <onlmon/cemc/CemcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlcemcmon_client.so)

void cemcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("cemc_occupancy", "CEMCMON_0");
  cl->registerHisto("cemc_runningmean", "CEMCMON_0");
  cl->registerHisto("h2_hcal_hits", "CEMCMON_0");
  cl->registerHisto("h2_hcal_rm", "CEMCMON_0");
  cl->registerHisto("h2_hcal_mean", "CEMCMON_0");
  cl->registerHisto("h_event", "CEMCMON_0");
  cl->registerHisto("h_sectorAvg_total", "CEMCMON_0");
  cl->registerHisto("h_waveform_twrAvg", "CEMCMON_0");
  cl->registerHisto("h_waveform_time", "CEMCMON_0");
  cl->registerHisto("h_waveform_pedestal", "CEMCMON_0");
  for (int ih=0; ih<32; ih++){
    cl->registerHisto(Form("h_rm_sectorAvg_s%d",ih), "CEMCMON_0");
  }
  cl->AddServerHost("localhost");  // check local host first
  // CreateHostList(online);
  //  get my histos from server, the second parameter = 1
  //  says I know they are all on the same node
  cl->requestHistoBySubSystem("CEMCMON_0", 1);
  OnlMonDraw *cemcmon = new CemcMonDraw("CEMCMONDRAW");  // create Drawing Object
  cl->registerDrawer(cemcmon);              // register with client framework
}

void cemcDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  std::cout << 1 << std::endl;
  cl->requestHistoBySubSystem("CEMCMON_0");  // update histos
  std::cout << 2 << std::endl;
  cl->Draw("CEMCMONDRAW", what);  // Draw Histos of registered Drawers
  std::cout << 3 << std::endl;
}

void cemcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("CEMCMONDRAW");                        // Create PS files
  return;
}

void cemcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("CEMCMONDRAW");                      // Create html output
  return;
}
