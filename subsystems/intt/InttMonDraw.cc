#include "InttMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>
#include <onlmon/RunDBodbc.h>

#include <phool/phool.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TPie.h>
#include <TPieSlice.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TArrow.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>  // for memset
#include <ctime>
#include <iomanip>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector
#include <string>

using namespace std;

const char *CARM[] = { "South", "North" };
ofstream outfile("fvtxclientlog.txt");

// Definitions for array of canvases
const int SUMMARYS = 0;
const int SUMMARYN = 1;
const int YIELDSS = 2;
const int YIELDSN = 3;
const int YIELDSPACKETS = 4;
const int YIELDSPACKETN = 5;
const int ADCS = 6;
const int ADCN = 7;
const int CHIPCHANNELSE = 8;
const int CHIPCHANNELNE = 9;
const int CHIPCHANNELSW = 10;
const int CHIPCHANNELNW = 11;
const int CONTROLWORDS = 12;
const int CONTROLWORDN = 13;
const int YIELDSPACKETSVSTIME = 14;
const int YIELDSPACKETNVSTIME = 15;
const int YIELDSPACKETSVSTIMESHORT = 16;
const int YIELDSPACKETNVSTIMESHORT = 17;

const char *ROCMap12[] = {  //set labels for Yield per Event plots here
  "SW0",  //      AB?
  "SW1",  //                       A -> 5B              B?
  "SW2",  // Large leak current in Run-12    A -> 0A   B -> 0B
  "SW3",  //                       A -> 1A   B -> 2A     B -> 2A
  "SW4",  //                       A -> 2B   B -> 3A     B -> 3A
  "SW5",  //  A -> 3B   B -> 4A     B -> 4A

  "SE0",  // Fiber Out in Run-12            B -> 0B    A?
  "SE1",  //                       A -> 1A  B -> 1B 
  "SE2",  //                       A -> 2A  B -> 2B 
  "SE3",  //                       A -> 3A?             B?
  "SE4",  //                           A -> 4A  B -> 4B
  "SE5"  //                           A -> 5A  B -> 5B
  //  "---",
  //  "---",
  //  "---",
  //  "---"
}; 

const char *ROCMap34[] = {

  "NW0",  // Large leak current in Run-12  A -> 0A   B -> 0B
  "NW1",  //                     A -> 1A   B -> 1B
  "NW2",  //  B1 fixed itself at 4BOFF in Run-12           AB?        
  "NW3",  //                     A -> 3A   B -> 3B      
  "NW4",  //                     A -> 4A   B -> 4B
  "NW5",  //                     B -> 5B   A?

  "NE0",  //            A -> 2A    B -> 2B
  "NE1",  //                             A -> 2A    B -> 2B
  "NE2",  //                             A -> 2A    B -> 2B
  "NE3",  //                             A -> 3A    B -> 3B
  "NE4",  //                             A -> 4A    B -> 4B
  "NE5"   //                             A -> 5A    B -> 5B
  //  "---",
  //  "---",
  //  "---"
};


const char *ROCMap12_cw[] = {  //set labels for Control Word plots here
  "SW0",  //  
  "SW1",  //                
  "SW2",  //
  "SW3",  //                
  "SW4",  //                 
  "SW5",  // 

  "SE0",  //
  "SE1",  //             
  "SE2",  //              
  "SE3",  //                  
  "SE4",  //                   
  "SE5"  //                  

}; 

const char *ROCMap34_cw[] = {

  "NW0",
  "NW1",  //                 
  "NW2",  //   
  "NW3",  //                     
  "NW4",  //                   
  "NW5",  //                 

  "NE0",  //          
  "NE1",  //                          
  "NE2",  //                          
  "NE3",  //                          
  "NE4",  //                        
  "NE5"   //                        

};

const char *ControlWordBit[] = {

  "Buck0 ",
  "Buck1 ",  //                 
  "Buck2 ",  //   
  "Buck3 ",  //                     
  "FPHX0 ",  //                   
  "FPHX1 ",  //                 
  "FPHX2",  //          
  "FPHX3 ",  //                          
  "ROC0 ",  //                          
  "ROC1 ",  //                          
  "ROC2 ",  //                        
  "ROC3 ",   //                        
  "FEM "
};

















