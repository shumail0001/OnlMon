#include "MvtxMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2Poly.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>
#include <TStyle.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

MvtxMonDraw::MvtxMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars = new OnlMonDB(ThisName);
  return;
}

int MvtxMonDraw::Init()
{
  return 0;
}

int MvtxMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "MvtxMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "MvtxMon Example Monitor", -1, 0, xsize / 2, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("mvtxpad1", "who needs this?", 0.1, 0.7, 0.9, 0.9, 0);
    Pad[1] = new TPad("mvtxpad2", "who needs this?", 0.1, 0.3, 0.9, 0.7, 0);
    Pad[2] = new TPad("mvtxpad3", "who needs this?", 0.1, 0.05, 0.9, 0.3, 0);
    Pad[0]->Draw();
    Pad[1]->Draw();
      Pad[2]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "MvtxMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "MvtxMon2 Example Monitor", -xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[3] = new TPad("mvtxpad4", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[4] = new TPad("mvtxpad5", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[3]->Draw();
    Pad[4]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "MvtxMon3")
  {
    TC[2] = new TCanvas(name.c_str(), "MvtxMon3 Example Monitor", xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[5] = new TPad("mvtxpad6", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[6] = new TPad("mvtxpad7", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[5]->Draw();
    Pad[6]->Draw();
    // this one is used to plot the run number on the canvas
    //        transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    //        transparent[2]->SetFillStyle(4000);
    //        transparent[2]->Draw();
    //      TC[2]->SetEditable(0);
  }
  else if (name == "MvtxMon_HitMap")
  {
    TC[3] = new TCanvas(name.c_str(), "MvtxMon Example Monitor", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    //TC[3]->Divide(2,1/*NSTAVE*/);
    TC[3]->Divide(NCHIP,9/*NSTAVE*/);
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[3]->SetEditable(false);
    TC[3]->SetTopMargin(0.05);
    TC[3]->SetBottomMargin(0.05);
  }
  else if (name == "MvtxMon_General")
  {
    TC[4] = new TCanvas(name.c_str(), "MVTX Monitoring General", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    TC[4]->cd();
    Pad[7] = new TPad("mvtxpad9", "who needs this?", 0., 0.02, 0.98, 0.95, 0);  
    Pad[7]->Draw();
    Pad[7]->Divide(4,2);  
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[4]->SetEditable(false);
    TC[4]->SetTopMargin(0.05);
    TC[4]->SetBottomMargin(0.05);
//Pad[8]->SetRightMargin(0.13);
  }
  else if (name == "MvtxMon_FEE")
  {
    TC[5] = new TCanvas(name.c_str(), "MVTX Monitoring FEE", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    TC[5]->Divide(5,3);
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[5]->SetEditable(false);
    TC[5]->SetTopMargin(0.05);
    TC[5]->SetBottomMargin(0.05);
  }
  else if (name == "MvtxMon_OCC")
  {
    TC[6] = new TCanvas(name.c_str(), "MVTX Monitoring Occupancy", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    TC[6]->Divide(3,3);
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[6]->SetEditable(false);
    TC[6]->SetTopMargin(0.05);
    TC[6]->SetBottomMargin(0.05);
  }
  else if (name == "MvtxMon_FHR")
  {
    TC[7] = new TCanvas(name.c_str(), "MVTX Monitoring FHR", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    TC[7]->Divide(5,4);
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[7]->SetEditable(false);
    TC[7]->SetTopMargin(0.05);
    TC[7]->SetBottomMargin(0.05);
  }
  return 0;
}

int MvtxMonDraw::Draw(const std::string &what)
{
  int iret = 0;
  int idraw = 0;
  if (what == "ALL" || what == "FIRST")
  {
    iret += DrawFirst(what);
    idraw++;
  }
  if (what == "ALL" || what == "SECOND")
  {
    iret += DrawSecond(what);
    idraw++;
  }
  if (what == "ALL" || what == "L0L"|| what == "L0R"|| what == "L1L"|| what == "L1R"|| what == "L2L"|| what == "L2R")
  {
    iret += DrawHitMap(what);
    idraw++;
  }
  if (what == "ALL" || what == "GENERAL")
  {
    iret += DrawGeneral(what);
    idraw++;
  }
  if (what == "ALL" || what == "FEE")
  {
    iret += DrawFEE(what);
    idraw++;
  }
  if (what == "ALL" || what == "OCC")
  {
    iret += DrawOCC(what);
    idraw++;
  }
  if (what == "ALL" || what == "FHR")
  {
    iret += DrawFHR(what);
    idraw++;
  }
  if (what == "ALL" || what == "HISTORY")
  {
    iret += DrawHistory(what);
    idraw++;
  }
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int MvtxMonDraw::DrawFirst(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *mvtxmon_ChipStaveOcc = dynamic_cast<TH2*>(cl->getHisto("MVTXMON_0","MVTXMON/RawData/mvtxmon_ChipStaveOcc"));
  TH1 *mvtxmon_ChipStave1D = cl->getHisto("MVTXMON_0","MVTXMON/RawData/mvtxmon_ChipStave1D");
  TH1 *mvtxmon_ChipFiredHis = cl->getHisto("MVTXMON_0","MVTXMON/RawData/mvtxmon_ChipFiredHis");

  
  if (!gROOT->FindObject("MvtxMon1"))
  {
    MakeCanvas("MvtxMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  
  PublishHistogram<TH2*>(Pad[0],mvtxmon_ChipStaveOcc);
  PublishHistogram<TH1*>(Pad[1],mvtxmon_ChipStave1D);
  PublishHistogram<TH1*>(Pad[2],mvtxmon_ChipFiredHis);

  PublishStatistics(TC[0],cl);
  TC[0]->SetEditable(false);
  return 0;
}

int MvtxMonDraw::DrawSecond(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *mvtxmon_EvtHitChip = cl->getHisto("MVTXMON_0","MVTXMON/RawData/mvtxmon_EvtHitChip");
  TH1 *mvtxmon_EvtHitDis = cl->getHisto("MVTXMON_0","MVTXMON/RawData/mvtxmon_EvtHitDis");

  if (!gROOT->FindObject("MvtxMon2"))
  {
    MakeCanvas("MvtxMon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");

  PublishHistogram<TH1*>(Pad[3],mvtxmon_EvtHitChip);
  PublishHistogram<TH1*>(Pad[4],mvtxmon_EvtHitDis);

  PublishStatistics(TC[1],cl);
  TC[1]->SetEditable(false);
  return 0;
}

int MvtxMonDraw::DrawHitMap(const std::string &what )
{
  OnlMonClient *cl = OnlMonClient::instance();

  const int canvasID = 3;
  TH2 *mvtxmon_HitMap[NSTAVE][NCHIP][NFlx] = {nullptr};
  if (!gROOT->FindObject("MvtxMon_HitMap"))
  {
    MakeCanvas("MvtxMon_HitMap");
  }
  TC[canvasID]->SetEditable(true);
  TC[canvasID]->Clear("D");

  int aLs = 0;
  int aLe = 0;
  int aSs = 0;
  int aSe = 0;
  if (what == "L0R" || what == "ALL"){
    aLs = 0; aLe = 1;
    aSs = 0; aSe = 6;
  }
  else if (what == "L0L"){
    aLs = 0; aLe = 1;
    aSs = 6; aSe = 12;
  }
  else if (what == "L1R"){
    aLs = 1; aLe = 2;
    aSs = 0; aSe = 8;
  }
  else if (what == "L1L"){
    aLs = 1; aLe = 2;
    aSs = 8; aSe = 16;
  }
  else if (what == "L2R"){
    aLs = 2; aLe = 3;
    aSs = 0; aSe = 10;
  }
  else if (what == "L2L"){
    aLs = 2; aLe = 3;
    aSs = 10; aSe = 20;
  }
  
  int ipad = 0;
  int returnCode = 0;
  for (int aLayer = aLs; aLayer < aLe; aLayer++) {
    for (int aStave = aSs; aStave < aSe; aStave++) {
      for (int iChip = 0; iChip < 9; iChip++) {
        int stave = aLayer==0?aStave:NStaves[aLayer]+aStave;
	for (int iFelix = 0; iFelix <NFlx; iFelix++){
          mvtxmon_HitMap[stave][iChip][iFelix] = dynamic_cast<TH2*>(cl->getHisto(Form("MVTXMON/chipHitmapL%dS%dC%d", aLayer, aStave, iChip)));
	  mvtxmon_HitMap[stave][iChip][iFelix]->GetXaxis()->CenterTitle();
	  mvtxmon_HitMap[stave][iChip][iFelix]->GetYaxis()->CenterTitle();
	  mvtxmon_HitMap[stave][iChip][iFelix]->GetYaxis()->SetTitleOffset(1.4);
          if(iFelix > 0) mvtxmon_HitMap[stave][iChip][0]->Add(mvtxmon_HitMap[stave][iChip][iFelix]); //merge all felix servers into 1
        }
        returnCode += PublishHistogram<TH2*>(TC[canvasID],ipad*9+iChip+1,mvtxmon_HitMap[stave][iChip][0],"colz"); //publish merged one
      }
      ipad++;
    }
  }

  PublishStatistics(TC[canvasID],cl);
  TC[canvasID]->SetEditable(false);
  return returnCode < 0 ? -1 : 0;
}

int MvtxMonDraw::DrawGeneral(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  const int canvasID = 4;

  TH2Poly* mvtxmon_LaneStatusOverview[3][NFlx] = {nullptr};  
  TH2Poly* mvtxmon_mGeneralOccupancy[NFlx] = {nullptr}; 
  TH1D* mvtxmon_mGeneralErrorPlots[NFlx]  = {nullptr}; 
  TH2D* mvtxmon_mGeneralErrorFile[NFlx]  = {nullptr}; 

  for (int iFelix = 0; iFelix <1; iFelix++){
    mvtxmon_LaneStatusOverview[0][iFelix] = dynamic_cast<TH2Poly*>(cl->getHisto("MVTXMON/LaneStatus/laneStatusOverviewFlagWARNING"));
    mvtxmon_LaneStatusOverview[1][iFelix] = dynamic_cast<TH2Poly*>(cl->getHisto("MVTXMON/LaneStatus/laneStatusOverviewFlagERROR"));
    mvtxmon_LaneStatusOverview[2][iFelix] = dynamic_cast<TH2Poly*>(cl->getHisto("MVTXMON/LaneStatus/laneStatusOverviewFlagFAULT"));
    mvtxmon_mGeneralOccupancy[iFelix] = dynamic_cast<TH2Poly*>(cl->getHisto("MVTXMON/General/General_Occupancy"));
    mvtxmon_mGeneralErrorPlots[iFelix] = dynamic_cast<TH1D*>(cl->getHisto("MVTXMON/General/ErrorPlots"));
    mvtxmon_mGeneralErrorFile[iFelix] = dynamic_cast<TH2D*>(cl->getHisto("MVTXMON/General/ErrorFile"));
    for(int i = 0; i < 3; i++){
      if(iFelix > 0) mvtxmon_LaneStatusOverview[i][0]->Add(mvtxmon_LaneStatusOverview[i][iFelix],1.); //merge all felix servers into 1
    }
    if(iFelix > 0) mvtxmon_mGeneralOccupancy[0]->Add(mvtxmon_mGeneralOccupancy[iFelix],1.);
    if(iFelix > 0) mvtxmon_mGeneralErrorPlots[0]->Add(mvtxmon_mGeneralErrorPlots[iFelix]);
    if(iFelix > 0) mvtxmon_mGeneralErrorFile[0]->Add(mvtxmon_mGeneralErrorFile[iFelix]);
  }

  for(int i = 0; i < 3; i++){
    mvtxmon_LaneStatusOverview[i][0]->SetStats(0);
    mvtxmon_LaneStatusOverview[i][0]->SetMinimum(0);
    mvtxmon_LaneStatusOverview[i][0]->SetMaximum(1);
    mvtxmon_LaneStatusOverview[i][0]->SetBit(TH1::kIsAverage);
  }


  if (!gROOT->FindObject("MvtxMon_General"))
  {
    MakeCanvas("MvtxMon_General");
  }

  TC[canvasID]->SetEditable(true);
  TC[canvasID]->Clear("D");
  
  int returnCode = 0;

Pad[7]->Divide(4,2);
//TC[canvasID]->ls();
//TC[canvasID]->Print();
Pad[7]->ls();
Pad[7]->Print();
//TC[canvasID]->cd(1);
  //Pad[7]->cd(2);
//mvtxmon_LaneStatusOverview[0][0]->DrawCopy("lcolz");
  returnCode += PublishHistogram<TH2Poly*>(Pad[7],1,mvtxmon_LaneStatusOverview[0][0],"lcolz");
  returnCode += PublishHistogram<TH2Poly*>(Pad[7],2,mvtxmon_LaneStatusOverview[1][0],"lcolz");
  returnCode += PublishHistogram<TH2Poly*>(Pad[7],3,mvtxmon_LaneStatusOverview[2][0],"lcolz");
  returnCode += PublishHistogram<TH2Poly*>(Pad[7],4,mvtxmon_mGeneralOccupancy[0]);
  returnCode += PublishHistogram<TH2Poly*>(Pad[7],4,mvtxmon_mGeneralOccupancy[0],"colz same");
  returnCode += PublishHistogram<TH1D*>(Pad[7],5,mvtxmon_mGeneralErrorPlots[0]);
  returnCode += PublishHistogram<TH2D*>(Pad[7],6,mvtxmon_mGeneralErrorFile[0]);


  //mvtxmon_mGeneralOccupancy[0]->SetRightMargin(0.07);
/*mvtxmon_mGeneralOccupancy[0]->GetYaxis()->SetTitleOffset(0.6);
  PublishHistogram<TH2Poly*>(Pad[8],mvtxmon_mGeneralOccupancy[0]);
  PublishHistogram<TH2Poly*>(Pad[8],mvtxmon_mGeneralOccupancy[0],"colz same");*/

 /* TC[canvasID]->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  Pad[7]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasID]->Update();
  TC[canvasID]->Show();*/
  
  PublishStatistics(TC[canvasID],cl);
  TC[canvasID]->SetEditable(false);
  return returnCode < 0 ? -1 : 0;
}

int MvtxMonDraw::DrawFEE(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  const int canvasID = 5;

  TH2I* mTriggerVsFeeId[NFlx] = {nullptr}; 
  TH1I* mTrigger[NFlx] ={nullptr}; 
  TH2I* mLaneInfo[NFlx] = {nullptr}; 
  TH2I* mLaneStatus[3][NFlx] = {nullptr}; 
  TH2I* mLaneStatusCumulative[3][NFlx] = {nullptr}; 
  TH1I* mLaneStatusSummary[3][NFlx] = {nullptr}; 
  TH1I* mLaneStatusSummaryIB[NFlx] = {nullptr}; 

  for (int iFelix = 0; iFelix <NFlx; iFelix++){
    mTriggerVsFeeId[iFelix] = dynamic_cast<TH2I*>(cl->getHisto("MVTXMON/FEE/TriggerVsFeeid"));
    mTrigger[iFelix] = dynamic_cast<TH1I*>(cl->getHisto("MVTXMON/FEE/TriggerFlag"));
    mLaneInfo[iFelix] = dynamic_cast<TH2I*>(cl->getHisto("MVTXMON/FEE/LaneInfo"));
    mLaneStatusSummaryIB[iFelix]= dynamic_cast<TH1I*>(cl->getHisto("MVTXMON/LaneStatusSummary/LaneStatusSummaryIB"));
    for (int i = 0; i < 3; i++) {
      mLaneStatus[i][iFelix] = dynamic_cast<TH2I*>(cl->getHisto(Form("MVTXMON/LaneStatus/laneStatusFlag%s", mLaneStatusFlag[i].c_str())));
      mLaneStatusCumulative[i][iFelix] = dynamic_cast<TH2I*>(cl->getHisto(Form("MVTXMON/LaneStatus/laneStatusFlagCumulative%s", mLaneStatusFlag[i].c_str())));
      mLaneStatusSummary[i][iFelix] = dynamic_cast<TH1I*>(cl->getHisto(Form("MVTXMON/LaneStatusSummary/LaneStatusSummaryL%i", i)));
     if(iFelix > 0) mLaneStatus[i][0]->Add(mLaneStatus[i][iFelix]); //merge all felix servers into 1
     if(iFelix > 0) mLaneStatusCumulative[i][0]->Add(mLaneStatusCumulative[i][iFelix]); //merge all felix servers into 1
     if(iFelix > 0) mLaneStatusSummary[i][0]->Add(mLaneStatusSummary[i][iFelix]); //merge all felix servers into 1
    }
    if(iFelix > 0) mTriggerVsFeeId[0]->Add(mTriggerVsFeeId[iFelix]);
    if(iFelix > 0) mTrigger[0]->Add(mTrigger[iFelix]);
    if(iFelix > 0) mLaneInfo[0]->Add(mLaneInfo[iFelix]);
    if(iFelix > 0) mLaneStatusSummaryIB[0]->Add(mLaneStatusSummaryIB[iFelix]);
  }

  if (!gROOT->FindObject("MvtxMon_FEE"))
  {
    MakeCanvas("MvtxMon_FEE");
  }

  TC[canvasID]->SetEditable(true);
  TC[canvasID]->Clear("D");

  int returnCode = 0;
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],1,mTriggerVsFeeId[0],"lcol");
  returnCode += PublishHistogram<TH1I*>(TC[canvasID],2,mTrigger[0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],3,mLaneInfo[0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],4,mLaneStatus[0][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],5,mLaneStatus[1][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],6,mLaneStatus[2][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],7,mLaneStatusCumulative[0][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],8,mLaneStatusCumulative[1][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],9,mLaneStatusCumulative[2][0]);
  returnCode += PublishHistogram<TH1I*>(TC[canvasID],10,mLaneStatusSummary[0][0]);
  returnCode += PublishHistogram<TH1I*>(TC[canvasID],11,mLaneStatusSummary[1][0]);
  returnCode += PublishHistogram<TH1I*>(TC[canvasID],12,mLaneStatusSummary[2][0]);
  returnCode += PublishHistogram<TH1I*>(TC[canvasID],13,mLaneStatusSummaryIB[0]);
  
  PublishStatistics(TC[canvasID],cl);
  TC[canvasID]->SetEditable(false);
  return returnCode < 0 ? -1 : 0;
}

int MvtxMonDraw::DrawOCC(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  const int canvasID = 6;
  TH1D* hOccupancyPlot[3][NFlx];
  TH2I* hEtaPhiHitmap[3][NFlx];
  TH2D* hChipStaveOccupancy[3][NFlx];

  for (int aLayer = 0; aLayer < 3; aLayer++) {
    for (int iFelix = 0; iFelix <NFlx; iFelix++){
      hOccupancyPlot[aLayer][iFelix] = dynamic_cast<TH1D*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%dOccupancy", aLayer)));
      hEtaPhiHitmap[aLayer][iFelix] = dynamic_cast<TH2I*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dEtaPhi", aLayer, aLayer)));
      hChipStaveOccupancy[aLayer][iFelix] = dynamic_cast<TH2D*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStave", aLayer, aLayer)));
      if(iFelix > 0) hOccupancyPlot[aLayer][0]->Add(hOccupancyPlot[aLayer][iFelix]); //merge all felix servers into 1
      if(iFelix > 0) hEtaPhiHitmap[aLayer][0]->Add(hEtaPhiHitmap[aLayer][iFelix]); //merge all felix servers into 1
      if(iFelix > 0) hChipStaveOccupancy[aLayer][0]->Add(hChipStaveOccupancy[aLayer][iFelix]); //merge all felix servers into 1
    }
  }

  if (!gROOT->FindObject("MvtxMon_OCC"))
  {
    MakeCanvas("MvtxMon_OCC");
  }

  TC[canvasID]->SetEditable(true);
  TC[canvasID]->Clear("D");

  int returnCode = 0;
  returnCode += PublishHistogram<TH1D*>(TC[canvasID],1,hOccupancyPlot[0][0]);
  returnCode += PublishHistogram<TH1D*>(TC[canvasID],2,hOccupancyPlot[1][0]);
  returnCode += PublishHistogram<TH1D*>(TC[canvasID],3,hOccupancyPlot[2][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],4,hEtaPhiHitmap[0][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],5,hEtaPhiHitmap[1][0]);
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],6,hEtaPhiHitmap[2][0]);
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],7,hChipStaveOccupancy[0][0],"colz");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],8,hChipStaveOccupancy[1][0],"colz");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],9,hChipStaveOccupancy[2][0],"colz");

  PublishStatistics(TC[canvasID],cl);
  TC[canvasID]->SetEditable(false);
  return returnCode < 0 ? -1 : 0;
}

int MvtxMonDraw::DrawFHR(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  const int canvasID = 7;

  TH2D* mDeadChipPos[3][NFlx];
  TH2D* mAliveChipPos[3][NFlx];
  //TH2D* mChipStaveOccupancy[3][NFlx];
  TH1D* mOccupancyPlot[3][NFlx];
  
  TH2I* mErrorVsFeeid[NFlx] = {nullptr}; 
  TH2Poly* mGeneralOccupancy[NFlx] = {nullptr}; 
  TH2Poly* mGeneralNoisyPixel[NFlx] = {nullptr}; 
  TH2D* mTotalDeadChipPos[NFlx] = {nullptr}; 
  TH2D* mTotalAliveChipPos[NFlx] = {nullptr}; 

  for (int iFelix = 0; iFelix <NFlx; iFelix++){
    mErrorVsFeeid[iFelix] =  dynamic_cast<TH2I*>(cl->getHisto("MVTXMON/General/ErrorVsFeeid"));
    mGeneralOccupancy[iFelix] = dynamic_cast<TH2Poly*>(cl->getHisto("MVTXMON/General/General_Occupancy"));
    mGeneralNoisyPixel[iFelix] = dynamic_cast<TH2Poly*>(cl->getHisto("MVTXMON/General/Noisy_Pixel"));
    mTotalDeadChipPos[iFelix] = dynamic_cast<TH2D*>(cl->getHisto("MVTXMON/Occupancy/TotalDeadChipPos"));
    mTotalAliveChipPos[iFelix] = dynamic_cast<TH2D*>(cl->getHisto("MVTXMON/Occupancy/TotalAliveChipPos"));
    for (int mLayer = 0; mLayer < 3; mLayer++) {
      mDeadChipPos[mLayer][iFelix] =  dynamic_cast<TH2D*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dDeadChipPos", mLayer, mLayer)));
      mAliveChipPos[mLayer][iFelix] =  dynamic_cast<TH2D*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dAliveChipPos", mLayer, mLayer)));
     // mChipStaveOccupancy[mLayer][iFelix] =  dynamic_cast<TH2D*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStaveC", mLayer, mLayer)));
      mOccupancyPlot[mLayer][iFelix] =  dynamic_cast<TH1D*>(cl->getHisto(Form("MVTXMON/Occupancy/Layer%dOccupancy/LOG", mLayer)));
     if(iFelix > 0) mDeadChipPos[mLayer][0]->Add(mDeadChipPos[mLayer][iFelix]);
     if(iFelix > 0) mAliveChipPos[mLayer][0]->Add(mAliveChipPos[mLayer][iFelix]);
   //  if(iFelix > 0) mChipStaveOccupancy[mLayer][0]->Add(mChipStaveOccupancy[mLayer][iFelix]);
     if(iFelix > 0) mOccupancyPlot[mLayer][0]->Add(mOccupancyPlot[mLayer][iFelix]);
    }
    if(iFelix > 0) mErrorVsFeeid[0]->Add(mErrorVsFeeid[iFelix]);
    if(iFelix > 0) mGeneralOccupancy[0]->Add(mGeneralOccupancy[iFelix],1.);
    if(iFelix > 0) mGeneralNoisyPixel[0]->Add(mGeneralNoisyPixel[iFelix],1.);
    if(iFelix > 0) mTotalDeadChipPos[0]->Add(mTotalDeadChipPos[iFelix]);
    if(iFelix > 0) mTotalAliveChipPos[0]->Add(mTotalAliveChipPos[iFelix]);
  }



  if (!gROOT->FindObject("MvtxMon_FHR"))
  {
    MakeCanvas("MvtxMon_FHR");
  }

  TC[canvasID]->SetEditable(true);
  TC[canvasID]->Clear("D");

  int returnCode = 0;
  returnCode += PublishHistogram<TH2I*>(TC[canvasID],1,mErrorVsFeeid[0]);
  returnCode += PublishHistogram<TH2Poly*>(TC[canvasID],2,mGeneralOccupancy[0]);
  returnCode += PublishHistogram<TH2Poly*>(TC[canvasID],3,mGeneralNoisyPixel[0]);
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],4,mDeadChipPos[0][0],"lcol");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],5,mDeadChipPos[1][0],"lcol");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],6,mDeadChipPos[2][0],"lcol");
  gStyle->SetPalette(85);
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],7,mAliveChipPos[0][0],"lcol");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],8,mAliveChipPos[1][0],"lcol");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],9,mAliveChipPos[2][0],"lcol");
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],17,mTotalAliveChipPos[0],"lcol");
  gStyle->SetPalette(55);
 // returnCode += PublishHistogram<TH2D*>(TC[canvasID],10,mChipStaveOccupancy[0][0]);
 // returnCode += PublishHistogram<TH2D*>(TC[canvasID],11,mChipStaveOccupancy[1][0]);
