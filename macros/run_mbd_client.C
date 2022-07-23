#include <CommonFuncs.C>

#include <onlmon/mbd/MbdMonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlmbdmon_client.so)

void mbdDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("mbdmon_hist1", "MBDMON");
  cl->registerHisto("mbdmon_hist2", "MBDMON");
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MBDMON", 1);
  OnlMonDraw *mbdmon = new MbdMonDraw();    // create Drawing Object
  cl->registerDrawer(mbdmon);              // register with client framework
}

void mbdDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MBDMON");         // update histos
  cl->Draw("MBDMON",what);                       // Draw Histos of registered Drawers
}

void mbdPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MBDMON");                          // Create PS files
  return;
}

void mbdHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MBDMON");                        // Create html output
  return;
}