InttMonDraw::InttMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  
  m_run_number = -1;
  m_run_type = "";
  rundb_odbc = new RunDBodbc();  
  // rundb_odbc = new RunDBodbc();


  canv_name[SUMMARYS]="fvtxSummarySouth";
  canv_name[SUMMARYN]="fvtxSummaryNorth";
  canv_name[YIELDSS]="fvtxYieldsSouth";
  canv_name[YIELDSN]="fvtxYieldsNorth";
  canv_name[YIELDSPACKETS]="fvtxYieldsByPacketSouth";
  canv_name[YIELDSPACKETN]="fvtxYieldsByPacketNorth";
  canv_name[ADCS]="fvtxAdcSouth";
  canv_name[ADCN]="fvtxAdcNorth";
  canv_name[CHIPCHANNELSE]="fvtxChipVsChannelSouthE";
  canv_name[CHIPCHANNELNE]="fvtxChipVsChannelNorthE";
  canv_name[CHIPCHANNELSW]="fvtxChipVsChannelSouthW";
  canv_name[CHIPCHANNELNW]="fvtxChipVsChannelNorthW";
  canv_name[CONTROLWORDS]="fvtxControlWordS";
  canv_name[CONTROLWORDN]="fvtxControlWordN";
  canv_name[YIELDSPACKETSVSTIME]="fvtxYieldsByPacketSouthVsTime";
  canv_name[YIELDSPACKETNVSTIME]="fvtxYieldsByPacketNorthVsTime";
  canv_name[YIELDSPACKETSVSTIMESHORT]="fvtxYieldsByPacketSouthVsTimeShort";
  canv_name[YIELDSPACKETNVSTIMESHORT]="fvtxYieldsByPacketNorthVsTimeShort";


  canv_desc[SUMMARYS]="fvtx_Summary_By_Wedge_South";
  canv_desc[SUMMARYN]="fvtx_Summary_By_Wedge_North";
  canv_desc[YIELDSS]="fvtx_Yields_Per_Event_By_Wedge_South";
  canv_desc[YIELDSN]="fvtx_Yields_Per_Event_By_Wedge_North";
  canv_desc[YIELDSPACKETS]="fvtx_Yields_Per_Event_By_Packet_South";
  canv_desc[YIELDSPACKETN]="fvtx_Yields_Per_Event_By_Packet_North";
  canv_desc[ADCS]="fvtx_Adc_By_Station_South";
  canv_desc[ADCN]="fvtx_Adc_By_Station_North";
  canv_desc[CHIPCHANNELSE]="fvtx_Chip_Vs_Channel_By_Roc_SouthE";
  canv_desc[CHIPCHANNELNE]="fvtx_Chip_Vs_Channel_By_Roc_NorthE";
  canv_desc[CHIPCHANNELSW]="fvtx_Chip_Vs_Channel_By_Roc_SouthW";
  canv_desc[CHIPCHANNELNW]="fvtx_Chip_Vs_Channel_By_Roc_NorthW";
  canv_desc[CONTROLWORDS]="fvtx_FEM_Control_Word_South";
  canv_desc[CONTROLWORDN]="fvtx_FEM_Control_Word_North";
  canv_desc[YIELDSPACKETSVSTIME]="fvtx_Yields_Per_Event_By_Packet_Vs_Time_South";
  canv_desc[YIELDSPACKETNVSTIME]="fvtx_Yields_Per_Event_By_Packet_Vs_Time_North";
  canv_desc[YIELDSPACKETSVSTIMESHORT]="fvtx_Yields_Per_Event_By_Packet_Vs_Time_South_Short";
  canv_desc[YIELDSPACKETNVSTIMESHORT]="fvtx_Yields_Per_Event_By_Packet_Vs_Time_North_Short";

  npad[SUMMARYS]=4;
  npad[SUMMARYN]=4;
  npad[YIELDSS]=1;
  npad[YIELDSN]=1;
  npad[YIELDSPACKETS]=1;
  npad[YIELDSPACKETN]=1;
  npad[ADCS]=4;
  npad[ADCN]=4;
  npad[CHIPCHANNELSE]=6;
  npad[CHIPCHANNELNE]=6;
  npad[CHIPCHANNELSW]=6;
  npad[CHIPCHANNELNW]=6;
  npad[CONTROLWORDS]=1;
  npad[CONTROLWORDN]=1;
  npad[YIELDSPACKETSVSTIME]=1;
  npad[YIELDSPACKETNVSTIME]=1;
  npad[YIELDSPACKETSVSTIMESHORT]=1;
  npad[YIELDSPACKETNVSTIMESHORT]=1;

  
  for(int icanvas=0; icanvas<ncanvas; icanvas++)
    {
      TC[icanvas]=0;
      transparent[icanvas]=0;
      for(int ipad=0; ipad<nmaxpad; ipad++)
	{
	  TP[icanvas][ipad]=0;
	}
    }

  double vals[48];
  for (int ival=0; ival<48; ival++) vals[ival] = 1.;
  pieFvtxDisk = new TPie("pieFvtxDisk","", 48, vals);
  pieFvtxDisk->SetCircle(0.5,0.5,0.45);

  double vals2[12];
  for (int ival=0; ival<12; ival++)
    {
      vals2[ival] = 1.;
    }

  double pie_r = pieFvtxDisk->GetRadius();
  pieROCname = new TPie("pieROCname","", 12, vals2);
  pieROCname->SetRadius(0.65*pie_r);
  pieROCname->SetLabelsOffset(-0.10);

  double vals3[2];
  for (int ival=0; ival<1; ival++) vals3[ival] = 1.;
  pieCenter = new TPie("pieCenter","", 2, vals3);
  pieCenter->SetRadius(0.35*pie_r);
  pieCenter->SetLabelsOffset(-0.25);


  // CAA Set everything for text, lines, etc.
  tsta0 = new TPaveText(0.2,0.8,0.2,0.90,"NDC");
  tsta0->SetTextSize(0.035);
  tsta0->SetFillColor(kWhite);
  tsta0->SetBorderSize(0);
  tsta0->AddText("Station 0");
  tsta0->AddText("West      East");

  tsta1 = new TPaveText(0.4,0.8,0.4,0.88,"NDC");
  tsta1->SetTextSize(0.035);
  tsta1->SetFillColor(kWhite);
  tsta1->SetBorderSize(0);
  tsta1->AddText("Station 1");
  tsta1->AddText("West      East");

  tsta2 = new TPaveText(0.6,0.8,0.6,0.88,"NDC");
  tsta2->SetTextSize(0.035);
  tsta2->SetFillColor(kWhite);
  tsta2->SetBorderSize(0);
  tsta2->AddText("Station 2");
  tsta2->AddText("West      East");

  tsta3 = new TPaveText(0.8,0.8,0.8,0.88,"NDC");
  tsta3->SetTextSize(0.035);
  tsta3->SetFillColor(kWhite);
  tsta3->SetBorderSize(0);
  tsta3->AddText("Station 3");
  tsta3->AddText("West      East");

  line1 = new TLine(48-0.5,0,48-0.5,50);
  line1->SetLineStyle(2);
  line2 = new TLine(96-0.5,0,96-0.5,50);
  line2->SetLineStyle(2);
  line3 = new TLine(144-0.5,0,144-0.5,50);
  line3->SetLineStyle(2);
  
  line_ccs1 = new TLine(128,0,128,104);
  line_ccs1->SetLineColor(kMagenta);
  line_ccs2 = new TLine(256,0,256,104);
  line_ccs2->SetLineColor(kMagenta);
  line_ccs3 = new TLine(384,0,384,104);
  line_ccs3->SetLineColor(kMagenta);
  line_ccs4 = new TLine(0,26,512,26);
  line_ccs4->SetLineColor(kMagenta);
  line_ccs5 = new TLine(0,52,512,52);
  line_ccs5->SetLineColor(kMagenta);
  line_ccs6 = new TLine(0,78,512,78);
  line_ccs6->SetLineColor(kMagenta);


  for (int i=0; i<12; i++)
    {
      lineROC[i] = new TLine(i*2+.5,0,i*2+.5,7);
      lineROC[i]->SetLineColor(kRed);

      lineROC_time[i] = new TLine(0,i*2+.5,600,i*2+.5);
      lineROC_time[i]->SetLineColor(kBlack);

      lineROC_cw[i] = new TLine(i*2+.5,0,i*2+.5,15.5);
      lineROC_cw[i]->SetLineColor(kRed);

      for (int iarm=0; iarm<NARMS; iarm++)
	{    
	  TString ROCMap = iarm==0?TString(ROCMap12[i]):TString(ROCMap34[i]);

	  textROC[iarm][i] = new TText(i*2+1.5,0,ROCMap);
	  textROC[iarm][i]->SetTextColor(kMagenta);
	  textROC[iarm][i]->SetTextAlign(12);
	  textROC[iarm][i]->SetTextAngle(90);
	  textROC[iarm][i]->SetTextSize(.05);

	  textROC_time[iarm][i] = new TText(0,i*2+1.5,ROCMap);
	  textROC_time[iarm][i]->SetTextColor(kBlack);
	  textROC_time[iarm][i]->SetTextAlign(12);
	  textROC_time[iarm][i]->SetTextSize(.05);

	  ROCMap = iarm==0?TString(ROCMap12_cw[i]):TString(ROCMap34_cw[i]);

	  textROC_cw[iarm][i] = new TText(i*2+1.5,0,ROCMap);
	  textROC_cw[iarm][i]->SetTextColor(kMagenta);
	  textROC_cw[iarm][i]->SetTextAlign(12);
	  textROC_cw[iarm][i]->SetTextAngle(90);
	  textROC_cw[iarm][i]->SetTextSize(.05);

	}
    }

  for (int i=0; i<13; i++) text_cw_bit[i] = new TText(-2.65,i-0.5,TString(ControlWordBit[i]) );



  ostringstream armStation;
  for (int iarm=0; iarm<NARMS; iarm++) {
    for (int istation=0; istation<NSTATIONS; istation++) {
      //      ptArmStation[iarm][istation] = new TPaveText(0.2, 0.96, 0.3, 0.99,"NDC");
      ptArmStation[iarm][istation] = new TPaveText(0.35, 0.45, 0.65, 0.55,"NDC");
      ptArmStation[iarm][istation]->SetTextSize(0.07);
      ptArmStation[iarm][istation]->SetFillColor(kWhite);
      ptArmStation[iarm][istation]->SetBorderSize(0);
      armStation.str("");
      armStation << CARM[iarm] << " St. " << istation;
      ptArmStation[iarm][istation]->AddText(armStation.str().c_str());
    }
  }
  //  return ;








  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars = new OnlMonDB(ThisName);
  return;
}

