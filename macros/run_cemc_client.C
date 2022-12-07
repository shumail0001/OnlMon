#include <CommonFuncs.C>

#include <onlmon/cemc/CemcMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlcemcmon_client.so)

void cemcDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("cemc_occupancy", "CEMCMON");
  cl->registerHisto("cemc_runningmean", "CEMCMON");
  cl->AddServerHost("localhost");   // check local host first
  //CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("CEMCMON", 1);
  OnlMonDraw *cemcmon = new CemcMonDraw();    // create Drawing Object
  cl->registerDrawer(cemcmon);              // register with client framework
}

void cemcDraw(const char *what="Standard")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  std::cout << 1 << std::endl;
  cl->requestHistoBySubSystem("CEMCMON");         // update histos
  std::cout << 2 << std::endl;
  cl->Draw("CEMCMON",what);                       // Draw Histos of registered Drawers
  std::cout << 3 << std::endl;
}

void cemcPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("CEMCMON");                          // Create PS files
  return;
}

void cemcHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("CEMCMON");                        // Create html output
  return;
}