//  returnCode += PublishHistogram<TH2D*>(TC[canvasID],12,mChipStaveOccupancy[2][0]);
  returnCode += PublishHistogram<TH1D*>(TC[canvasID],13,mOccupancyPlot[0][0]);
  returnCode += PublishHistogram<TH1D*>(TC[canvasID],14,mOccupancyPlot[1][0]);
  returnCode += PublishHistogram<TH1D*>(TC[canvasID],15,mOccupancyPlot[2][0]);
  returnCode += PublishHistogram<TH2D*>(TC[canvasID],16,mTotalDeadChipPos[0],"lcol");

  
  PublishStatistics(TC[canvasID],cl);
  TC[canvasID]->SetEditable(false);
  return returnCode < 0 ? -1 : 0;
}

int MvtxMonDraw::MakePS(const std::string &what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  std::ostringstream filename;
  int iret = Draw(what);
  if (iret)  // on error no ps files please
  {
    return iret;
  }
  filename << ThisName << "_1_" << cl->RunNumber() << ".ps";
  TC[0]->Print(filename.str().c_str());
  filename.str("");
  filename << ThisName << "_2_" << cl->RunNumber() << ".ps";
  TC[1]->Print(filename.str().c_str());
  return 0;
}

int MvtxMonDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient *cl = OnlMonClient::instance();

  // Register the 1st canvas png file to the menu and produces the png file.
  std::string pngfile = cl->htmlRegisterPage(*this, "First Canvas", "1", "png");
  cl->CanvasToPng(TC[0], pngfile);

  // idem for 2nd canvas.
  pngfile = cl->htmlRegisterPage(*this, "Second Canvas", "2", "png");
  cl->CanvasToPng(TC[1], pngfile);
  // Now register also EXPERTS html pages, under the EXPERTS subfolder.

  std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
  std::ofstream out(logfile.c_str());
  out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
      << "</TITLE></HEAD>" << std::endl;
  out << "<P>Some log file output would go here." << std::endl;
  out.close();

  std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
  std::ofstream out2(status.c_str());
  out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
       << "</TITLE></HEAD>" << std::endl;
  out2 << "<P>Some status output would go here." << std::endl;
  out2.close();
  cl->SaveLogFile(*this);
  return 0;
}