InttMonDraw::~InttMonDraw()
{
  delete pieFvtxDisk;
  delete pieROCname;
  delete pieCenter;
  delete tsta0;
  delete tsta1;
  delete tsta2;
  delete tsta3;
  delete line1;
  delete line2;
  delete line_ccs1; 
  delete line_ccs2; 
  delete line_ccs3; 
  delete line_ccs4; 
  delete line_ccs5; 
  delete line_ccs6; 
  delete line3;
  for (int i=0; i<12; i++)
    {
      delete lineROC[i];
      delete lineROC_time[i];
      delete lineROC_cw[i];
      for (int iarm=0; iarm<NARMS; iarm++)
	{
	  delete textROC[iarm][i];
	  delete textROC_cw[iarm][i];
	  delete textROC_time[iarm][i];
	}
    }
  for (int i=0; i<13; i++) delete text_cw_bit[i];

  delete rundb_odbc;

  return;
}

int InttMonDraw::Init()
{
  return 0;
}

void InttMonDraw::registerHists()
{
  OnlMonClient *cl = OnlMonClient::instance();
  m_run_number = cl->RunNumber();
  m_run_type = rundb_odbc->RunType(m_run_number);
  
  cl->registerHisto("fvtxH1NumEvent", "FVTXMON");
  cl->registerHisto(Form("hFvtxEventNumberVsTime"), "FVTXMON");
  cl->registerHisto(Form("hFvtxEventNumberVsTimeShort"), "FVTXMON");
  
  for (int i = 0; i < NARMS; i++) 
    {
      cl->registerHisto(Form("hFvtxYields%d",i), "FVTXMON");
      cl->registerHisto(Form("hFvtxYieldsByPacket%d",i), "FVTXMON");
      cl->registerHisto(Form("hFvtxYieldsByPacketVsTime%d",i), "FVTXMON");
      cl->registerHisto(Form("hFvtxYieldsByPacketVsTimeShort%d",i), "FVTXMON");
      cl->registerHisto(Form("hFvtxControlWord%d",i), "FVTXMON");
      for (int istation=0; istation<12; istation++) 
	{
	  cl->registerHisto(Form("hFvtxDisk%d_%d",i,istation), "FVTXMON");
	  cl->registerHisto(Form("hFvtxAdc%d_%d",i,istation), "FVTXMON");
	}
    }
  for (int iroc = 0; iroc < NROCS; iroc++) 
    {
      cl->registerHisto(Form("hFvtxChipChannel_ROC%d",iroc), "FVTXMON");
    }
  return ;

}

