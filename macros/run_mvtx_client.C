#include "CommonFuncs.C"

#include <onlmon/mvtx/MvtxMonDraw.h>

#include <onlmon/OnlMonClient.h>

//const std::string DrawerName = "MVTXMONDRAW";

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_client.so)

void mvtxDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  OnlMonDraw *mvtxmon = new MvtxMonDraw("MVTXMONDRAW");  // create Drawing Object
  // register histos we want with monitor name
  std::string mLaneStatusFlag[3] = {"WARNING", "ERROR", "FAULT"};
  const int NStaves[3] = {12, 16, 20};

  for (int serverid = 0; serverid < 6; serverid++)
  {
    std::string servername = "MVTXMON_" + std::to_string(serverid);
    mvtxmon->AddServer(servername);
    cl->registerHisto("OCC_ChipStaveOcc", servername);
    cl->registerHisto("OCC_ChipStave1D", servername);
    cl->registerHisto("MVTXMON_RawData_ChipFiredHis", servername);
    cl->registerHisto("MVTXMON_RawData_EvtHitChip", servername);
    cl->registerHisto("MVTXMON_RawData_EvtHitDis", servername);
    cl->registerHisto("MVTXMON_General_Occupancy", servername);
    cl->registerHisto("MVTXMON_LaneStatus_laneStatusOverviewFlagWARNING", servername);
    cl->registerHisto("MVTXMON_LaneStatus_laneStatusOverviewFlagERROR", servername);
    cl->registerHisto("MVTXMON_LaneStatus_laneStatusOverviewFlagFAULT", servername);

    cl->registerHisto("hChipStrobes", servername);
    cl->registerHisto("hChipL1", servername);

    // FEE
    cl->registerHisto("MVTXMON_FEE_TriggerVsFeeid", servername);
    cl->registerHisto("MVTXMON_FEE_TriggerFlag", servername);
    // cl->registerHisto("MVTXMON/FEE/LaneInfo", servername);

    for (int i = 0; i < MvtxMonDraw::NFlags; i++)
    {
      cl->registerHisto(Form("MVTXMON_LaneStatus_laneStatusFlag%s", mLaneStatusFlag[i].c_str()), servername);
      cl->registerHisto(Form("MVTXMON_LaneStatus_laneStatusFlagCumulative%s", mLaneStatusFlag[i].c_str()), servername);
    }

    for (int i = 0; i < 3; i++)
    {
      cl->registerHisto(Form("MVTXMON_LaneStatusSummary_LaneStatusSummaryL%i", i), servername);
    }

    cl->registerHisto("MVTXMON_LaneStatusSummary_LaneStatusSummary", servername);

    // raw task
    cl->registerHisto("MVTXMON_General_ErrorPlots", servername);
    cl->registerHisto("MVTXMON_General_ErrorFile", servername);

    for (int aLayer = 0; aLayer < 3; aLayer++)
    {
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%dOccupancy", aLayer), servername);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dChipStave", aLayer, aLayer), servername);
      cl->registerHisto(Form("MVTXMON_Noisy_Layer%d_ChipStave", aLayer), servername);
    }

    // fhr
    cl->registerHisto("MVTXMON_General_ErrorVsFeeid", servername);
    // cl->registerHisto("MVTXMON_General_Occupancy", servername);
    cl->registerHisto("MVTXMON_General_Noisy_Pixel", servername);
    cl->registerHisto("RCDAQ_evt", servername);

    for (int mLayer = 0; mLayer < 3; mLayer++)
    {
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dDeadChipPos", mLayer, mLayer), servername);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%d_Layer%dAliveChipPos", mLayer, mLayer), servername);
      // cl->registerHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStaveC", mLayer, mLayer), servername);
      cl->registerHisto(Form("MVTXMON_Occupancy_Layer%dOccupancy_LOG", mLayer), servername);
    }

    cl->registerHisto("MVTXMON_Occupancy_TotalDeadChipPos", servername);
    cl->registerHisto("MVTXMON_Occupancy_TotalAliveChipPos", servername);
  }

  // for local host, just call mvtxDrawInit(2)
  CreateSubsysHostlist("mvtx_hosts.list", online);
  // says I know they are all on the same node
  for (auto iter = mvtxmon->ServerBegin(); iter != mvtxmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }

  cl->registerDrawer(mvtxmon);  // register with client framework
}

void mvtxDraw(const char *what = "ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();         // get pointer to framewrk
  OnlMonDraw *mvtxmon = cl->GetDrawer("MVTXMONDRAW");  // get pointer to this drawer
  for (auto iter = mvtxmon->ServerBegin(); iter != mvtxmon->ServerEnd(); ++iter)
  {
    cl->requestHistoBySubSystem(iter->c_str(), 1);
  }
  cl->Draw("MVTXMONDRAW", what);  // Draw Histos of registered Drawers
}

void mvtxSavePlot()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->SavePlot("MVTXMONDRAW");                  // Save Plots
  return;
}

void mvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MVTXMONDRAW");                  // Create html output
  return;
}
