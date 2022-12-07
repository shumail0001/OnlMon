#include <CommonFuncs.C>

#include <onlmon/epd/EpdMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlepdmon_client.so)

void epdDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("epdmon_hist1", "EPDMON");
  cl->registerHisto("epdmon_hist2", "EPDMON");
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("EPDMON", 1);
  OnlMonDraw *epdmon = new EpdMonDraw();    // create Drawing Object
  cl->registerDrawer(epdmon);              // register with client framework
}

void epdDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("EPDMON");         // update histos
  cl->Draw("EPDMON",what);                       // Draw Histos of registered Drawers
}

void epdPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("EPDMON");                          // Create PS files
  return;
}

void epdHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("EPDMON");                        // Create html output
  return;
}