//int InttMonDraw::MakeCanvas(const std::string &name)
int InttMonDraw::MakeCanvas(const char *name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  
  ostringstream tname;
  ostringstream tdesc;

  float xlow = 0.;
  float xup = 0.;
  float ylow = 0.; 
  float yup = 0.;

  for(int icanvas=0; icanvas<ncanvas; icanvas++)
    {
      if( strcmp(name,"ALL") && strcmp(name,canv_name[icanvas].c_str()) ) continue;

      // CANVAS
      TC[icanvas]=new TCanvas(canv_name[icanvas].c_str(),canv_desc[icanvas].c_str(),-1 - (icanvas%2 * xsize/2), 0, xsize/2, ysize);
      gSystem->ProcessEvents();

      // PAD(s)
      for(int ipad=0; ipad<npad[icanvas]; ipad++)
	{
	  tname.str("");
	  tname<<"fvtxPad"<<icanvas<<"_"<<ipad;
	  if(npad[icanvas]==1)
	    {
	      xlow = DISTBORD;
	      ylow = DISTBORD + PADSIZEYBOTTOM;
	      xup = xlow + (PADSIZEX1 - 2 * DISTBORD);
	      yup = ylow + (PADSIZEY1 - 3 * DISTBORD);
	      TP[icanvas][ipad]=new TPad(tname.str().c_str(),"",xlow,ylow,xup,yup,0.);
	    }
	  else if(npad[icanvas]==4) // 4 pads
	    {
	      xlow = DISTBORD + (ipad % 2) * PADSIZEX2;
	      ylow = DISTBORD + (2 - ipad / 2) * PADSIZEY2;
	      xup = xlow + (PADSIZEX2 - 2 * DISTBORD);
	      yup = ylow + (PADSIZEY2 - 3 * DISTBORD);
	      TP[icanvas][ipad]=new TPad(tname.str().c_str(),"",xlow,ylow,xup,yup,0.);
	    }
	  else if(npad[icanvas]==6) // 6 pads
	    {
	      xlow = DISTBORD + (ipad % 2) * PADSIZEX2;
	      ylow = DISTBORD + PADSIZEYBOTTOM + (ipad / 2) * PADSIZEY3;
	      xup = xlow + (PADSIZEX2 - 2 * DISTBORD);
	      yup = ylow + (PADSIZEY3 - 3 * DISTBORD);
	      TP[icanvas][ipad]=new TPad(tname.str().c_str(),"", xlow, ylow, xup, yup, 0.);
	    }
	  TP[icanvas][ipad]->Draw();
	}

      // TRANSPARENT
      tname.str("");
      tname<<"fvtxTrans"<<icanvas;
      transparent[icanvas]=new TPad(tname.str().c_str(),"this does not show",0,0,1,1);
      transparent[icanvas]->SetFillStyle(4000);
      transparent[icanvas]->Draw();
    }

  // if (name == "InttMon1")
  // {
  // // xpos (-1) negative: do not draw menu bar
  //TC[0] = new TCanvas(name.c_str(), "InttMon Example Monitor", -1, 0, xsize / 2, ysize);
  //// root is pathetic, whenever a new TCanvas is created root piles up
  //// 6kb worth of X11 events which need to be cleared with
  //// gSystem->ProcessEvents(), otherwise your process will grow and
  //// grow and grow but will not show a definitely lost memory leak
  //gSystem->ProcessEvents();
  //Pad[0] = new TPad("inttpad1", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
  //Pad[1] = new TPad("inttpad2", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
  //Pad[0]->Draw();
  //Pad[1]->Draw();
  //// this one is used to plot the run number on the canvas
  // transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
  // transparent[0]->SetFillStyle(4000);
  //transparent[0]->Draw();
  //TC[0]->SetEditable(0);
  //}
  //else if (name == "InttMon2")
  //{
  //// xpos negative: do not draw menu bar
  // TC[1] = new TCanvas(name.c_str(), "InttMon2 Example Monitor", -xsize / 2, 0, xsize / 2, ysize);
  // gSystem->ProcessEvents();
  //Pad[2] = new TPad("inttpad3", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
  //Pad[3] = new TPad("inttpad4", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
  //Pad[2]->Draw();
  //Pad[3]->Draw();
  //// this one is used to plot the run number on the canvas
    //transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    //transparent[1]->SetFillStyle(4000);
    //transparent[1]->Draw();
    //TC[1]->SetEditable(0);
  //}
  //else if (name == "InttMon3")
  //{
  //TC[2] = new TCanvas(name.c_str(), "InttMon3 Example Monitor", xsize / 2, 0, xsize / 2, ysize);
  //gSystem->ProcessEvents();
  //Pad[4] = new TPad("inttpad5", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
  //Pad[5] = new TPad("inttpad6", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
  //Pad[4]->Draw();
  //Pad[5]->Draw();
  //// this one is used to plot the run number on the canvas
  ////        transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
  ////        transparent[2]->SetFillStyle(4000);
  ////        transparent[2]->Draw();
  ////      TC[2]->SetEditable(0);
  // }
  return 0;
}

