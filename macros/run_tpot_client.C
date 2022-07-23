#include <CommonFuncs.C>

#include <onlmon/tpot/TpotMonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonltpotmon_client.so)

void tpotDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("tpotmon_hist1", "TPOTMON");
  cl->registerHisto("tpotmon_hist2", "TPOTMON");
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("TPOTMON", 1);
  OnlMonDraw *tpotmon = new TpotMonDraw();    // create Drawing Object
  cl->registerDrawer(tpotmon);              // register with client framework
}

void tpotDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("TPOTMON");         // update histos
  cl->Draw("TPOTMON",what);                       // Draw Histos of registered Drawers
}

void tpotPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("TPOTMON");                          // Create PS files
  return;
}

void tpotHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("TPOTMON");                        // Create html output
  return;
}
