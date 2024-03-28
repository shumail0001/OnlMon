#include "LocalPolMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TGraphPolar.h>
#include <TGraphPolargram.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2I.h>
#include <TLegend.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TString.h>
#include <TText.h>
#include <TLine.h>
#include <TMath.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector
#include <map>

LocalPolMonDraw::LocalPolMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int LocalPolMonDraw::Init()
{
  iPoint=0;

  //histograms to store the number of counts already taken into account in the graphs
  h_PreviousCounts         = new TH1D*[4];
  h_PreviousCountsScramble = new TH1D*[4];
  
  h_PreviousCounts[0]         = new TH1D("h_PreviousBlueCountsUD","h_BlueCountsUD",4,0,4);
  h_PreviousCounts[1]         = new TH1D("h_PreviousBlueCountsLR","h_BlueCountsLR",4,0,4);
  h_PreviousCounts[2]         = new TH1D("h_PreviousYellCountsUD","h_YellCountsUD",4,0,4);
  h_PreviousCounts[3]         = new TH1D("h_PreviousYellCountsLR","h_YellCountsLR",4,0,4);
  h_PreviousCountsScramble[0] = new TH1D("h_PreviousBlueCountsScrambleUD","h_BlueCountsScrambleUD",4,0,4);
  h_PreviousCountsScramble[1] = new TH1D("h_PreviousBlueCountsScrambleLR","h_BlueCountsScrambleLR",4,0,4);
  h_PreviousCountsScramble[2] = new TH1D("h_PreviousYellCountsScrambleUD","h_YellCountsScrambleUD",4,0,4);
  h_PreviousCountsScramble[3] = new TH1D("h_PreviousYellCountsScrambleLR","h_YellCountsScrambleLR",4,0,4);


  g_Asym         = new TGraphErrors***[2];
  g_AsymScramble = new TGraphErrors***[2];
  TString BeamName[2]={"Blue","Yell"};
  TString MethodName[2]={"Arithmetic","Geometric"};
  TString Orientation[2]={"LR","UD"};
  for(int beam=0; beam<2; beam++){
    g_Asym[beam]         = new TGraphErrors**[2];
    g_AsymScramble[beam] = new TGraphErrors**[2];
    for(int method=0; method<2; method++){
      g_Asym[beam][method]        =new TGraphErrors*[2];
      g_AsymScramble[beam][method]=new TGraphErrors*[2];
      for(int orient=0; orient<2; orient++){
	//Since root badely handle the associated histograms for drawing the frame, ending up in instabilities and crashes
	//Poor alternative for the moment, to create Fresh TGraph each time we need
	g_Asym[beam][method][orient] = nullptr;
	g_AsymScramble[beam][method][orient] = nullptr;
      }
    }
  }
  
  //Equivalent of TH1F frame when drawing a TGraph for polar coordinate system
  gpolargram=new TGraphPolargram("locpolpolargram",0,0.05,-TMath::Pi(),TMath::Pi());
  gpolargram->SetToRadian();
  gpolargram->SetNdivPolar(216);
  gpolargram->SetNdivRadial(105);

  g_Polar = new TGraphPolar**[2];
  for(int beam=0; beam<2; beam++){
    g_Polar[beam] = new TGraphPolar*[2];
    for(int method=0; method<2; method++){
      //For the same reason as above, to prevent random crash inside root, create a fresh object each time we need
      g_Polar[beam][method]=nullptr;
    }
  }
  Pad=new TPad**[2];//Who is taking care of the deletion?
  Pad[0]=new TPad*[16];
  Pad[1]=new TPad*[4];

  NewPoint=false;
  OnlMonClient *cl = OnlMonClient::instance();
  myRun=cl->RunNumber();
  return 0;
}

int LocalPolMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "LocalPolMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "LocalPolMon Asymmetries", -xsize /2.5, 0, xsize / 2.5, ysize*0.9);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for(int ipad=0; ipad<16; ipad++){
      Pad[0][ipad] = new TPad(Form("locpolpad0%d",ipad), "who needs this?", 0.0+0.25*(ipad%4), 0.95-0.2375*(ipad/4+1), 0.0+0.25*(ipad%4+1), 0.95-0.2375*(ipad/4), 0);
      Pad[0][ipad]->SetLeftMargin(0.15);
      Pad[0][ipad]->SetBottomMargin(0.15);
      Pad[0][ipad]->Draw();
    }

    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("locpoltransparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "LocalPolMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "LocalPolMon Polarisation direction", -xsize / 2.5, 0, xsize / 2.5, ysize);
    gSystem->ProcessEvents();
    for(int ipad=0; ipad<4; ipad++){
      Pad[1][ipad] = new TPad(Form("locpolpad1%d",ipad),"who needs this?", 0.0+0.5*(ipad%2),0.95-0.475*(ipad/2+1),0.0+0.5*(ipad%2+1),0.95-0.475*(ipad/2),0);
      Pad[1][ipad]->SetTopMargin(0.15);
      Pad[1][ipad]->Draw();
    }

    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("locpoltransparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  return 0;
}