int InttMonDraw::Draw(const std::string &what)
{
  /* int iret = 0;
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
  if (what == "ALL" || what == "HISTORY")
  {
    iret += DrawHistory(what);
    idraw++;
  }
  if (!idraw)
  {
    std::cout << PHWHERE << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;*/

  int iret = 0;
  int idraw = 0;
  string cmd=what;

  // setup, ROOT appearance; keep the old settings and put them back in the end
  TStyle* oldStyle = gStyle;
  TStyle* fvtxStyle = new TStyle(*gStyle);
  gStyle = fvtxStyle;
  gStyle->Reset();

  gStyle->SetOptStat(0);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleH(0.08);

  gStyle->SetTitleXSize(0.045);
  gStyle->SetTitleYSize(0.045);
  gStyle->SetTitleXOffset(0.99);
  gStyle->SetTitleYOffset(0.95);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);

  gStyle->SetPalette(1,0);
  // end of settings

  /*if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxSummary"))
    {
      iret+=DrawSummary(what,0); // Second argument is arm
      iret+=DrawSummary(what,1); // Second argument is arm
      idraw++;
      }*/
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxYields"))
    {
      iret+=DrawYieldsByWedge(what,0);
      iret+=DrawYieldsByWedge(what,1);
      idraw++;
    }
  /*  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxYieldsByPacket"))
    {
      iret+=DrawYieldsByPacket(what,0);
      iret+=DrawYieldsByPacket(what,1);
      idraw++;
    }
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxAdc"))
    {
      iret+=DrawAdc(what,0);
      iret+=DrawAdc(what,1);
      idraw++;
    }
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxChipVsChannelW"))
    {
      iret+=DrawChipVsChannel(what,0,0); // arm, side
      iret+=DrawChipVsChannel(what,1,0); // arm, side
      idraw++;
    }
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxChipVsChannelE"))
    {
      iret+=DrawChipVsChannel(what,0,1); // arm, side
      iret+=DrawChipVsChannel(what,1,1); // arm, side
      idraw++;
    }
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxControlWord"))
    {
      iret+=DrawControlWord(what,0);
      iret+=DrawControlWord(what,1);
      idraw++;
    }
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxYieldsByPacketVsTime"))
    {
      iret+=DrawYieldsByPacketVsTime(what,0);
      iret+=DrawYieldsByPacketVsTime(what,1);
      idraw++;
    }
  if(!strcmp(what.c_str(),"ALL") || !strcmp(what.c_str(),"fvtxYieldsByPacketVsTimeShort"))
    {
      iret+=DrawYieldsByPacketVsTimeShort(what,0);
      iret+=DrawYieldsByPacketVsTimeShort(what,1);
      idraw++;
      }*/
  if (!idraw)
    {
      cout << PHWHERE << " Unimplemented Drawing option: " << what << endl;
      iret = -1;
    }
  // go back to original settings
  gStyle = oldStyle;
  delete fvtxStyle;


  return iret;

}


