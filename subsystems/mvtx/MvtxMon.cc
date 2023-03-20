// use #include "" only for your local include and put
// those in the first line(s) before any #include <>
// otherwise you are asking for weird behavior
// (more info - check the difference in include path search when using "" versus <>)

#include "MvtxMon.h"

#include <onlmon/OnlMon.h>  // for OnlMon
#include <onlmon/OnlMonDB.h>
#include <onlmon/OnlMonServer.h>

#include <Event/msg_profile.h>

#include <TH1.h>
#include <TString.h>
#include <TList.h>
#include <TLatex.h>
#include <TH2.h>
#include <TH2Poly.h>
#include <TLine.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <cmath>
#include <cstdio>  // for printf
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>  // for allocator, string, char_traits
#include <utility>


enum
{
  TRGMESSAGE = 1,
  FILLMESSAGE = 2
};

MvtxMon::MvtxMon(const std::string &name)
  : OnlMon(name)
{
  // leave ctor fairly empty, its hard to debug if code crashes already
  // during a new MvtxMon()
  return;
}

MvtxMon::~MvtxMon()
{
  // you can delete NULL pointers it results in a NOOP (No Operation)
  delete dbvars;
  return;
}

int MvtxMon::Init()
{
  // read our calibrations from MvtxMonData.dat
  std::string fullfile = std::string(getenv("MVTXCALIB")) + "/" + "MvtxMonData.dat";
  std::ifstream calib(fullfile);
  calib.close();
  // use printf for stuff which should go the screen but not into the message
  // system (all couts are redirected)
  printf("doing the Init\n");
  OnlMonServer *se = OnlMonServer::instance();

  // register histograms with server otherwise client won't get them
  mvtxmon_ChipStaveOcc = new TH2D("MVTXMON/RawData/mvtxmon_ChipStaveOcc","Average Occupancy: Run XX Stave Number and Chip Number",NCHIP,-0.5,NCHIP-0.5,NSTAVE,-0.5,NSTAVE-0.5);
  mvtxmon_ChipStave1D = new TH1D("MVTXMON/RawData/mvtxmon_ChipStave1D","Average Occupancy: Run XX Stave Number and Chip Number",NCHIP*NSTAVE,-0.5,NCHIP * NSTAVE-0.5);
  mvtxmon_ChipFiredHis= new TH1D("MVTXMON/RawData/mvtxmon_ChipFiredHis","Number of Chips Fired in Each Event Distribution: Run XX",NCHIP*NSTAVE,-0.5,NCHIP*NSTAVE - 0.5);
  mvtxmon_EvtHitChip= new TH1D("MVTXMON/RawData/mvtxmon_EvtHitChip","Number of Hits Per Event Per Chip Distribution: Run XX",25,-0.5,24.5);
  mvtxmon_EvtHitDis = new TH1D("MVTXMON/RawData/mvtxmon_EvtHitDis","Number of Hits Per Event Distribution: Run XX",25,-0.5,24.5);

  se->registerHisto(this, mvtxmon_ChipStaveOcc);
  se->registerHisto(this, mvtxmon_ChipStave1D);
  se->registerHisto(this, mvtxmon_ChipFiredHis);
  se->registerHisto(this, mvtxmon_EvtHitChip);
  se->registerHisto(this, mvtxmon_EvtHitDis);

  mvtxmon_GeneralOccupancy = new TH2Poly();
  mvtxmon_GeneralOccupancy->SetTitle("General Occupancy;mm;mm");
  mvtxmon_GeneralOccupancy->SetName("MVTXMON/General/General_Occupancy/REMOVE");
  mvtxmon_GeneralOccupancy->SetStats(0);
  mvtxmon_GeneralOccupancy->SetMinimum(pow(10, mMinGeneralAxisRange));
  mvtxmon_GeneralOccupancy->SetMaximum(pow(10, mMaxGeneralAxisRange));

  createPoly(mvtxmon_GeneralOccupancy);
  se->registerHisto(this, mvtxmon_GeneralOccupancy);

  for (int i = 0; i < NFlags; i++) {
    mvtxmon_LaneStatusOverview[i] = new TH2Poly();
    mvtxmon_LaneStatusOverview[i]->SetName(Form("MVTXMON/LaneStatus/laneStatusOverviewFlag%s", mLaneStatusFlag[i].c_str()));
    TString title = Form("Fraction of lanes into %s", mLaneStatusFlag[i].c_str());
    title += ";mm (IB 3x);mm (IB 3x)";
    mvtxmon_LaneStatusOverview[i]->SetTitle(title);
    createPoly(mvtxmon_LaneStatusOverview[i]);
    se->registerHisto(this, mvtxmon_LaneStatusOverview[i]); // mLaneStatusOverview
  }

//fee
  mTriggerVsFeeId = new TH2I("MVTXMON/FEE/TriggerVsFeeid", "Trigger count vs Trigger ID and Fee ID", NFees, 0, NFees, NTrigger, 0.5, NTrigger + 0.5);
  mTriggerVsFeeId->GetXaxis()->SetTitle("FeeID");
  mTriggerVsFeeId->GetYaxis()->SetTitle("Trigger ID");
  for (int i = 0; i < NTrigger; i++) {
    mTriggerVsFeeId->GetYaxis()->SetBinLabel(i + 1, mTriggerType[i]);
  }
  se->registerHisto(this, mTriggerVsFeeId);

  mTrigger = new TH1I("MVTXMON/FEE/TriggerFlag", "Trigger vs counts", NTrigger, 0.5, NTrigger + 0.5);
  mTrigger->GetXaxis()->SetTitle("Trigger ID");
  mTrigger->GetYaxis()->SetTitle("Counts");
  for (int i = 0; i < NTrigger; i++) {
    mTrigger->GetXaxis()->SetBinLabel(i + 1, mTriggerType[i]);
  }
  se->registerHisto(this, mTrigger);

  mLaneInfo = new TH2I("MVTXMON/FEE/LaneInfo", "Lane Information", NLanesMax, -.5, NLanesMax - 0.5, NFlags, -.5, NFlags - 0.5);
  mLaneInfo->GetXaxis()->SetTitle("Lane");
  mLaneInfo->GetYaxis()->SetTitle("Flag");
  se->registerHisto(this, mLaneInfo);

  for (int i = 0; i < NFlags; i++) {
    mLaneStatus[i] = new TH2I(Form("MVTXMON/LaneStatus/laneStatusFlag%s", mLaneStatusFlag[i].c_str()), Form("Lane Status Flag: %s", mLaneStatusFlag[i].c_str()), NFees, 0, NFees, NLanesMax, 0, NLanesMax);
    mLaneStatus[i]->GetXaxis()->SetTitle("FEE ID");
    mLaneStatus[i]->GetYaxis()->SetTitle("Lane");
    drawLayerName(mLaneStatus[i]);
    se->registerHisto(this,  mLaneStatus[i]);

    mLaneStatusCumulative[i] = new TH2I(Form("MVTXMON/LaneStatus/laneStatusFlagCumulative%s", mLaneStatusFlag[i].c_str()), Form("Lane Status Flags since SOX: %s", mLaneStatusFlag[i].c_str()), NFees, 0, NFees, NLanesMax, 0, NLanesMax);
    mLaneStatusCumulative[i]->GetXaxis()->SetTitle("FEE ID");
    mLaneStatusCumulative[i]->GetYaxis()->SetTitle("Lane");
    drawLayerName(mLaneStatusCumulative[i]);
    se->registerHisto(this,  mLaneStatusCumulative[i]);
  }


  for (int i = 0; i < NLAYERS; i++) {
    mLaneStatusSummary[i] = new TH1I(Form("MVTXMON/LaneStatusSummary/LaneStatusSummaryL%i", i), Form("Lane Status Summary L%i", i), 3, 0, 3);
    mLaneStatusSummary[i]->GetYaxis()->SetTitle("#Lanes");
    for (int j = 0; j < NFlags; j++) {
      mLaneStatusSummary[i]->GetXaxis()->SetBinLabel(j + 1, mLaneStatusFlag[j].c_str());
    }
    mLaneStatusSummary[i]->GetXaxis()->CenterLabels();
    se->registerHisto(this,  mLaneStatusSummary[i]);
  }

  mLaneStatusSummaryIB = new TH1I("MVTXMON/LaneStatusSummary/LaneStatusSummaryIB", "Lane Status Summary IB", 3, 0, 3);
  mLaneStatusSummaryIB->GetYaxis()->SetTitle("#Lanes");
  for (int j = 0; j < NFlags; j++) {
    mLaneStatusSummaryIB->GetXaxis()->SetBinLabel(j + 1, mLaneStatusFlag[j].c_str());
  }
  mLaneStatusSummaryIB->GetXaxis()->CenterLabels();
  se->registerHisto(this,  mLaneStatusSummaryIB);

  //raw task
  hErrorPlots = new TH1D("MVTXMON/General/ErrorPlots", "Decoding Errors", NError, 0.5, NError + 0.5);
  hErrorPlots->GetYaxis()->SetTitle("Counts");
  hErrorPlots->GetXaxis()->SetTitle("Error ID");
  hErrorPlots->SetMinimum(0);  //remove
  hErrorPlots->SetFillColor(kRed); //remove
  for (int i = 0; i < NError; i++) {
    pt[i] = new TPaveText(0.20, 0.80 - i * 0.05, 0.85, 0.85 - i * 0.05, "NDC");
    pt[i]->SetTextSize(0.04);
    pt[i]->SetTextAlign(12);
    pt[i]->SetFillColor(0);
    pt[i]->SetTextColor(2);
    pt[i]->AddText(ErrorType[i].Data());
    hErrorPlots->GetListOfFunctions()->Add(pt[i]);
  }
  se->registerHisto(this,  hErrorPlots);

  hErrorFile = new TH2D("MVTXMON/General/ErrorFile", "Decoding Errors vs File ID", NFiles + 1, -0.5, NFiles + 0.5, NError, 0.5, NError + 0.5);
  hErrorFile->GetYaxis()->SetTitle("Error ID");
  hErrorFile->GetXaxis()->SetTitle("File ID (data-link)");
  hErrorFile->GetZaxis()->SetTitle("Counts");
  hErrorFile->SetMinimum(0);
  se->registerHisto(this,  hErrorFile);

  //put this on client side
  /*hInfoCanvas = new TH1D("General/InfoCanvas", "InfoCanvas", 3, -0.5, 2.5);
  hInfoCanvas->SetTitle("QC Process Information Canvas");
  hInfoCanvas->GetListOfFunctions()->Add(ptNEvent);
  hInfoCanvas->GetListOfFunctions()->Add(bulb);
  hInfoCanvas->GetListOfFunctions()->Add(bulbRed);
  hInfoCanvas->GetListOfFunctions()->Add(bulbYellow);
  hInfoCanvas->GetListOfFunctions()->Add(bulbGreen);
  bulb = new TEllipse(0.2, 0.75, 0.30, 0.20);
  se->registerHisto(this,  hInfoCanvas);*/

  for (int aLayer = 0; aLayer < 3; aLayer++) {
    hOccupancyPlot[aLayer] = new TH1D(Form("MVTXMON/Occupancy/Layer%dOccupancy", aLayer), Form("MVTX Layer %d Occupancy Distribution", aLayer), 301, -15, 0.05);
    hOccupancyPlot[aLayer]->GetYaxis()->SetTitle("Counts");
    hOccupancyPlot[aLayer]->GetXaxis()->SetTitle("Occupancy");
    se->registerHisto(this,  hOccupancyPlot[aLayer]);

    hEtaPhiHitmap[aLayer] = new TH2I(Form("MVTXMON/Occupancy/Layer%d/Layer%dEtaPhi", aLayer, aLayer), Form("MVTX Layer%d, Hits vs Eta and Phi", aLayer), 90, (-1) * etaCoverage[aLayer], etaCoverage[aLayer], NStaves[aLayer]*5, PhiMin, PhiMax);
    hEtaPhiHitmap[aLayer]->GetYaxis()->SetTitle("#phi");
    hEtaPhiHitmap[aLayer]->GetXaxis()->SetTitle("#eta");
    hEtaPhiHitmap[aLayer]->GetZaxis()->SetTitle("Number of Hits");
    se->registerHisto(this,  hEtaPhiHitmap[aLayer]);

    hChipStaveOccupancy[aLayer] = new TH2D(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStave", aLayer, aLayer), Form("MVTX Layer%d, Occupancy vs Chip and Stave", aLayer), 9, -.5, 9 - .5,     NStaves[aLayer], -.5, NStaves[aLayer] - .5);
    hChipStaveOccupancy[aLayer]->GetYaxis()->SetTitle("Stave Number");
    hChipStaveOccupancy[aLayer]->GetXaxis()->SetTitle("Chip Number");
    hChipStaveOccupancy[aLayer]->GetZaxis()->SetTitle("Number of Hits");
    se->registerHisto(this,  hChipStaveOccupancy[aLayer]);

    for (int aStave = 0; aStave < NStaves[aLayer]; aStave++) {
      for (int iChip = 0; iChip < 9; iChip++) {
        hChipHitmap[aLayer][aStave][iChip] = new TH2I(Form("MVTXMON/chipHitmapL%dS%dC%d", aLayer, aStave, iChip), Form("chipHitmapL%dS%dC%d", aLayer, aStave, iChip), 1024, -.5, 1023.5, 512, -.5, 511.5);
        hChipHitmap_evt[aLayer][aStave][iChip] = new TH2I(Form("MVTXMON/chipHitmapL%dS%dC%d_evt", aLayer, aStave, iChip), Form("chipHitmapL%dS%dC%d_evt", aLayer, aStave, iChip), 1024, -.5, 1023.5, 512, -.5, 511.5);
	hChipHitmap[aLayer][aStave][iChip]->GetXaxis()->SetTitle("Col");
	hChipHitmap[aLayer][aStave][iChip]->GetYaxis()->SetTitle("Row");
        se->registerHisto(this, hChipHitmap[aLayer][aStave][iChip]);
      }
    }
  }

  //fhr
  mErrorVsFeeid = new TH2I("MVTXMON/General/ErrorVsFeeid", "Error count vs Error id and Fee id", 3 * StaveBoundary[3], 0, 3 * StaveBoundary[3], NErrorExtended, 0.5, NErrorExtended + 0.5);
  mErrorVsFeeid->GetXaxis()->SetTitle("FEE ID");
  mErrorVsFeeid->GetYaxis()->SetTitle("Error ID");
  se->registerHisto(this,  mErrorVsFeeid);

  mGeneralOccupancy = new TH2Poly();
  mGeneralOccupancy->SetTitle("General Occupancy;mm;mm");
  mGeneralOccupancy->SetName("MVTXMON/General/General_Occupancy");
  //mGeneralOccupancy->GetXaxis()->SetTitle("");
  //mGeneralOccupancy->GetYaxis()->SetTitle("");
  mGeneralOccupancy->SetStats(0);

  mGeneralNoisyPixel = new TH2Poly();
  mGeneralNoisyPixel->SetTitle("Noisy Pixel Number;mm (IB 3x);mm (IB 3x)");
  mGeneralNoisyPixel->SetName("MVTXMON/General/Noisy_Pixel");
  mGeneralNoisyPixel->GetXaxis()->SetTitle("");
  mGeneralNoisyPixel->GetYaxis()->SetTitle("");

  createPoly(mGeneralOccupancy);
  createPoly(mGeneralNoisyPixel);
  se->registerHisto(this,  mGeneralOccupancy);
  se->registerHisto(this,  mGeneralNoisyPixel);

  for (int mLayer = 0; mLayer < 3; mLayer++) {
    //need geometry for this
    /*mDeadChipPos[mLayer] = new TH2D(Form("MVTXMON/Occupancy/Layer%d/Layer%dDeadChipPos", mLayer, mLayer), Form("DeadChipPos on Layer %d", mLayer), nbinsetaIB, etabinsIB[mLayer], nbinsphiIB, phibinsIB[mLayer]);    // every nine chips have same phi
    mDeadChipPos[mLayer] ->GetXaxis()->SetTitle("ChipEta");
    mDeadChipPos[mLayer] ->GetYaxis()->SetTitle("ChipPhi");
    mAliveChipPos[mLayer] = new TH2D(Form("MVTXMON/Occupancy/Layer%d/Layer%dAliveChipPos", mLayer, mLayer), Form("AliveChipPos on Layer %d", mLayer), nbinsetaIB, etabinsIB[mLayer], nbinsphiIB, phibinsIB[mLayer]); // every nine chips have same phi
    mAliveChipPos[mLayer]->GetXaxis()->SetTitle("ChipEta");
    mAliveChipPos[mLayer]->GetYaxis()->SetTitle("ChipPhi");*/
    mDeadChipPos[mLayer] = new TH2D(Form("MVTXMON/Occupancy/Layer%d/Layer%dDeadChipPos", mLayer, mLayer), Form("DeadChipPos on Layer %d", mLayer), 9, -0.5, 9 - 0.5, NStaves[mLayer], -0.5, NStaves[mLayer] - 0.5);
    mDeadChipPos[mLayer] ->GetXaxis()->SetTitle("Chip Number");
    mDeadChipPos[mLayer] ->GetYaxis()->SetTitle("Stave Number");
    mAliveChipPos[mLayer] = new TH2D(Form("MVTXMON/Occupancy/Layer%d/Layer%dAliveChipPos", mLayer, mLayer), Form("AliveChipPos on Layer %d", mLayer), 9, -0.5, 9 - 0.5, NStaves[mLayer], -0.5, NStaves[mLayer] - 0.5);
    mAliveChipPos[mLayer]->GetXaxis()->SetTitle("Chip Number");
    mAliveChipPos[mLayer]->GetYaxis()->SetTitle("Stave Number");

    mDeadChipPos[mLayer]->SetStats(0);
    mAliveChipPos[mLayer]->SetStats(0);


   // mChipStaveOccupancy[mLayer] = new TH2D(Form("MVTXMON/Occupancy/Layer%d/Layer%dChipStaveC", mLayer, mLayer), Form("ITMVTXS Layer%d, Occupancy vs Chip and Stave", mLayer), 9, -0.5, 9 - 0.5, NStaves[mLayer], -0.5, NStaves[mLayer] - 0.5);
   // mChipStaveOccupancy[mLayer]->GetXaxis()->SetTitle("Chip Number");
   // mChipStaveOccupancy[mLayer]->GetYaxis()->SetTitle("Stave Number");
    mOccupancyPlot[mLayer] = new TH1D(Form("MVTXMON/Occupancy/Layer%dOccupancy/LOG", mLayer), Form("MVTX Layer %d Noise pixels occupancy distribution", mLayer), 300, -15, 0);
    mOccupancyPlot[mLayer]->GetXaxis()->SetTitle("log(Occupancy)");
    mOccupancyPlot[mLayer]->GetYaxis()->SetTitle("");
    se->registerHisto(this,  mDeadChipPos[mLayer]);
    se->registerHisto(this,  mAliveChipPos[mLayer]);
    //se->registerHisto(this,  mChipStaveOccupancy[mLayer]);
    se->registerHisto(this,  mOccupancyPlot[mLayer]);
  }

  /*mTotalDeadChipPos = new TH2D(Form("MVTXMON/Occupancy/TotalDeadChipPos"), Form("TotalDeadChipPos "), mEtabins, -2.405, 2.405, mPhibins, -3.24, 3.24);
  mTotalDeadChipPos->GetXaxis()->SetTitle("ChipEta");
  mTotalDeadChipPos->GetYaxis()->SetTitle("ChipPhi");*/
  mTotalDeadChipPos = new TH2D(Form("MVTXMON/Occupancy/TotalDeadChipPos"), Form("TotalDeadChipPos "), 9, -0.5, 9 - 0.5, NStaves[2], -0.5, NStaves[2] - 0.5);
  mTotalDeadChipPos->GetXaxis()->SetTitle("Chip Number");
  mTotalDeadChipPos->GetYaxis()->SetTitle("Stave Number");
  se->registerHisto(this,  mTotalDeadChipPos);
  
  /*mTotalAliveChipPos = new TH2D(Form("MVTXMON/Occupancy/TotalAliveChipPos"), Form("TotalAliveChipPos "), mEtabins, -2.405, 2.405, mPhibins, -3.24, 3.24);
  mTotalAliveChipPos->GetXaxis()->SetTitle("ChipEta");
  mTotalAliveChipPos->GetYaxis()->SetTitle("ChipPhi");*/
  mTotalAliveChipPos = new TH2D(Form("MVTXMON/Occupancy/TotalAliveChipPos"), Form("TotalAliveChipPos "), 9, -0.5, 9 - 0.5, NStaves[2], -0.5, NStaves[2] - 0.5);
  mTotalAliveChipPos->GetXaxis()->SetTitle("Chip Number");
  mTotalAliveChipPos->GetYaxis()->SetTitle("Stave Number");
  se->registerHisto(this,  mTotalAliveChipPos);  


  dbvars = new OnlMonDB(ThisName);  // use monitor name for db table name
  DBVarInit();
  Reset();
  return 0;
}

int MvtxMon::BeginRun(const int /* runno */)
{
  // if you need to read calibrations on a run by run basis
  // this is the place to do it
  return 0;
}

int MvtxMon::process_event(Event *evt)
{
  evtcnt++;
  //std::cout << "Processing Event " << evtcnt << std::endl;
  OnlMonServer *se = OnlMonServer::instance();
  // using ONLMONBBCLL1 makes this trigger selection configurable from the outside
  // e.g. if the BBCLL1 has problems or if it changes its name
  if (!se->Trigger("ONLMONBBCLL1"))
  {
    std::ostringstream msg;
    msg << "Processing Event " << evtcnt
        << ", Trigger : 0x" << std::hex << se->Trigger()
        << std::dec;
    // severity levels and id's for message sources can be found in
    // $ONLINE_MAIN/include/msg_profile.h
    // The last argument is a message type. Messages of the same type
    // are throttled together, so distinct messages should get distinct
    // message types
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), TRGMESSAGE);
  }
  
  //int NAllHits = 0;

  for(int l = 0; l < NLAYERS; l++){
    for(int i = 0; i < NStaves[l]; i++){
      for(int j = 0; j < NCHIP; j++){
        mHitPerChip_evt[l][i][j] = 0;
        hChipHitmap_evt[l][i][j]->Reset("ICESM");
      }
    }
  }

 
