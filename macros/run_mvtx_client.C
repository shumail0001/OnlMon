#include <CommonFuncs.C>

#include <onlmon/mvtx/MvtxMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_client.so)

void mvtxDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  //cl->registerHisto("mvtxmon_hist1", "MVTXMON_0");
  //cl->registerHisto("mvtxmon_hist2", "MVTXMON_0");
  cl->registerHisto("mvtxmon_ChipStaveOcc", "MVTXMON_0");
  cl->registerHisto("mvtxmon_ChipStave1D", "MVTXMON_0");
  cl->registerHisto("mvtxmon_ChipFiredHis", "MVTXMON_0");
  cl->registerHisto("mvtxmon_EvtHitChip", "MVTXMON_0");
  cl->registerHisto("mvtxmon_EvtHitDis", "MVTXMON_0");

   for(int i = 0; i < MvtxMonDraw::NSTAVE; i++){
		for(int j = 0; j < MvtxMonDraw::NCHIP; j++){
			cl->registerHisto(Form("mvtxmon_HitMap_%d_%d",i,j), "MVTXMON_0");
    }
  }

  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MVTXMON_0", 1);
  OnlMonDraw *mvtxmon = new MvtxMonDraw("MVTXMONDRAW");  // create Drawing Object
  cl->registerDrawer(mvtxmon);              // register with client framework
}

void mvtxDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MVTXMON_0");       // update histos
  cl->Draw("MVTXMONDRAW", what);                    // Draw Histos of registered Drawers
}

void mvtxPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MVTXMONDRAW");                        // Create PS files
  return;
}

void mvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MVTXMONDRAW");                      // Create html output
  return;
}