//int InttMonDraw::DrawFirst(const std::string & /* what */)
/*{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *inttmon_hist1 = cl->getHisto("inttmon_hist1");
  TH1 *inttmon_hist2 = cl->getHisto("inttmon_hist1");
  if (!gROOT->FindObject("InttMon1"))
  {
    MakeCanvas("InttMon1");
  }
  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  Pad[0]->cd();
  if (inttmon_hist1)
  {
    inttmon_hist1->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(0);
    return -1;
  }
  Pad[1]->cd();
  if (inttmon_hist2)
  {
    inttmon_hist2->DrawCopy();
  }
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
  OBtransparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(0);
  return 0;
}*/

//int InttMonDraw::DrawSecond(const std::string & /* what */)
/*
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *inttmon_hist1 = cl->getHisto("inttmon_hist2");
  TH1 *inttmon_hist2 = cl->getHisto("inttmon_hist2");
  if (!gROOT->FindObject("InttMon2"))
  {
    MakeCanvas("InttMon2");
  }
  TC[1]->SetEditable(1);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (inttmon_hist1)
  {
    inttmon_hist1->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(0);
    return -1;
  }
  Pad[3]->cd();
  if (inttmon_hist2)
  {
    inttmon_hist2->DrawCopy();
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(0);
  return 0;
}
*/








