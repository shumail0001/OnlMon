#include <CommonFuncs.C>

#include <onlmon/bbcll1/Bbcll1MonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlbbcll1mon_client.so)

void bbcll1DrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("bbcll1mon_hist1", "BBCLL1MON_0");
  cl->registerHisto("bbcll1mon_hist2", "BBCLL1MON_0");
  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("BBCLL1MON_0", 1);
  OnlMonDraw *bbcll1mon = new Bbcll1MonDraw("BBCLL1MONDRAW");  // create Drawing Object
  cl->registerDrawer(bbcll1mon);                // register with client framework
}

void bbcll1Draw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("BBCLL1MON_0");     // update histos
  cl->Draw("BBCLL1MONDRAW", what);                  // Draw Histos of registered Drawers
}

void bbcll1PS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("BBCLL1MONDRAW");                      // Create PS files
  return;
}

void bbcll1Html()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("BBCLL1MONDRAW");                    // Create html output
  return;
}
