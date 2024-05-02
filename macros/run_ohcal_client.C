#include "CommonFuncs.C"

#include <onlmon/hcal/HcalMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlhcalmon_client.so)

void ohcalDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  char HCALMON_STR[100];
  for (int i = 0; i < 2; i++)
  {
    sprintf(HCALMON_STR, "OHCALMON_%i", i);
    cl->registerHisto("h2_hcal_hits", HCALMON_STR);
    cl->registerHisto("h_caloPack_gl1_clock_diff", HCALMON_STR);
    cl->registerHisto("h2_hcal_rm", HCALMON_STR);
    cl->registerHisto("h2_hcal_mean", HCALMON_STR);
    cl->registerHisto("h2_hcal_time", HCALMON_STR);
    cl->registerHisto("h2_hcal_waveform", HCALMON_STR);
    cl->registerHisto("h2_hcal_correlation", HCALMON_STR);
    cl->registerHisto("h_event", HCALMON_STR);
    cl->registerHisto("h_sectorAvg_total", HCALMON_STR);
    cl->registerHisto("h_waveform_twrAvg", HCALMON_STR);
    cl->registerHisto("h_waveform_time", HCALMON_STR);
    cl->registerHisto("h_waveform_pedestal", HCALMON_STR);
    cl->registerHisto("h_ntower", HCALMON_STR);
    cl->registerHisto("h1_packet_chans", HCALMON_STR);
    cl->registerHisto("h1_packet_length", HCALMON_STR);
    cl->registerHisto("h1_packet_number", HCALMON_STR);
    cl->registerHisto("h1_packet_event", HCALMON_STR);
    cl->registerHisto("h_evtRec", HCALMON_STR);
    cl->registerHisto("h2_hcal_hits_trig1", HCALMON_STR);
    cl->registerHisto("h2_hcal_hits_trig2", HCALMON_STR);
    cl->registerHisto("h2_hcal_hits_trig3", HCALMON_STR);
    cl->registerHisto("h2_hcal_hits_trig4", HCALMON_STR);
    cl->registerHisto("h_hcal_trig", HCALMON_STR);

    for (int ih = 0; ih < 32; ih++)
    {
      cl->registerHisto(Form("h_rm_sectorAvg_s%d", ih), HCALMON_STR);
    }
    for (int ieta = 0; ieta < 24; ieta++)
    {
      for (int iphi = 0; iphi < 64; iphi++)
      {
        cl->registerHisto(Form("h_rm_tower_%d_%d", ieta, iphi), HCALMON_STR);
      }
    }
  }

  // for local host, just call ohcalDrawInit(2)
  CreateSubsysHostlist("hcal_hosts.list", online);

  // says I know they are all on the same node
  for (int i = 0; i < 2; i++)
  {
    sprintf(HCALMON_STR, "OHCALMON_%i", i);
    cl->requestHistoBySubSystem(HCALMON_STR, 1);
  }
  OnlMonDraw *hcalmon = new HcalMonDraw("OHCALMONDRAW");  // create Drawing Object
  
  cl->registerDrawer(hcalmon);                           // register with client framework
}

void ohcalDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  char HCALMON_STR[100];
  for (int i = 0; i < 2; i++)
  {
    sprintf(HCALMON_STR, "OHCALMON_%i", i);
    cl->requestHistoBySubSystem(HCALMON_STR, 1);
  }
  cl->Draw("OHCALMONDRAW", what);  // Draw Histos of registered Drawers
}

void ohcalSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("OHCALMONDRAW");                    // Save Plots
  return;
}

void ohcalHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("OHCALMONDRAW");                  // Create html output
  return;
}
