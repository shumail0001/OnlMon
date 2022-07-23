#include <CommonFuncs.C>

#include <onlmon/mbdll1/Mbdll1MonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlmbdll1mon_client.so)

void mbdll1DrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("mbdll1mon_hist1", "MBDLL1MON");
  cl->registerHisto("mbdll1mon_hist2", "MBDLL1MON");
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MBDLL1MON", 1);
  OnlMonDraw *mbdll1mon = new Mbdll1MonDraw();    // create Drawing Object
  cl->registerDrawer(mbdll1mon);              // register with client framework
}

void mbdll1Draw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MBDLL1MON");         // update histos
  cl->Draw("MBDLL1MON",what);                       // Draw Histos of registered Drawers
}

void mbdll1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MBDLL1MON");                          // Create PS files
  return;
}

void mbdll1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MBDLL1MON");                        // Create html output
  return;
}
