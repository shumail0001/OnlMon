#include <ServerFuncs.C>

#include <onlmon/OnlMonServer.h>
#include <onlmon/MyMon.h>

R__LOAD_LIBRARY(libonlmymon.so)
//R__LOAD_LIBRARY(libfun4all.so)



void run_example(const char *prdffile = "data.prdf")
{
//  gROOT->ProcessLine(".L $ONLMON_MACROS/ServerFuncs.C");

  OnlMon *m = new MyMon();      // create subsystem Monitor object
//  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
//  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);
  return ;
}

/*

void exampleDrawInit(const int online = 0)
{
  gSystem->Load("libonlmymon.so");
  gSystem->Load("libonlmonclient.so");
  gROOT->ProcessLine(".L CommonFuncs.C");
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("mymon_hist1", "MYMON");
  cl->registerHisto("mymon_hist2", "MYMON");
     cl->AddServerHost("localhost");   // check local host first
   //CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MYMON", 1);
  OnlMonDraw *mymon = new MyMonDraw();    // create Drawing Object
  cl->registerDrawer(mymon);              // register with client framework
}

void exampleDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MYMON");         // update histos
  cl->Draw("MYMON",what);                       // Draw Histos of registered Drawers
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
*/
