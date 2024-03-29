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
  //cl->registerHisto("h_example","LOCALPOLMON_0");
  //cl->registerHisto("h_example2","LOCALPOLMON_0");


  //cl->registerHisto("h_BlueCountsUD","LOCALPOLMON_0");
  //cl->registerHisto("h_BlueCountsLR","LOCALPOLMON_0");
  //cl->registerHisto("h_YellCountsUD","LOCALPOLMON_0");
  //cl->registerHisto("h_YellCountsLR","LOCALPOLMON_0");
  //
  //cl->registerHisto("h_BlueCountsScrambleUD","LOCALPOLMON_0");
  //cl->registerHisto("h_BlueCountsScrambleLR","LOCALPOLMON_0");
  //cl->registerHisto("h_YellCountsScrambleUD","LOCALPOLMON_0");
  //cl->registerHisto("h_YellCountsScrambleLR","LOCALPOLMON_0");


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
