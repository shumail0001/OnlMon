#include "CommonFuncs.C"

#include <onlmon/localpol/LocalPolMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonllocalpolmon_client.so)

void localpolDrawInit(const int online = 0)
{

  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("h_example","LOCALPOLMON_0");
  cl->registerHisto("h_example2","LOCALPOLMON_0");

  cl->AddServerHost("localhost");  // check local host first
  //CreateHostList(online);
  
 // get my histos from server, the second parameter = 1 
// says I know they are all on the same node
  cl->requestHistoBySubSystem("LOCALPOLMON_0", 1);

  OnlMonDraw *localpolmon = new LocalPolMonDraw("LOCALPOLMONDRAW");    // create Drawing Object
  cl->registerDrawer(localpolmon);              // register with client framework
}

void localpolDraw(const char *what="ALL")
{

  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("LOCALPOLMON_0",1);      // update histos
  cl->Draw("LOCALPOLMONDRAW",what);                      // Draw Histos of registered Drawers

}

void localpolSavePlot()
{
    OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
    cl->SavePlot("LOCALPOLMONDRAW");                  // Save Plots
    return;
}

void localpolHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("LOCALPOLMONDRAW");                       // create html output
  return;
}
