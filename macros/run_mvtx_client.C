#include "CommonFuncs.C"

#include <onlmon/mvtx/MvtxMonDraw.h>

#include <onlmon/OnlMonClient.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_client.so)

void mvtxDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  std::string mLaneStatusFlag[3] = {"WARNING", "ERROR", "FAULT"};
  const int NStaves[3] = {12, 16, 20};

  for (int iflx = 0; iflx < 6; iflx++)
  {
    std::string instanceName = "MVTXMON_" + std::to_string(iflx);
    std::cout << instanceName << std::endl;
    cl->registerHisto("MVTXMON_RawData_ChipStaveOcc", instanceName);
    cl->registerHisto("MVTXMON_RawData_ChipStave1D", instanceName);
    cl->registerHisto("MVTXMON_RawData_ChipFiredHis", instanceName);
    cl->registerHisto("MVTXMON_RawData_EvtHitChip", instanceName);
    cl->registerHisto("MVTXMON_RawData_EvtHitDis", instanceName);
    cl->registerHisto("MVTXMON_General_Occupancy", instanceName);
    cl->registerHisto("MVTXMON_LaneStatus_laneStatusOverviewFlagWARNING", instanceName);
    cl->registerHisto("MVTXMON_LaneStatus_laneStatusOverviewFlagERROR", instanceName);
    cl->registerHisto("MVTXMON_LaneStatus_laneStatusOverviewFlagFAULT", instanceName);

    // FEE
    cl->registerHisto("MVTXMON_FEE_TriggerVsFeeid", instanceName);
    cl->registerHisto("MVTXMON_FEE_TriggerFlag", instanceName);
    // cl->registerHisto("MVTXMON/FEE/LaneInfo", instanceName);

    for (int i = 0; i < MvtxMonDraw::NFlags; i++)
    {
      cl->registerHisto(Form("MVTXMON_LaneStatus_laneStatusFlag%s", mLaneStatusFlag[i].c_str()), instanceName);
      cl->registerHisto(Form("MVTXMON_LaneStatus_laneStatusFlagCumulative%s", mLaneStatusFlag[i].c_str()), instanceName);
    }

    for (int i = 0; i < 3; i++)
    {
      cl->registerHisto(Form("MVTXMON_LaneStatusSummary_LaneStatusSummaryL%i", i), instanceName);
    }

    cl->registerHisto("MVTXMON_LaneStatusSummary_LaneStatusSummary", instanceName);

    // raw task
    cl->registerHisto("MVTXMON_General_ErrorPlots", instanceName);
    cl->registerHisto("MVTXMON_General_ErrorFile", instanceName);

    for (int aLayer = 0; aLayer < 3; aLayer++)
    {
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%dOccupancy", aLayer), instanceName);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dEtaPhi", aLayer, aLayer), instanceName);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dChipStave", aLayer, aLayer), instanceName);
      for (int aStave = 0; aStave < NStaves[aLayer]; aStave++)
      {
        for (int iChip = 0; iChip < 9; iChip++)
        {
          cl->registerHisto(Form("MVTXMON_chipHitmapL%dS%dC%d", aLayer, aStave, iChip), instanceName);
        }
      }
    }

    // fhr
    cl->registerHisto("MVTXMON_General_ErrorVsFeeid", instanceName);
    cl->registerHisto("MVTXMON_General_General_Occupancy", instanceName);
    cl->registerHisto("MVTXMON_General_Noisy_Pixel", instanceName);

    for (int mLayer = 0; mLayer < 3; mLayer++)
    {
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dDeadChipPos", mLayer, mLayer), instanceName);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dAliveChipPos", mLayer, mLayer), instanceName);
      // cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStaveC", mLayer, mLayer), instanceName);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%dOccupancy_LOG", mLayer), instanceName);
    }

    cl->registerHisto("MVTXMON_Occupancy_TotalDeadChipPos", instanceName);
    cl->registerHisto("MVTXMON_Occupancy_TotalAliveChipPos", instanceName);
  }

  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  for (int server = 0; server < 6; server++)
  {
    cl->requestHistoBySubSystem("MVTXMON_" + to_string(server), 1);
  }
  OnlMonDraw *mvtxmon = new MvtxMonDraw("MVTXMONDRAW");  // create Drawing Object
  cl->registerDrawer(mvtxmon);                           // register with client framework
}

void mvtxDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  for (int server = 0; server < 6; server++)
  {
    cl->requestHistoBySubSystem("MVTXMON_" + to_string(server), 1);
  }
  cl->Draw("MVTXMONDRAW", what);  // Draw Histos of registered Drawers
}

void mvtxPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MVTXMONDRAW");                    // Create PS files
  return;
}

void mvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MVTXMONDRAW");                  // Create html output
  return;
}
