#include <CommonFuncs.C>

#include <onlmon/tpc/TpcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonltpcmon_client.so)

void tpcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("tpcmon_hist1", "TPCMON_0");
  cl->registerHisto("tpcmon_hist2", "TPCMON_0");
  cl->registerHisto("NorthSideADC", "TPCMON_0");
  cl->registerHisto("SouthSideADC", "TPCMON_0");
  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("TPCMON_0", 1);
  OnlMonDraw *tpcmon = new TpcMonDraw("TPCMONDRAW");  // create Drawing Object
  cl->registerDrawer(tpcmon);             // register with client framework
}

void tpcDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("TPCMON_0");        // update histos
  cl->Draw("TPCMONDRAW", what);                     // Draw Histos of registered Drawers
}

void tpcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("TPCMONDRAW");                         // Create PS files
  return;
}

void tpcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("TPCMONDRAW");                       // Create html output
  return;
}