int InttMonDraw::DrawYieldsByWedge(const std::string &what, int arm)
{
  OnlMonClient *cl = OnlMonClient::instance();
  TText fvtx_message;
  int eventNum = 0;
  TH1F* fvtxH1NumEvent = (TH1F*)cl->getHisto("fvtxH1NumEvent");
  what.c_str();
  if (arm==0 && ! gROOT->FindObject("fvtxYieldsSouth"))
    {
      MakeCanvas("fvtxYieldsSouth");
    }
  else if (arm==1 && ! gROOT->FindObject("fvtxYieldsNorth"))
    {
      MakeCanvas("fvtxYieldsNorth");
    }

  if (arm==0) TC[YIELDSS]->Clear("D");
  else if (arm==1) TC[YIELDSN]->Clear("D");

  if (fvtxH1NumEvent) {
    eventNum = (int)fvtxH1NumEvent->GetEntries();
    cout << "event Num " << eventNum << endl;
    if (eventNum == 0) {
      cout << "FVTX : no events have yet been processed" << endl;
      if (arm==0) {
	TC[YIELDSS]->Clear();
	fvtx_message.DrawTextNDC(0.3, 0.5, "FVTX.S : No events processed!");
	TC[YIELDSS]->Update();
      }
      else if (arm==1) {
	TC[YIELDSN]->Clear();
	fvtx_message.DrawTextNDC(0.3, 0.5, "FVTX.N : No events processed!");
	TC[YIELDSN]->Update();
      }
      return -1;
    }
    else if (eventNum < FVTXMINEVENTS) {
      cout << "FVTX : not enough events yet" << endl;
      if (arm==0) {
	TC[YIELDSS]->Clear();
	fvtx_message.DrawTextNDC(0.3, 0.5, "FVTX.S : Not enough events!");
	TC[YIELDSS]->Update();
      }
      else if (arm==1) {
	TC[YIELDSN]->Clear();
	fvtx_message.DrawTextNDC(0.3, 0.5, "FVTX.N : Not enough events!");
	TC[YIELDSN]->Update();
      }
      return -1;
    }
  }
  else // Dead server
    {
      if (arm==0) {
	DrawDeadServer(transparent[YIELDSS]);
	TC[YIELDSS]->Update();
      }
      else if (arm==1) {
	DrawDeadServer(transparent[YIELDSN]);
	TC[YIELDSN]->Update();
      }

      return -1;
    }

  if (arm==0) {
    TC[YIELDSS]->cd();
    TP[YIELDSS][0]->cd();
  }
  else if (arm==1) {
    TC[YIELDSN]->cd();
    TP[YIELDSN][0]->cd();
  }
  TH1F* hFvtxYields = (TH1F*)cl->getHisto(Form("hFvtxYields%d",arm));
  TH1F* hFvtxYieldsScaled = (TH1F*)hFvtxYields->Clone("hFvtxYieldsClone");
  hFvtxYieldsScaled->Scale(1.0/eventNum);
  //hFvtxYieldsScaled->SetMaximum(28);//run-14 200 GeV Au+Au
  //hFvtxYieldsScaled->SetMaximum(4);//run-14 200 GeV He+Au
  //hFvtxYieldsScaled->SetMaximum(1.);//run-15 200 GeV p+p
  hFvtxYieldsScaled->SetMaximum(MAXHITSPERWEDGE[arm]);
  hFvtxYieldsScaled->SetFillColor(kGray);
  hFvtxYieldsScaled->SetLineColor(kBlack);
  hFvtxYieldsScaled->SetFillStyle(1001);
  hFvtxYieldsScaled->GetXaxis()->SetNdivisions(8,4,0,kFALSE);
  hFvtxYieldsScaled->DrawCopy("");
  line1->Draw();
  line2->Draw();
  line3->Draw();
  tsta0->Draw();
  tsta1->Draw();
  tsta2->Draw();
  tsta3->Draw();

  //--
  TLine l;
  l.SetLineStyle(2);
  l.DrawLine(-0.5,CUT_HOT_WEDGE_ST0[arm],47.5,CUT_HOT_WEDGE_ST0[arm]);
  l.DrawLine(-0.5,CUT_COLD_WEDGE_ST0[arm],47.5,CUT_COLD_WEDGE_ST0[arm]);
  l.DrawLine(47.5,CUT_HOT_WEDGE_ST1[arm],191.5,CUT_HOT_WEDGE_ST1[arm]);
  l.DrawLine(47.5,CUT_COLD_WEDGE_ST1[arm],191.5,CUT_COLD_WEDGE_ST1[arm]);
  //--

  // run and time info
  time_t t = cl->EventTime();
  ostringstream posting;
  posting.str("");
  posting << "Run# " << cl->RunNumber() << "   Nevt:" << eventNum
	  << "   Date:" <<  ctime(&t);

  if (arm==0) transparent[YIELDSS]->cd();
  else if (arm==1) transparent[YIELDSN]->cd();

  TText printRun;
  printRun.SetTextFont(62);
  printRun.SetTextSize(TXTSIZE*0.7);
  printRun.SetNDC();
  printRun.DrawText(0.11, 0.30, posting.str().c_str());

  if (arm==0) TC[YIELDSS]->Update();
  else if (arm==1) TC[YIELDSN]->Update();

  system("cat fvtxclientlog.txt");

  delete hFvtxYieldsScaled;
  return 0;
}
















