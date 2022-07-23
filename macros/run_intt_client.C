#include <CommonFuncs.C>

#include <onlmon/intt/InttMonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlinttmon_client.so)

void inttDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("inttmon_hist1", "INTTMON");
  cl->registerHisto("inttmon_hist2", "INTTMON");
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("INTTMON", 1);
  OnlMonDraw *inttmon = new InttMonDraw();    // create Drawing Object
  cl->registerDrawer(inttmon);              // register with client framework
}

void inttDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("INTTMON");         // update histos
  cl->Draw("INTTMON",what);                       // Draw Histos of registered Drawers
}

void inttPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("INTTMON");                          // Create PS files
  return;
}

void inttHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("INTTMON");                        // Create html output
  return;
}
