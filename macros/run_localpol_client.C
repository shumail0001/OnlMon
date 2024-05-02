#include "CommonFuncs.C"

#include <onlmon/localpol/LocalPolMonDraw.h>

#include <onlmon/OnlMonClient.h>
#include <TString.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonllocalpolmon_client.so)

void localpolDrawInit(const int online = 0)
{

  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name

  TString BeamName[2]={"Blue","Yell"};
  TString MethodName[2]={"Arithmetic","Geometric"};
  TString Orientation[2]={"LR","UD"};
  for(int beam=0; beam<2; beam++){
    for(int method=0; method<2; method++){
      for(int orient=0; orient<2; orient++){
	cl->registerHisto(Form("h_Asym%s%s%s",BeamName[beam].Data(),MethodName[method].Data(),Orientation[orient].Data()),"LOCALPOLMON_0");
	cl->registerHisto(Form("h_AsymScramble%s%s%s",BeamName[beam].Data(),MethodName[method].Data(),Orientation[orient].Data()),"LOCALPOLMON_0");
      }
    }
  }
  cl->registerHisto("h_times","LOCALPOLMON_0");
  // for local host, just call localpolDrawInit(2)
  //CreateSubsysHostlist("localpol_hosts.list", online);

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