int LocalPolMonDraw::Draw(const std::string &what)
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
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int LocalPolMonDraw::DrawFirst(const std::string & /* what */)
{

  
  OnlMonClient *cl = OnlMonClient::instance();

  time_t evttime = cl->EventTime("CURRENT");
  TH1D* h_Counts[4];
  TH1D* h_CountsScramble[4];
  h_Counts[0] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_BlueCountsUD");
  h_Counts[1] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_BlueCountsLR");
  h_Counts[2] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_YellCountsUD");
  h_Counts[3] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_YellCountsLR");

  h_CountsScramble[0] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_BlueCountsScrambleUD");
  h_CountsScramble[1] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_BlueCountsScrambleLR");
  h_CountsScramble[2] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_YellCountsScrambleUD");
  h_CountsScramble[3] = (TH1D*)cl->getHisto("LOCALPOLMON_0","h_YellCountsScrambleLR");

  if (!gROOT->FindObject("LocalPolMon1"))
  {
    MakeCanvas("LocalPolMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  gStyle->SetOptStat(0);
  //gStyle->SetLabelSize(0.5,"X");
  bool IsGood=true;
  for(int i=0; i<4; i++){
    if (!h_Counts[i]){
      IsGood=false;
      std::cout<<"Histogram h_Counts["<<i<<"] is not defined"<<std::endl;
    }
    if (!h_CountsScramble[i]){
      IsGood=false;
      std::cout<<"Histogram h_CountsScramble["<<i<<"] is not defined"<<std::endl;
    }
  }
  if(!IsGood){
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }
  if(h_Counts[0]->GetEntries()-h_PreviousCounts[0]->GetEntries()>thresholdNewPoint){//To be tuned by config file
    NewPoint=true;
  }
  else if (myRun!=cl->RunNumber()){
    std::cout<<"New Run or new Files, reset counter"<<std::endl;
    for(int ihisto=0; ihisto<4; ihisto++){
      h_PreviousCounts[ihisto]->Reset();
      h_PreviousCountsScramble[ihisto]->Reset();
    }
    myRun=cl->RunNumber();
  }
  else{
    std::cout<<"Currently only "<<h_Counts[0]->GetEntries()-h_PreviousCounts[0]->GetEntries() <<" hits, no accurate measurement "<<std::endl;
  }

  for(int ibeam=0; ibeam<2; ibeam++){
    for(int orient=0; orient<2; orient++){

      double L_U = h_Counts[2*ibeam+orient]->GetBinContent(1) - h_PreviousCounts[2*ibeam+orient]->GetBinContent(1);
      double R_D = h_Counts[2*ibeam+orient]->GetBinContent(2) - h_PreviousCounts[2*ibeam+orient]->GetBinContent(2);
      double L_D = h_Counts[2*ibeam+orient]->GetBinContent(3) - h_PreviousCounts[2*ibeam+orient]->GetBinContent(3);
      double R_U = h_Counts[2*ibeam+orient]->GetBinContent(4) - h_PreviousCounts[2*ibeam+orient]->GetBinContent(4);
      
      double* asymresult = ComputeAsymmetries(L_U, R_D, L_D, R_U);
      if(!asymresult[1]) continue;//Most likely only needed for offline use, we have a new point but no additional data=> so let us not add this dummy zero here
      m_time[iPoint]=evttime;
      m_asym[ibeam][0][orient][iPoint]=asymresult[0];
      m_easym[ibeam][0][orient][iPoint]=asymresult[1];
	
      m_asym[ibeam][1][orient][iPoint]=asymresult[2];
      m_easym[ibeam][1][orient][iPoint]=asymresult[3];

      
      L_U = h_CountsScramble[2*ibeam+orient]->GetBinContent(1) - h_PreviousCountsScramble[2*ibeam+orient]->GetBinContent(1);
      R_D = h_CountsScramble[2*ibeam+orient]->GetBinContent(2) - h_PreviousCountsScramble[2*ibeam+orient]->GetBinContent(2);
      L_D = h_CountsScramble[2*ibeam+orient]->GetBinContent(3) - h_PreviousCountsScramble[2*ibeam+orient]->GetBinContent(3);
      R_U = h_CountsScramble[2*ibeam+orient]->GetBinContent(4) - h_PreviousCountsScramble[2*ibeam+orient]->GetBinContent(4);
      
      double* scrambleresult = ComputeAsymmetries(L_U, R_D, L_D, R_U);
	
      m_fasym[ibeam][0][orient][iPoint]=scrambleresult[0];
      m_efasym[ibeam][0][orient][iPoint]=scrambleresult[1];
	
      m_fasym[ibeam][1][orient][iPoint]=scrambleresult[2];
      m_efasym[ibeam][1][orient][iPoint]=scrambleresult[3];

      delete asymresult;
      delete scrambleresult;
    }
  }
  TString BeamName[2]={"Blue","Yell"};
  TString MethodName[2]={"Arithmetic","Geometric"};
  TString Orientation[2]={"LR","UD"};
  for(int ibeam=0; ibeam<2; ibeam++){
    for(int orient=0; orient<2; orient++){
      for(int method=0; method<2; method++){
	if(g_Asym[ibeam][method][orient]){
	  delete g_Asym[ibeam][method][orient];
	  g_Asym[ibeam][method][orient]=nullptr;
	}
	g_Asym[ibeam][method][orient]= new TGraphErrors();
	g_Asym[ibeam][method][orient]->SetName(Form("g_Asym%s%s%s",BeamName[ibeam].Data(),MethodName[method].Data(),Orientation[orient].Data()));
	g_Asym[ibeam][method][orient]->SetMarkerStyle(kFullCircle);
	g_Asym[ibeam][method][orient]->SetMarkerColor(kBlue);
	g_Asym[ibeam][method][orient]->SetLineColor(kBlue);
	g_Asym[ibeam][method][orient]->SetTitle(Form("%s %s %s Asym.",BeamName[ibeam].Data(),Orientation[orient].Data(),MethodName[method].Data()));

	if(g_AsymScramble[ibeam][method][orient]){
	  delete g_AsymScramble[ibeam][method][orient];
	  g_AsymScramble[ibeam][method][orient]=nullptr;
	}
	g_AsymScramble[ibeam][method][orient] = new TGraphErrors();
	g_AsymScramble[ibeam][method][orient]->SetName(Form("g_AsymScramble%s%s%s",BeamName[ibeam].Data(),MethodName[method].Data(),Orientation[orient].Data()));
	g_AsymScramble[ibeam][method][orient]->SetMarkerStyle(kFullCircle);
	g_AsymScramble[ibeam][method][orient]->SetMarkerColor(kGray);
	g_AsymScramble[ibeam][method][orient]->SetLineColor(kGray);
	g_AsymScramble[ibeam][method][orient]->SetTitle(Form("Unpol %s %s %s Asym.",BeamName[ibeam].Data(),Orientation[orient].Data(),MethodName[method].Data()));
	int N=m_time.size();
	for(int i=0; i<N; i++){
	  g_Asym[ibeam][method][orient]->SetPoint(i,m_time[i],m_asym[ibeam][method][orient][i]);
	  g_Asym[ibeam][method][orient]->SetPointError(i,0,m_easym[ibeam][method][orient][i]);
	  
	  g_AsymScramble[ibeam][method][orient]->SetPoint(i,m_time[i],m_fasym[ibeam][method][orient][i]);
	  g_AsymScramble[ibeam][method][orient]->SetPointError(i,0,m_efasym[ibeam][method][orient][i]);
	}
      }
    }
  }
  long int start=min_element(m_time.begin(),m_time.end(),[](const std::pair<int, long int>&lhs, const std::pair<int, long int>&rhs){return lhs.second<rhs.second;})->second;
  long int stop =max_element(m_time.begin(),m_time.end(),[](const std::pair<int, long int>&lhs, const std::pair<int, long int>&rhs){return lhs.second<rhs.second;})->second;
  
  if(stop-start<15*60){//less than 15 min, we set it to 15min
    stop=start+15*60;
  }
  else{
    stop=stop+60;//we add 1 min
  }
  start=start-60;
  
  if(hframe){
    delete hframe;
    hframe=nullptr;
  }
  hframe=new TH1F("locpolframe","",100,start,stop);
  hframe->SetStats(kFALSE);

  if(hscrambleframe){
    delete hscrambleframe;
    hscrambleframe=nullptr;
  }
  hscrambleframe=new TH1F("locpolscrambleframe","",100,start,stop);
  hscrambleframe->SetStats(kFALSE);
  
  if(NewPoint){
    NewPoint=false;
    iPoint++;
    for(int i=0; i<4; i++){
      h_PreviousCounts[i]->Add(h_Counts[i]);
      h_PreviousCountsScramble[i]->Add(h_CountsScramble[i]);
    }
  }
  gStyle->SetTitleFontSize(0.1);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(kWhite);
  gROOT->ForceStyle();
  TLine l;
  TLine lref;
  lref.SetLineColor(kRed);
  TText PrintTitle;
  PrintTitle.SetTextFont(62);
  PrintTitle.SetTextSize(0.06);
  PrintTitle.SetNDC();          // set to normalized coordinates
  PrintTitle.SetTextAlign(23);  // center/top alignment
  for(int ibeam=0; ibeam<2; ibeam++){
    for(int orient=0; orient<2; orient++){
      for(int method=0; method<2; method++){
	Pad[0][8*ibeam+2*method+orient]->cd();
	hframe->Draw();
	hframe->GetXaxis()->SetTimeDisplay(1);
	hframe->GetXaxis()->SetTimeFormat("%H:%M%F1970-01-01 00:00:00s0");
	hframe->GetXaxis()->SetNdivisions(507);
	hframe->GetXaxis()->SetTitle("Time (hh:mm)");
	hframe->GetXaxis()->SetTitleSize(0.06);
	hframe->GetXaxis()->SetTitleOffset(1.1);
	hframe->GetXaxis()->SetLabelSize(0.06);
	hframe->GetXaxis()->SetLabelOffset(0.025);
	
	hframe->GetYaxis()->SetTitle("Asymmetry");
	hframe->GetYaxis()->SetTitleSize(0.06);
	hframe->GetYaxis()->SetTitleOffset(1.1);
	hframe->GetYaxis()->CenterTitle(true);
	hframe->GetYaxis()->SetRangeUser(-0.1,0.1);
	hframe->GetYaxis()->SetLabelSize(0.06);
	l.   DrawLine(start,0.00,stop,0.00);
	lref.DrawLine(start,0.01,stop,0.01);
	lref.DrawLine(start,0.02,stop,0.02);
	g_Asym[ibeam][method][orient]->Draw("epsame");
	PrintTitle.DrawText(0.5, 0.95, Form("%s %s %s Asym.",BeamName[ibeam].Data(),Orientation[orient].Data(),MethodName[method].Data()));

	Pad[0][8*ibeam+2*method+orient+4]->cd();
	hscrambleframe->Draw();
	hscrambleframe->GetXaxis()->SetTimeDisplay(1);
	hscrambleframe->GetXaxis()->SetTimeFormat("%H:%M%F1970-01-01 00:00:00s0");
	hscrambleframe->GetXaxis()->SetNdivisions(507);
	hscrambleframe->GetXaxis()->SetTitle("Time (hh:mm)");
	hscrambleframe->GetXaxis()->SetTitleSize(0.06);
	hscrambleframe->GetXaxis()->SetTitleOffset(1.1);
	hscrambleframe->GetXaxis()->SetLabelSize(0.06);
	hscrambleframe->GetXaxis()->SetLabelOffset(0.025);
	
	hscrambleframe->GetYaxis()->SetTitle("Fake Asymmetry");
	hscrambleframe->GetYaxis()->SetTitleSize(0.06);
	hscrambleframe->GetYaxis()->SetTitleOffset(1.1);
	hscrambleframe->GetYaxis()->CenterTitle(true);
	hscrambleframe->GetYaxis()->SetRangeUser(-0.1,0.1);
	hscrambleframe->GetYaxis()->SetLabelSize(0.06);
	l.   DrawLine(start,0,stop,0);
	lref.DrawLine(start,-0.01,stop,-0.01);
	lref.DrawLine(start, 0.01,stop,0.01);
	g_AsymScramble[ibeam][method][orient]->Draw("epsame");
	PrintTitle.DrawText(0.5, 0.95, Form("Unpol. %s %s %s Asym.",BeamName[ibeam].Data(),Orientation[orient].Data(),MethodName[method].Data()));
      }
    }
  }
  

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);
  return 0;
}

int LocalPolMonDraw::DrawSecond(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  if (!gROOT->FindObject("LocalPolMon2"))
  {
    MakeCanvas("LocalPolMon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  TC[1]->Update();

  TString BeamName[2]={"Blue","Yell"};
  TString MethodName[2]={"Arithmetic","Geometric"};
  TString Orientation[2]={"LR","UD"};
  for(int beam=0; beam<2; beam++){
    for(int method=0; method<2; method++){
      if(g_Polar[beam][method]){
	delete g_Polar[beam][method];
	g_Polar[beam][method]=nullptr;
      }
      g_Polar[beam][method]=new TGraphPolar();
      g_Polar[beam][method]->SetName(Form("g_Polar%s%s",BeamName[beam].Data(),MethodName[method].Data()));
      g_Polar[beam][method]->SetMarkerStyle(kFullCircle);
      g_Polar[beam][method]->SetMarkerColor(kRed);
      g_Polar[beam][method]->SetLineColor(kRed);
      g_Polar[beam][method]->SetPoint(0,0,0);
      g_Polar[beam][method]->SetPolargram(gpolargram);
    }
  }


  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(kWhite);
  gROOT->ForceStyle();
  TText PrintTitle;
  PrintTitle.SetTextFont(62);
  PrintTitle.SetTextSize(0.06);
  PrintTitle.SetNDC();          // set to normalized coordinates
  PrintTitle.SetTextAlign(23);  // center/top alignment
  for(int ibeam=0; ibeam<2; ibeam++){
    for(int method=0; method<2; method++){
      int N=m_asym[ibeam][method][0].size();
      for(int i=0; i<N; i++){
  	double x=m_asym[ibeam][method][1][i];//left right
  	double y=m_asym[ibeam][method][0][i];//top-bottom
  	double ex=m_asym[ibeam][method][1][i];
  	double ey=m_asym[ibeam][method][0][i];
  	double theta=atan2(y,x);
  	double radius=sqrt(x*x+y*y);
	if(radius<1e-7){
	  radius=1e-7;
	}
  	double etheta=sqrt(y*y*ex*ex+x*x*ey*ey)/pow(radius,2);
  	double eradius=sqrt(x*x*ex*ex+y*y*ey*ey)/radius;
  	g_Polar[ibeam][method]->SetPoint(i+1,theta,radius);
  	g_Polar[ibeam][method]->SetPointError(i+1,etheta,eradius);
      }
      Pad[1][2*method+ibeam]->cd();
      if(gPad->FindObject("polargram"))   std::cout<<"polargram found\n";
      if(gPad->FindObject("locpolpolargram")) std::cout<<"mypolagram found\n";
      g_Polar[ibeam][method]->GetPolargram()->Draw();
      g_Polar[ibeam][method]->Draw("epsame");
      PrintTitle.DrawText(0.15,0.98,Form("%s %s",BeamName[ibeam].Data(),MethodName[method].Data()));
      Pad[1][2*method+ibeam]->Update();
    }
  }

  TC[1]->Update();
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
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);
  return 0;
}

int LocalPolMonDraw::SavePlot(const std::string &what, const std::string &type)
{

  OnlMonClient *cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)  // on error no png files please
  {
      return iret;
  }
  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    std::string filename = ThisName + "_" + std::to_string(icnt) + "_" +
      std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(canvas, filename);
  }
  return 0;
}

int LocalPolMonDraw::MakeHtml(const std::string &what)
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



double* LocalPolMonDraw::ComputeAsymmetries(double L_U, double R_D, double L_D, double R_U){
  double* result = new double[4];
      double leftA  = L_U+R_D;
      double rightA = L_D+R_U;
      double tmpNumA = leftA-rightA;
      double tmpDenA = leftA+rightA;
      result[0] = 0;
      result[1] = 0;
	
      if(tmpDenA>0){
	result[0] = tmpNumA/tmpDenA;
	result[1] = 2*sqrt(pow(rightA,2)*leftA+pow(leftA,2)*rightA)/pow(tmpDenA,2);      
      }
	
      double leftG = sqrt(L_U*R_D);
      double rightG= sqrt(L_D*R_U);
      double tmpNumG = leftG-rightG;
      double tmpDenG = leftG+rightG;
      result[2] = 0;
      result[3] = 0;
	
      if(tmpDenG>0){
	result[2] = tmpNumG/tmpDenG;
	result[3] = sqrt(pow(rightG,2)*leftA+pow(leftG,2)*rightA)/pow(tmpDenG,2);
      }
      return result;
}
