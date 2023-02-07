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
#include <TH2.h>
#include <TH2Poly.h>

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
  //mvtxhist1 = new TH1F("mvtxmon_hist1", "test 1d histo", 101, 0., 100.);
  //mvtxhist2 = new TH2F("mvtxmon_hist2", "test 2d histo", 101, 0., 100., 101, 0., 100.);
   OnlMonServer *se = OnlMonServer::instance();

  //HitMap_1_4 = new TH2D ;1	2D HitMap: Run 872 Stave 1 and Chip ID 4
  mvtxmon_ChipStaveOcc = new TH2D("mvtxmon_ChipStaveOcc","Average Occupancy: Run XX Stave Number and Chip Number",NCHIP,-0.5,NCHIP-0.5,NSTAVE,-0.5,NSTAVE-0.5);
  mvtxmon_ChipStave1D = new TH1D("mvtxmon_ChipStave1D","Average Occupancy: Run XX Stave Number and Chip Number",NCHIP*NSTAVE,-0.5,NCHIP * NSTAVE-0.5);
  mvtxmon_ChipFiredHis= new TH1D("mvtxmon_ChipFiredHis","Number of Chips Fired in Each Event Distribution: Run XX",NCHIP*NSTAVE,-0.5,NCHIP*NSTAVE - 0.5);
  mvtxmon_EvtHitChip= new TH1D("mvtxmon_EvtHitChip","Number of Hits Per Event Per Chip Distribution: Run XX",25,-0.5,24.5);
  mvtxmon_EvtHitDis = new TH1D("mvtxmon_EvtHitDis","Number of Hits Per Event Distribution: Run XX",25,-0.5,24.5);
  for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){
			mvtxmon_HitMap[i][j] = new TH2D(Form("mvtxmon_HitMap_%d_%d",i,j),Form("HitMap_%d_%d",i,j),NBins,-0.5,NColMax+0.5,NBins,-0.5,NRowMax+0.5); 
      se->registerHisto(this, mvtxmon_HitMap[i][j]);
    }
  }
 for (int i = 0; i < NFlags; i++) {
    mvtxmon_LaneStatusOverview[i] = new TH2Poly();
    mvtxmon_LaneStatusOverview[i]->SetName(Form("LaneStatus/laneStatusOverviewFlag%s", mLaneStatusFlag[i].c_str()));
  }

  mvtxmon_GeneralOccupancy = new TH2Poly();
  mvtxmon_GeneralOccupancy->SetTitle("General Occupancy;mm (IB 3x);mm (IB 3x)");
  mvtxmon_GeneralOccupancy->SetName("General/General_Occupancy");
  mvtxmon_GeneralOccupancy->SetStats(0);
  mvtxmon_GeneralOccupancy->SetMinimum(pow(10, mMinGeneralAxisRange));
  mvtxmon_GeneralOccupancy->SetMaximum(pow(10, mMaxGeneralAxisRange));

    for (int ilayer = 0; ilayer < 3; ilayer++) {
      for (int istave = 0; istave < NStaves[ilayer]; istave++) {
        double* px = new double[4];
        double* py = new double[4];
        getStavePoint(ilayer, istave, px, py);
        if (ilayer < 3) {
          for (int icoo = 0; icoo < 4; icoo++) {
            px[icoo] *= 3.;
            py[icoo] *= 3.;
          }
        }
        mvtxmon_GeneralOccupancy->AddBin(4, px, py);
      }
    }

  for (int i = 0; i < NFlags; i++) {
    TString title = Form("Fraction of lanes into %s", mLaneStatusFlag[i].c_str());
    title += ";mm (IB 3x);mm (IB 3x)";
    mvtxmon_LaneStatusOverview[i]->SetTitle(title);
    mvtxmon_LaneStatusOverview[i]->SetStats(0);
    mvtxmon_LaneStatusOverview[i]->SetOption("lcolz");
    mvtxmon_LaneStatusOverview[i]->SetMinimum(0);
    mvtxmon_LaneStatusOverview[i]->SetMaximum(1);
    mvtxmon_LaneStatusOverview[i]->SetBit(TH1::kIsAverage);
    for (int ilayer = 0; ilayer < 3; ilayer++) {
      for (int istave = 0; istave < NStaves[ilayer]; istave++) {
        double* px = new double[4];
        double* py = new double[4];
        getStavePoint(ilayer, istave, px, py);
        if (ilayer < 3) {
          for (int icoo = 0; icoo < 4; icoo++) {
            px[icoo] *= 3.;
            py[icoo] *= 3.;
          }
        }
        mvtxmon_LaneStatusOverview[i]->AddBin(4, px, py);
      }
    }
    se->registerHisto(this, mvtxmon_LaneStatusOverview[i]); // mLaneStatusOverview
  }

  
  //InfoCanvas= new TH1D;1	QC Process Information Canvas

 
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, mvtxmon_ChipStaveOcc);  // uses the TH1->GetName() as key
  se->registerHisto(this, mvtxmon_ChipStave1D);
  se->registerHisto(this, mvtxmon_ChipFiredHis);
  se->registerHisto(this, mvtxmon_EvtHitChip);
  se->registerHisto(this, mvtxmon_EvtHitDis);
  se->registerHisto(this, mvtxmon_GeneralOccupancy);
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
  