int InttMonDraw::DrawDeadServer(TPad *transparentpad)
{
  transparentpad->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();          // set to normalized coordinates
  FatalMsg.SetTextAlign(23);  // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "INTT MONITOR");
  FatalMsg.SetTextAlign(22);  // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21);  // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparentpad->Update();
  return 0;
}

int InttMonDraw::MakePS(const std::string &what)
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

int InttMonDraw::MakeHtml(const std::string &what)
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

  std::string logfile = cl->htmlRegisterPage(*this, "For EXPERTS/Log", "log", "html");
  std::ofstream out(logfile.c_str());
  out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
      << "</TITLE></HEAD>" << std::endl;
  out << "<P>Some log file output would go here." << std::endl;
  out.close();

  std::string status = cl->htmlRegisterPage(*this, "For EXPERTS/Status", "status", "html");
  std::ofstream out2(status.c_str());
  out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
       << "</TITLE></HEAD>" << std::endl;
  out2 << "<P>Some status output would go here." << std::endl;
  out2.close();
  cl->SaveLogFile(*this);
  return 0;
}

int InttMonDraw::DrawHistory(const std::string & /* what */)
{
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  std::vector<float> var;
  std::vector<float> varerr;
  std::vector<time_t> timestamp;
  std::vector<int> runnumber;
  std::string varname = "inttmondummy";
  // this sets the time range from whihc values should be returned
  time_t begin = 0;            // begin of time (1.1.1970)
  time_t end = time(nullptr);  // current time (right NOW)
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << PHWHERE << " Error in db access" << std::endl;
    return iret;
  }
  if (!gROOT->FindObject("InttMon3"))
  {
    MakeCanvas("InttMon3");
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
  Pad[4]->cd();
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

  varname = "inttmoncount";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << PHWHERE << " Error in db access" << std::endl;
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
  Pad[5]->cd();
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
