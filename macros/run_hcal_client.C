#include <CommonFuncs.C>

#include <onlmon/hcal/HcalMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlhcalmon_client.so)

void hcalDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("h2_hcal_hits", "HCALMON_0");
  cl->registerHisto("h2_hcal_rm", "HCALMON_0");
  cl->registerHisto("h2_hcal_mean", "HCALMON_0");
  cl->registerHisto("h_event", "HCALMON_0");
  cl->registerHisto("h_sectorAvg_total", "HCALMON_0");
  cl->registerHisto("h_waveform_twrAvg", "HCALMON_0");
  cl->registerHisto("h_waveform_time", "HCALMON_0");
  cl->registerHisto("h_waveform_pedestal", "HCALMON_0");
  for (int ih=0; ih<32; ih++){
    cl->registerHisto(Form("h_rm_sectorAvg_s%d",ih), "HCALMON_0");
  }
  for (int ieta = 0; ieta < 24; ieta++) {
    for (int iphi = 0; iphi < 64; iphi++) {
      cl->registerHisto(Form("h_rm_tower_%d_%d", ieta, iphi), "HCALMON_0");
    }
  }
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("HCALMON_0", 1);
  OnlMonDraw *hcalmon = new HcalMonDraw("HCALMONDRAW");    // create Drawing Object
  cl->registerDrawer(hcalmon);              // register with client framework
}

void hcalDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("HCALMON_0",1);         // update histos
  cl->Draw("HCALMONDRAW",what);                       // Draw Histos of registered Drawers
}

void hcalPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("HCALMONDRAW");                          // Create PS files
  return;
}

void hcalHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("HCALMONDRAW");                        // Create html output
  return;
}