int NAllHits = 0;


	for(int i = 0; i < NSTAVE; i++){

		for(int j = 0; j < NCHIP; j++){

			HitPerChip[i][j] = 0;

		}

	}


	Packet *p = evt->getPacket(2000);
	if (p)
	{

		bool evnt_err = false;



		//int npixels[NSTAVE] = {0};
		//double mrow[NSTAVE] = {0};
		//double mcol[NSTAVE] = {0};
		//double mrow_refstave = -1;
		//double mcol_refstave = -1;

		if ( !evnt_err ) {
			for (int ruid=0; ruid<IDMVTXV1_MAXRUID+1; ruid++)
			{
				if (p->iValue(ruid)!=-1)
				{
					for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
					{
						if (p->iValue(ruid,ruchn)>0)
						{
							for (int i=0;i<p->iValue(ruid,ruchn);i++)
							{
								int hit = p->iValue(ruid,ruchn,i);
								int irow = decode_row(hit);
								int icol = decode_col(hit);
								//cout << "(ruid " << ruid << ", ruchn " << ruchn << ") ";
								//cout << "(row " << irow << ", col " << icol << ") ";
								if (chipmap.count({ruid,ruchn}) != 1) {
									std::cout << "invalid: (ruid " << ruid << ", ruchn " << ruchn << ") " << std::endl;
								} else {
									std::pair<int, int> chiplocation = chipmap[{ruid,ruchn}];
									int istave = chiplocation.first;
									int ichip = chiplocation.second;
									HitPerChip[istave][ichip] = HitPerChip[istave][ichip] + 1;

									mvtxmon_HitMap[istave][ichip]->Fill(icol,irow);

								//	ChipStave->Fill(ichip,istave); do later
									NAllHits = NAllHits + 1;

								}
							}
							//cout << endl;
						}
					}
				}
			}

			for(int i = 0; i < NSTAVE; i++){

				for(int j = 0; j < NCHIP; j++){

					int Index = NCHIP * i + j;

					OccPerChip[i][j] = float(HitPerChip[i][j])/(float(NRowMax) * float(NRowMax));

					mvtxmon_ChipStaveOcc->Fill(j,i,OccPerChip[i][j]);
					mvtxmon_ChipStave1D->Fill(Index,OccPerChip[i][j]);
				}
			}



			mvtxmon_EvtHitDis->Fill(NAllHits);
			int NChipFired = 0;

			for(int i = 0; i < NSTAVE; i++){

				for(int j = 0; j < NCHIP; j++){

					if(HitPerChip[i][j] > 0){
						mvtxmon_EvtHitChip->Fill(HitPerChip[i][j]);
						NChipFired = NChipFired + 1;
					}


				}

			}

			mvtxmon_ChipFiredHis->Fill(NChipFired);

mOccupancyLane = new double*[4/*NStaves[mLayer]*/];


// calculate active staves according digit hit vector
  std::vector<int> activeStaves;
  for (int i = 0; i < 4/*NStaves[mLayer]*/; i++) {
  mOccupancyLane[i] = new double[9];
    for (int j = 0; j < 9/*nHicPerStave[mLayer]*/; j++) {
      if (HitPerChip[i][j] != 0) {
        activeStaves.push_back(i);
	mOccupancyLane[i][j] = HitPerChip[i][j] / (/*GBTLinkInfo->statistics.nTriggers * */ 1024. * 512.);
        break;
      }
    }
  }


for (int i = 0; i < (int)activeStaves.size(); i++) {
    int istave = activeStaves[i];
   // mOccupancyPlot->Add(occupancyPlotTmp[i]);
      for (int ichip = 0; ichip < 9; ichip++) {
        //mChipStaveOccupancy->SetBinContent(ichip + 1, istave + 1, mOccupancyLane[istave][ichip]);
       /* if (!mChipStat[istave][ichip]) {
          mDeadChipPos->SetBinContent(mDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
          mTotalDeadChipPos->SetBinContent(mTotalDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mTotalDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
        } else {
          mAliveChipPos->SetBinContent(mAliveChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mAliveChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
          mTotalAliveChipPos->SetBinContent(mTotalAliveChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mTotalAliveChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 1);
          mDeadChipPos->SetBinContent(mDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 0);                // not dead
          mTotalDeadChipPos->SetBinContent(mTotalDeadChipPos->GetXaxis()->FindBin(mChipEta[istave][ichip] + 0.009), mTotalDeadChipPos->GetYaxis()->FindBin(mChipPhi[istave][ichip] + 0.001), 0); // not dead
        }*/
        /*int ilink = ichip / 3;
        for (int ierror = 0; ierror < o2::itsmft::GBTLinkDecodingStat::NErrorsDefined; ierror++) {
          if (mErrorVsFeeid && (mErrorCount[istave][ilink][ierror] != 0)) {
            mErrorVsFeeid->SetBinContent(((istave + StaveBoundary[mLayer]) * 3) + ilink + 1, ierror + 1, mErrorCount[istave][ilink][ierror]);
          }
        }*/
      }
      mvtxmon_GeneralOccupancy->SetBinContent(istave + 1 + StaveBoundary[0/*mLayer*/], *(std::max_element(mOccupancyLane[istave], mOccupancyLane[istave] + 9/*nChipsPerHic[mLayer]*/)));
      //mGeneralNoisyPixel->SetBinContent(istave + 1 + StaveBoundary[mLayer], mNoisyPixelNumber[mLayer][istave]);
    }


		}



		//hnevnt->Fill(0);
		delete p;

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
  float stepAngle = M_PI * 2 / NStaves[layer];             // the angle between to stave
  float midAngle = StartAngle[layer] + (stave * stepAngle);       // mid point angle
  float staveRotateAngle = M_PI / 2 - (stave * stepAngle); // how many angle this stave rotate(compare with first stave)
  px[1] = MidPointRad[layer] * std::cos(midAngle);              // there are 4 point to decide this TH2Poly bin
                                                                  // 0:left point in this stave;
                                                                  // 1:mid point in this stave;
                                                                  // 2:right point in this stave;
                                                                  // 3:higher point int this stave;
  py[1] = MidPointRad[layer] * std::sin(midAngle);            // 4 point calculated accord the blueprint
                                                                  // roughly calculate
  px[0] = 7.7 * std::cos(staveRotateAngle) + px[1];
  py[0] = -7.7 * std::sin(staveRotateAngle) + py[1];
  px[2] = -7.7 * std::cos(staveRotateAngle) + px[1];
  py[2] = 7.7 * std::sin(staveRotateAngle) + py[1];
  px[3] = 5.623 * std::sin(staveRotateAngle) + px[1];
  py[3] = 5.623 * std::cos(staveRotateAngle) + py[1];
}