int MvtxMonDraw::DrawHistory(const std::string & /* what */)
{
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  std::vector<float> var;
  std::vector<float> varerr;
  std::vector<time_t> timestamp;
  std::vector<int> runnumber;
  std::string varname = "n_events";
  // this sets the time range from whihc values should be returned
  time_t begin = 0;            // begin of time (1.1.1970)
  time_t end = time(nullptr);  // current time (right NOW)
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << __PRETTY_FUNCTION__ << " Error in db access" << std::endl;
    return iret;
  }
  if (!gROOT->FindObject("MvtxMon3"))
  {
    MakeCanvas("MvtxMon3");
  }
  // timestamps come sorted in ascending order
  float *x = new float[var.size()];
  float *y = new float[var.size()];
  float *ex = new float[var.size()];
  float *ey = new float[var.size()];
  int n = var.size();
  for (unsigned int i = 0; i < var.size(); i++)
  {
    //       std::cout << "timestamp: " << ctime(&timestamp[i])
    // 	   << ", run: " << runnumber[i]
    // 	   << ", var: " << var[i]
    // 	   << ", varerr: " << varerr[i]
    // 	   << std::endl;
    x[i] = timestamp[i] - TimeOffsetTicks;
    y[i] = var[i];
    ex[i] = 0;
    ey[i] = varerr[i];
  }
  Pad[5]->cd();
  if (gr[0])
  {
    delete gr[0];
  }
  gr[0] = new TGraphErrors(n, x, y, ex, ey);
  gr[0]->SetMarkerColor(4);
  gr[0]->SetMarkerStyle(21);
  gr[0]->Draw("ALP");
  gr[0]->GetXaxis()->SetTimeDisplay(1);
  gr[0]->GetXaxis()->SetLabelSize(0.03);
  // the x axis labeling looks like crap
  // please help me with this, the SetNdivisions
  // don't do the trick
  gr[0]->GetXaxis()->SetNdivisions(-1006);
  gr[0]->GetXaxis()->SetTimeOffset(TimeOffsetTicks);
  gr[0]->GetXaxis()->SetTimeFormat("%Y/%m/%d %H:%M");
  delete[] x;
  delete[] y;
  delete[] ex;
  delete[] ey;

  varname = "mvtxmoncount";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << __PRETTY_FUNCTION__ << " Error in db access" << std::endl;
    return iret;
  }
  x = new float[var.size()];
  y = new float[var.size()];
  ex = new float[var.size()];
  ey = new float[var.size()];
  n = var.size();
  for (unsigned int i = 0; i < var.size(); i++)
  {
    //       std::cout << "timestamp: " << ctime(&timestamp[i])
    // 	   << ", run: " << runnumber[i]
    // 	   << ", var: " << var[i]
    // 	   << ", varerr: " << varerr[i]
    // 	   << std::endl;
    x[i] = timestamp[i] - TimeOffsetTicks;
    y[i] = var[i];
    ex[i] = 0;
    ey[i] = varerr[i];
  }
  Pad[6]->cd();
  if (gr[1])
  {
    delete gr[1];
  }
  gr[1] = new TGraphErrors(n, x, y, ex, ey);
  gr[1]->SetMarkerColor(4);
  gr[1]->SetMarkerStyle(21);
  gr[1]->Draw("ALP");
  gr[1]->GetXaxis()->SetTimeDisplay(1);
  // TC[2]->Update();
  //    h1->GetXaxis()->SetTimeDisplay(1);
  //    h1->GetXaxis()->SetLabelSize(0.03);
  gr[1]->GetXaxis()->SetLabelSize(0.03);
  gr[1]->GetXaxis()->SetTimeOffset(TimeOffsetTicks);
  gr[1]->GetXaxis()->SetTimeFormat("%Y/%m/%d %H:%M");
  //    h1->Draw();
  delete[] x;
  delete[] y;
  delete[] ex;
  delete[] ey;

  TC[2]->Update();
  return 0;
}

template <typename T>
int MvtxMonDraw::PublishHistogram(TCanvas *c, int pad, T h, const char* opt ){
  if(c && pad !=0){
    c->cd(pad);
    std::cout<<"ups"<<std::endl;
  }
  if(h){  
    h->DrawCopy(opt);
    return 0;
  }
  else{
    DrawDeadServer(transparent[0]);
    return -1;
  }
}

template <typename T>
int MvtxMonDraw::PublishHistogram(TPad *p, int pad, T h, const char* opt ){
  if(p && pad !=0){
    p->cd(pad);
    TCanvas *c = nullptr;
    return PublishHistogram<T>(c,0,h,opt);
  }
  else{
    return -1;
  }
}

template <typename T>
int MvtxMonDraw::PublishHistogram(TPad *p, T h, const char* opt ){
  if(p){
    p->cd();
    TCanvas *c = nullptr;
    return PublishHistogram<T>(c,0,h,opt);
  }
  else{
    return -1;
  }
}

void MvtxMonDraw::PublishStatistics(TCanvas *c,OnlMonClient *cl){
  c->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  c->Update();
  c->Show();
}



