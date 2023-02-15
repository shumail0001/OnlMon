#include <CommonFuncs.C>

#include <onlmon/example/MyMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmymon_client.so)

void exampleDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("mymon_hist1", "MYMON");
  cl->registerHisto("mymon_hist2", "MYMON");
  cl->registerHisto("mymon_hist1", "MYMON2");
  cl->registerHisto("mymon_hist2", "MYMON2");
  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MYMON", 1);
  OnlMonDraw *mymon = new MyMonDraw();  // create Drawing Object
  cl->registerDrawer(mymon);            // register with client framework
}

void exampleDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MYMON");         // update histos
  cl->Draw("MYMON", what);                      // Draw Histos of registered Drawers
}

void examplePS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MYMON");                          // Create PS files
  return;
}

void exampleHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MYMON");                        // Create html output
  return;
}
