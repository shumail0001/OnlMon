#include <CommonFuncs.C>

#include <onlmon/mvtx/MvtxMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_client.so)

void mvtxDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  //cl->registerHisto("mvtxmon_hist1", "MVTXMON");
  //cl->registerHisto("mvtxmon_hist2", "MVTXMON");
  cl->registerHisto("MVTXMON/RawData/mvtxmon_ChipStaveOcc", "MVTXMON");
  cl->registerHisto("MVTXMON/RawData/mvtxmon_ChipStave1D", "MVTXMON");
  cl->registerHisto("MVTXMON/RawData/mvtxmon_ChipFiredHis", "MVTXMON");
  cl->registerHisto("MVTXMON/RawData/mvtxmon_EvtHitChip", "MVTXMON");
  cl->registerHisto("MVTXMON/RawData/mvtxmon_EvtHitDis", "MVTXMON");
  cl->registerHisto("MVTXMON/General/General_Occupancy", "MVTXMON");
  cl->registerHisto("MVTXMON/LaneStatus/laneStatusOverviewFlagWARNING", "MVTXMON");
  cl->registerHisto("MVTXMON/LaneStatus/laneStatusOverviewFlagERROR", "MVTXMON");
  cl->registerHisto("MVTXMON/LaneStatus/laneStatusOverviewFlagFAULT", "MVTXMON");

  //FEE
  cl->registerHisto("MVTXMON/FEE/TriggerVsFeeid", "MVTXMON");
  cl->registerHisto("MVTXMON/FEE/TriggerFlag", "MVTXMON");
  cl->registerHisto("MVTXMON/FEE/LaneInfo", "MVTXMON");

std::string mLaneStatusFlag[3] = { "WARNING", "ERROR", "FAULT" };

  for (int i = 0; i < MvtxMonDraw::NFlags; i++) {
    cl->registerHisto(Form("MVTXMON/LaneStatus/laneStatusFlag%s", mLaneStatusFlag[i].c_str()), "MVTXMON");
    cl->registerHisto(Form("MVTXMON/LaneStatus/laneStatusFlagCumulative%s", mLaneStatusFlag[i].c_str()), "MVTXMON");
  }

  for(int i = 0; i < 3; i++){
    cl->registerHisto(Form("MVTXMON/LaneStatusSummary/LaneStatusSummaryL%i", i), "MVTXMON");
  }

  cl->registerHisto("MVTXMON/LaneStatusSummary/LaneStatusSummaryIB", "MVTXMON");

//raw task
  cl->registerHisto("MVTXMON/General/ErrorPlots", "MVTXMON");
  cl->registerHisto("MVTXMON/General/ErrorFile", "MVTXMON");
const int NStaves[3] = { 12, 16, 20 };

for (int aLayer = 0; aLayer < 3; aLayer++) {
  cl->registerHisto(Form("MVTXMON/Occupancy/Layer%dOccupancy", aLayer), "MVTXMON");
  cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dEtaPhi", aLayer, aLayer), "MVTXMON");
  cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStave", aLayer, aLayer), "MVTXMON");
  for (int aStave = 0; aStave < NStaves[aLayer]; aStave++) {
    for (int iChip = 0; iChip < 9; iChip++) {
      cl->registerHisto(Form("MVTXMON/chipHitmapL%dS%dC%d", aLayer, aStave, iChip), "MVTXMON");
    }
  }
}

 //fhr
  cl->registerHisto("MVTXMON/General/ErrorVsFeeid", "MVTXMON");
  cl->registerHisto("MVTXMON/General/General_Occupancy", "MVTXMON");
  cl->registerHisto("MVTXMON/General/Noisy_Pixel", "MVTXMON");

  for (int mLayer = 0; mLayer < 3; mLayer++) {
    cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dDeadChipPos", mLayer, mLayer), "MVTXMON");
    cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dAliveChipPos", mLayer, mLayer), "MVTXMON");
    //cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStaveC", mLayer, mLayer), "MVTXMON");
    cl->registerHisto(Form("MVTXMON/Occupancy/Layer%dOccupancy/LOG", mLayer), "MVTXMON");
  }

  cl->registerHisto("MVTXMON/Occupancy/TotalDeadChipPos", "MVTXMON");
  cl->registerHisto("MVTXMON/Occupancy/TotalAliveChipPos", "MVTXMON");



  cl->AddServerHost("localhost");  // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MVTXMON_0", 1);
  OnlMonDraw *mvtxmon = new MvtxMonDraw("MVTXMONDRAW");  // create Drawing Object
  cl->registerDrawer(mvtxmon);              // register with client framework
}

void mvtxDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MVTXMON_0");       // update histos
  cl->Draw("MVTXMONDRAW", what);                    // Draw Histos of registered Drawers
}

void mvtxPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MVTXMONDRAW");                        // Create PS files
  return;
}

void mvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MVTXMONDRAW");                      // Create html output
  return;
}