//int test = 0;

  int packet_init = 2001;
  //std::cout<<"processing rcdaq event"<<std::endl;
  for ( int iPkt = 0; iPkt < 8; iPkt++ ){
    Packet *p = evt->getPacket(packet_init + iPkt);  
    if (p){
      //std::cout<<"PACKET: "<<iPkt<<std::endl;
      std::map<mvtx::InteractionRecord, std::vector<mvtx::ChipPixelData>> *data = reinterpret_cast<std::map<mvtx::InteractionRecord, std::vector<mvtx::ChipPixelData>>*>(p->pValue(-1, "ChipData"));
      int nevents_packet = 0; 
     if(data){
      for (auto const& mapit : *data){
        nevents_packet++;
        ntriggers++;
        COUT << "Key orbit: " << mapit.first.orbit<< " Key bc: " << mapit.first.bc <<ENDL;
        for(auto el : mapit.second){
          /*if(el.getChipID() ==270)*///std::cout<<" chip "<<el.getChipID()<<std::endl;
          //COUT << "Key orbit: " << mapit.first.orbit<< " Key bc: " << mapit.first.bc << ", Value orbit: " << el.getInteractionRecord().orbit << " chip id "<<el.getChipID()<<ENDL;
          //std::vector<PixelData> *data = reinterpret_cast<std::vector<PixelData>*>(p->pValue(ruchn, "ChipData"));
          std::vector<int> location = *(reinterpret_cast<std::vector<int>*>(p->pValue(el.getChipID(), "ChipLocation")));
          int feeid = 3 * StaveBoundary[location.at(0)] + location.at(1) * 3 + location.at(2)/3;
          //std::cout<<"chip id "<<el.getChipID()<<" feeid "<<location.at(3)<<" "<<feeid<<std::endl;
          for (int i = 0; i < 13; i++) {
            if (((uint32_t)(el.getTrigger()) >> i & 1) == 1) {
              mTrigger->Fill(i + 1);
              //if(i+1 == 10){ std::cout<<feeid<<" SOT "<<ntriggers<<std::endl;test++;}
              mTriggerVsFeeId->Fill(feeid, i + 1);
            }
          }
	  for (auto hit : el.getData()){
		//if(location.at(0) ==2 && el.getChipID()==270)std::cout<<"MON col "<<hit.getCol() << " row "<<hit.getRow()<<std::endl;
		//std::cout<<location.at(0)<<location.at(1)<<location.at(2)<<std::endl;
	        hChipHitmap[location.at(0)][location.at(1)][location.at(2)]->Fill(hit.getCol(),hit.getRow());
                hChipHitmap_evt[location.at(0)][location.at(1)][location.at(2)]->Fill(hit.getCol(),hit.getRow());
		mHitPerChip[location.at(0)][location.at(1)][location.at(2)]++;
                mHitPerChip_evt[location.at(0)][location.at(1)][location.at(2)]++;
                //mHitPerChip[location.at(0)][location.at(1)][location.at(2)] = 0;
	  }
        }
      }
  std::cout<<"nevents: "<<nevents_packet<<std::endl;
//std::cout<<"n SOT "<<test<<std::endl;

  double pixelOccupancy, chipOccupancy;
  for (int iLayer = 0; iLayer < 3; iLayer++) {
    //if (!mlayerEnable[iLayer]) {
    //   continue;
    // }
    //hEtaPhiHitmap[iLayer]->Reset();
    for (int iStave = 0; iStave < NStaves[iLayer]; iStave++) {
      for (int iChip = 0; iChip < 9; iChip++) {
        //chipOccupancy = mHitPerChip[iLayer][iStave][iChip];//hChipHitmap[iLayer][iStave][iChip]->Integral();
        //chipOccupancy = hChipHitmap_evt[iLayer][iStave][iChip]->Integral();
        //chipOccupancy = chipOccupancy / ((/*double)nevents_packet **/ (double)NPixels);
        //hChipStaveOccupancy[iLayer]->Fill(iChip, iStave, chipOccupancy);
        chipOccupancy = hChipHitmap[iLayer][iStave][iChip]->Integral() / ((double)ntriggers * (double)NPixels);
        hChipStaveOccupancy[iLayer]->SetBinContent(iChip+1, iStave+1, chipOccupancy);
        mvtxmon_ChipStave1D->Fill((iLayer==0?iStave:NStaves[iLayer]+iStave)*9+iChip+1,chipOccupancy);
        for (int iCol = 0; iCol < NCols; iCol++) {
          for (int iRow = 0; iRow < NRows; iRow++) {
            pixelOccupancy = hChipHitmap_evt[iLayer][iStave][iChip]->GetBinContent(iCol + 1, iRow + 1);
            if (pixelOccupancy > 0) {
//if(iLayer == 2) std::cout<<pixelOccupancy<<" "<<ntriggers<<std::endl; 
              if (pixelOccupancy/(double)nevents_packet > mOccupancyCutForNoisyPixel) {
                  mNoisyPixelNumber[iLayer][iStave][iChip]++;
              }
              pixelOccupancy /= (double)nevents_packet;       
//if(iLayer == 2) std::cout<<pixelOccupancy<<" "<<log10(pixelOccupancy)<<std::endl;                      
              hOccupancyPlot[iLayer]->Fill(log10(pixelOccupancy));

            }
          }
        }
      }
    }
  }


  for (int iLayer = 0; iLayer < 3; iLayer++) {
    for (int iStave = 0; iStave < NStaves[iLayer]; iStave++) {
      for (int iChip = 0; iChip < 9; iChip++) {
        double occ = *(std::max_element(mHitPerChip[iLayer][iStave], mHitPerChip[iLayer][iStave] + 9));
        mGeneralOccupancy->SetBinContent(mapstave[iLayer][iStave], occ/((double)NPixels*9*ntriggers));
        double noisy = *(std::max_element(mNoisyPixelNumber[iLayer][iStave], mNoisyPixelNumber[iLayer][iStave] + 9));
        mGeneralNoisyPixel->SetBinContent(mapstave[iLayer][iStave], noisy);
      }
    }
  }

  for (int iLayer = 0; iLayer < 3; iLayer++) {
    for (int iStave = 0; iStave < NStaves[iLayer]; iStave++) {
      for (int iChip = 0; iChip < 9; iChip++) {
       /* if (!mHitPerChip_evt[iLayer][istave][ichip]) {
          mDeadChipPos[iLayer]->SetBinContent(mDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
          mTotalDeadChipPos->SetBinContent(mTotalDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mTotalDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
        } else {
          mAliveChipPos[iLayer]->SetBinContent(mAliveChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mAliveChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
          mTotalAliveChipPos->SetBinContent(mTotalAliveChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mTotalAliveChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
          mDeadChipPos[iLayer]->SetBinContent(mDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 0);                // not dead
          mTotalDeadChipPos->SetBinContent(mTotalDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mTotalDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 0); // not dead
        }*/
        if (!mHitPerChip_evt[iLayer][iStave][iChip]) {
          mDeadChipPos[iLayer]->SetBinContent(mDeadChipPos[iLayer]->GetXaxis()->FindBin(iChip), mDeadChipPos[iLayer]->GetYaxis()->FindBin(iStave), 1);
          mTotalDeadChipPos->SetBinContent(mTotalDeadChipPos->GetXaxis()->FindBin(iChip), mTotalDeadChipPos->GetYaxis()->FindBin(iStave), 1);
        } else {
          mAliveChipPos[iLayer]->SetBinContent(mAliveChipPos[iLayer]->GetXaxis()->FindBin(iChip), mAliveChipPos[iLayer]->GetYaxis()->FindBin(iStave), 1);
          mTotalAliveChipPos->SetBinContent(mTotalAliveChipPos->GetXaxis()->FindBin(iChip), mTotalAliveChipPos->GetYaxis()->FindBin(iStave), 1);
          mDeadChipPos[iLayer]->SetBinContent(mDeadChipPos[iLayer]->GetXaxis()->FindBin(iChip), mDeadChipPos[iLayer]->GetYaxis()->FindBin(iStave), 0);                // not dead
          mTotalDeadChipPos->SetBinContent(mTotalDeadChipPos->GetXaxis()->FindBin(iChip), mTotalDeadChipPos->GetYaxis()->FindBin(iStave), 0); // not dead
        }
}
}
}
}


	
		
		delete p;

	}
  }


  // get temporary pointers to histograms
  // one can do in principle directly se->getHisto("mvtxhist1")->Fill()
  // but the search in the histogram Map is somewhat expensive and slows
  // things down if you make more than one operation on a histogram

 /* mvtxhist1->Fill((float) idummy);
  mvtxhist2->Fill((float) idummy, (float) idummy, 1.);
*/
 // if (idummy++ > 10)
  //{
    if (dbvars)
    {
      dbvars->SetVar("n_events", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);
      dbvars->SetVar("mvtxmondummy", sin((double) evtcnt), cos((double) se->Trigger()), (float) evtcnt);
      dbvars->SetVar("mvtxmonnew", (float) se->Trigger(), 10000. / se->CurrentTicks(), (float) evtcnt);
      dbvars->DBcommit();
    }
    std::ostringstream msg;
    msg << "Filling Histos";
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), FILLMESSAGE);
    idummy = 0;
 // }
  return 0;
}

