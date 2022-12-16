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

  //HitMap_1_4 = new TH2D ;1	2D HitMap: Run 872 Stave 1 and Chip ID 4
  mvtxmon_ChipStaveOcc = new TH2D("mvtxmon_ChipStaveOcc","Average Occupancy: Run XX Stave Number and Chip Number",NCHIP,-0.5,NCHIP-0.5,NSTAVE,-0.5,NSTAVE-0.5);
  mvtxmon_ChipStave1D = new TH1D("mvtxmon_ChipStave1D","Average Occupancy: Run XX Stave Number and Chip Number",NCHIP*NSTAVE,-0.5,NCHIP * NSTAVE-0.5);
  mvtxmon_ChipFiredHis= new TH1D("mvtxmon_ChipFiredHis","Number of Chips Fired in Each Event Distribution: Run XX",NCHIP*NSTAVE,-0.5,NCHIP*NSTAVE - 0.5);
  mvtxmon_EvtHitChip= new TH1D("mvtxmon_EvtHitChip","Number of Hits Per Event Per Chip Distribution: Run XX",25,-0.5,24.5);
  mvtxmon_EvtHitDis = new TH1D("mvtxmon_EvtHitDis","Number of Hits Per Event Distribution: Run XX",25,-0.5,24.5);
  //InfoCanvas= new TH1D;1	QC Process Information Canvas

  OnlMonServer *se = OnlMonServer::instance();
  // register histograms with server otherwise client won't get them
  se->registerHisto(this, mvtxmon_ChipStaveOcc);  // uses the TH1->GetName() as key
  se->registerHisto(this, mvtxmon_ChipStave1D);
  se->registerHisto(this, mvtxmon_ChipFiredHis);
  se->registerHisto(this, mvtxmon_EvtHitChip);
  se->registerHisto(this, mvtxmon_EvtHitDis);
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
								//int hit = p->iValue(ruid,ruchn,i);
								//int irow = decode_row(hit);
								//int icol = decode_col(hit);
								//cout << "(ruid " << ruid << ", ruchn " << ruchn << ") ";
								//cout << "(row " << irow << ", col " << icol << ") ";
								if (chipmap.count({ruid,ruchn}) != 1) {
									std::cout << "invalid: (ruid " << ruid << ", ruchn " << ruchn << ") " << std::endl;
								} else {
									std::pair<int, int> chiplocation = chipmap[{ruid,ruchn}];
									int istave = chiplocation.first;
									int ichip = chiplocation.second;
									HitPerChip[istave][ichip] = HitPerChip[istave][ichip] + 1;

								//	HitMap[istave][ichip]->Fill(icol,irow); do later

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

  if (idummy++ > 10)
  {
    if (dbvars)
    {
      dbvars->SetVar("mvtxmoncount", (float) evtcnt, 0.1 * evtcnt, (float) evtcnt);
      dbvars->SetVar("mvtxmondummy", sin((double) evtcnt), cos((double) se->Trigger()), (float) evtcnt);
      dbvars->SetVar("mvtxmonnew", (float) se->Trigger(), 10000. / se->CurrentTicks(), (float) evtcnt);
      dbvars->DBcommit();
    }
    std::ostringstream msg;
    msg << "Filling Histos";
    se->send_message(this, MSG_SOURCE_UNSPECIFIED, MSG_SEV_INFORMATIONAL, msg.str(), FILLMESSAGE);
    idummy = 0;
  }*/
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
  varname = "mvtxmoncount";
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
