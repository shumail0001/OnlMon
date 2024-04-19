#include "CommonFuncs.C"

#include <onlmon/daq/DaqMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonldaqmon_client.so)

void daqDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();

  for(int serverid = 0; serverid < 20; serverid++)
  {
      cl->registerHisto("h_gl1_clock_diff", Form("DAQMON_%d",serverid));
      cl->registerHisto("h_gl1_clock_diff_capture", Form("DAQMON_%d",serverid));
  }
      
  //cl->AddServerHost("localhost");
  CreateHostList(online);

  for(int serverid = 0; serverid < 20; serverid++)
  {
      cl->requestHistoBySubSystem(Form("DAQMON_%d",serverid), 1);
  }

  OnlMonDraw *daqmon = new DaqMonDraw("DAQMONDRAW");  // create Drawing Object
  cl->registerDrawer(daqmon);             // register with client framework
}

void daqDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  for(int serverid = 0; serverid < 20; serverid++)
  {
      cl->requestHistoBySubSystem(Form("DAQMON_%d",serverid), 1);
  }
  cl->Draw("DAQMONDRAW", what);                     // Draw Histos of registered Drawers
}

void daqSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("DAQMONDRAW");                         // Save Plots
  return;
}

void daqHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("DAQMONDRAW");                       // Create html output
  return;
}