int MvtxMon::Reset()
{
  // reset our internal counters
  evtcnt = 0;
  idummy = 0;
  return 0;
}

int MvtxMon::DBVarInit()
{
  // variable names are not case sensitive
  std::string varname;
  varname = "n_events";
  dbvars->registerVar(varname);
  varname = "mvtxmondummy";
  dbvars->registerVar(varname);
  varname = "mvtxmonnew";
  dbvars->registerVar(varname);
  if (verbosity > 0)
  {
    dbvars->Print();
  }
  dbvars->DBInit();
  return 0;
}

void MvtxMon::getStavePoint(int layer, int stave, double* px, double* py)
{
  float stepAngle = M_PI * 2 / NStaves[layer];              // the angle between to stave
  float midAngle = StartAngle[layer] + (stave * stepAngle); // mid point angle
  float staveRotateAngle = M_PI / 2 - (stave * stepAngle);  // how many angle this stave rotate(compare with first stave)
  px[1] = MidPointRad[layer] * std::cos(midAngle);          // there are 4 point to decide this TH2Poly bin
                                                            // 0:left point in this stave;
                                                            // 1:mid point in this stave;
                                                            // 2:right point in this stave;
                                                            // 3:higher point int this stave;
  py[1] = MidPointRad[layer] * std::sin(midAngle);          // 4 point calculated accord the blueprint
                                                            // roughly calculate
  px[0] = 7.7 * std::cos(staveRotateAngle) + px[1];
  py[0] = -7.7 * std::sin(staveRotateAngle) + py[1];
  px[2] = -7.7 * std::cos(staveRotateAngle) + px[1];
  py[2] = 7.7 * std::sin(staveRotateAngle) + py[1];
  px[3] = 5.623 * std::sin(staveRotateAngle) + px[1];
  py[3] = 5.623 * std::cos(staveRotateAngle) + py[1];
}

void MvtxMon::drawLayerName(TH2* histo2D)
{
  TLatex* t[NLAYERS];
  double minTextPosX[NLAYERS] = { 1, 42, 92};
  for (int ilayer = 0; ilayer < NLAYERS; ilayer++) {
    t[ilayer] = new TLatex(minTextPosX[ilayer], 28.3, Form("Layer %d", ilayer));
    histo2D->GetListOfFunctions()->Add(t[ilayer]);
  }
  for (const int& lay : LayerBoundaryFEE) {
    auto l = new TLine(lay, 0, lay, histo2D->GetNbinsY());
    histo2D->GetListOfFunctions()->Add(l);
  }
}


void MvtxMon::createPoly(TH2Poly *h){
  for (int ilayer = 0; ilayer < NLAYERS; ilayer++) {
    for (int istave = 0; istave < NStaves[ilayer]; istave++) {
      double* px = new double[4];
      double* py = new double[4];
      getStavePoint(ilayer, istave, px, py);
      for (int icoo = 0; icoo < 4; icoo++) {
        px[icoo] *= 3.;
        py[icoo] *= 3.;
      }
      h->AddBin(4, px, py);
    }
  }
}
