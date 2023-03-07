#include <CommonFuncs.C>

#include <onlmon/intt/InttMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlinttmon_client.so)

void inttDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  // (JB) I'm putting this in the Init() function

  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
   cl->registerHisto("InttNumEvents", "INTTMON_0");
   cl->registerHisto("InttHitMap", "INTTMON_0");
   cl->registerHisto("InttHitMapRef", "INTTMON_0");
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("INTTMON_0", 1);
  OnlMonDraw *inttmon = new InttMonDraw("INTTMONDRAW");    // create Drawing Object
	inttmon->Init(); //registers the hists it will need to the OnlMonClient
  cl->registerDrawer(inttmon);              // register with client framework
}

void inttDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("INTTMON_0");         // update histos
  cl->Draw("INTTMONDRAW",what);                       // Draw Histos of registered Drawers
}

void inttPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("INTTMONDRAW");                          // Create PS files
  return;
}

void inttHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("INTTMONDRAW");                        // Create html output
  return;
}
