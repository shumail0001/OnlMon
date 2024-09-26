#include "LL1MonDraw.h"

#include <onlmon/OnlMonClient.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
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

LL1MonDraw::LL1MonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int LL1MonDraw::Init()
{
  return 0;
}

int LL1MonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();

  gStyle->SetOptStat(0);
  if (name == "LL1Mon0")
  {
    int canvasindex = 0;
    std::string tctitle = name + " Monitor - MBD";
    // xpos (-1) negative: do not draw menu bar
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize/2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for(int ipad=0; ipad<nPad1; ipad++){
      Pad[ipad] = new TPad(Form("ll1pad%d",ipad),Form("ll1pad%d",ipad),padx1[ipad],pady1[ipad],padx2[ipad],pady2[ipad],0);
      Pad[ipad]->Draw();
      Pad[ipad]->SetTicks(1,1);
    }
    transparent[canvasindex] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  else if (name == "LL1Mon1")
  {
    int canvasindex = 1;
    std::string tctitle = name + " Monitor - MBD";
    // xpos negative: do not draw menu bar
    gStyle->SetOptStat(0);
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize / 2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(false);
  }
  else if (name == "LL1Mon2")
  {
    int canvasindex = 2;
    std::string tctitle = name + " Monitor - MBD";
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize / 2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas
    transparent[canvasindex] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(0);
  }
  else if (name == "LL1Mon3")
  {
    int canvasindex = 3;
    std::string tctitle = name + " Monitor - Photon";
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize / 2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    gSystem->ProcessEvents();

    transparent[canvasindex] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(0);

  }
  else if (name == "LL1Mon4")
  {
    int canvasindex = 4;
    std::string tctitle = name + " Monitor - HCAL";
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize / 2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas


    transparent[canvasindex] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(0);
  }
  else if (name == "LL1Mon5")
  {
    int canvasindex = 5;
    std::string tctitle = name + " Monitor - Jet";
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize / 2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    gSystem->ProcessEvents();
    // 
    transparent[canvasindex] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(0);
  }
  else if (name == "LL1Mon6")
  {
    int canvasindex = 6;
    std::string tctitle = name + " Monitor - EMCAL 2x2";
    TC[canvasindex] = new TCanvas(name.c_str(), tctitle.c_str(), -1, 0, xsize / 2, ysize/2);
    TC[canvasindex]->SetTicks(1,1);
    gSystem->ProcessEvents();
    // 
    float topbrim = 0.96;
    float dx = 0.25;
    float dy = topbrim / 4.;
    // this one is used to plot the run number on the canvas
    for (int i = 0; i < 16; i++)
      {
	std::string padname = "pad_emcal_"+std::to_string(i);
	int y = 3 - (i/4);
	int x = i%4;
	emcalpad[i] = new TPad(padname.c_str(),"", x*dx, y*dy,(x+1)*dx, (y+1)*dy); 
	emcalpad[i]->Draw();
      }
    transparent[canvasindex] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
    transparent[canvasindex]->SetFillStyle(4000);
    transparent[canvasindex]->Draw();
    TC[canvasindex]->SetEditable(0);
  }

  return 0;
}

int LL1MonDraw::Draw(const std::string &what)
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
  if (what == "ALL" || what == "THIRD")
  {
    iret += DrawThird(what);
    idraw++;
  }
  if (what == "ALL" || what == "FOURTH")
  {
    iret += DrawFourth(what);
    idraw++;
  }
  if (what == "ALL" || what == "FIFTH")
  {
    iret += DrawFifth(what);
    idraw++;
  }
  if (what == "ALL" || what == "SIXTH")
  {
    iret += DrawSixth(what);
    idraw++;
  }

  if (what == "ALL" || what == "SEVENTH")
  {
    iret += DrawSeventh(what);
    idraw++;
  }

  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int LL1MonDraw::DrawFirst(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 0;
  if (!gROOT->FindObject("LL1Mon0"))
  {
    MakeCanvas("LL1Mon0");
  }
// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");

  OnlMonClient *cl = OnlMonClient::instance();
  TH1 *h_nhit_n1 = cl->getHisto("LL1MON_0","h_nhit_n1");
  TH1 *h_nhit_n2 = cl->getHisto("LL1MON_0","h_nhit_n2");
  TH1 *h_nhit_s1 = cl->getHisto("LL1MON_0","h_nhit_s1");
  TH1 *h_nhit_s2 = cl->getHisto("LL1MON_0","h_nhit_s2");
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  Pad[0]->cd();
  if (h_nhit_n1)
  {
    h_nhit_n1->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(false);
    return -1;
  }
  Pad[1]->cd();
  h_nhit_n2->DrawCopy();
  Pad[2]->cd();
  h_nhit_s1->DrawCopy();
  Pad[3]->cd();
  h_nhit_s2->DrawCopy();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_1 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  gStyle->SetOptStat(0);
  TC[canvasindex]->Update();
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int LL1MonDraw::DrawSecond(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 1;
  if (!gROOT->FindObject("LL1Mon1"))
  {
    MakeCanvas("LL1Mon1");
  }

// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");

  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_nhit_corr= (TH2D*) cl->getHisto("LL1MON_0","h_nhit_corr");
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  if (h_nhit_corr)
  {
    h_nhit_corr->Draw("colz");
  }
  else
  {
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(false);
    return -1;
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_2 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Update();
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int LL1MonDraw::DrawThird(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 2;
  if (!gROOT->FindObject("LL1Mon2"))
  {
    MakeCanvas("LL1Mon2");
  }

// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");
  gStyle->SetOptStat(0);

  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_line_up= (TH2*) cl->getHisto("LL1MON_0","h_line_up");
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  if (h_line_up)
  {
    h_line_up->Draw("colz");
  }
  else
  {
    DrawDeadServer(transparent[canvasindex]);
    TC[canvasindex]->SetEditable(false);
    return -1;
  }
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[canvasindex]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int LL1MonDraw::DrawFourth(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 3;

  if (!gROOT->FindObject("LL1Mon3"))
  {
    MakeCanvas("LL1Mon3");
  }

// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true); 
  TC[canvasindex]->Clear("D");
  gStyle->SetOptStat(0);

  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_8x8_sum_emcal= (TH2*) cl->getHisto("LL1MON_0","h_8x8_sum_emcal");
  if (!h_8x8_sum_emcal)
   {
     DrawDeadServer(transparent[canvasindex]);
     TC[canvasindex]->SetEditable(false);
     return -1;
   }
  TH2 *h_8x8_sum_emcal_above_threshold= (TH2*) cl->getHisto("LL1MON_0","h_8x8_sum_emcal_above_threshold_0");
  if (!h_8x8_sum_emcal_above_threshold)
  {
      DrawDeadServer(transparent[canvasindex]);
      TC[canvasindex]->SetEditable(false);
      return -1;
  }
  TH2 *h_sample_diff_emcal= (TH2*) cl->getHisto("LL1MON_0","h_sample_diff_emcal");
  if (!h_sample_diff_emcal)
   {
     DrawDeadServer(transparent[canvasindex]);
     TC[canvasindex]->SetEditable(false);
     return -1;
   }
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");

  TC[canvasindex]->cd();
  TPad *left_top_pad = new TPad("left_top_pad", "", 0.0, 0.45, 0.5, 0.9);
  left_top_pad->SetTicks(1,1);
  left_top_pad->Draw();
  left_top_pad->cd();
 
  h_8x8_sum_emcal->SetTitle("EMCAL 8x8 Total Sums ; #eta <8x8> sum; #phi <8x8> sum"); 
  h_8x8_sum_emcal->Draw("colz");
 
  TC[canvasindex]->cd();
  TPad *top_right_pad = new TPad("top_right_pad", "", 0.5, 0.45, 1.0, 0.9);
  top_right_pad->SetTicks(1,1);
  top_right_pad->Draw();
  top_right_pad->SetLogz();
  top_right_pad->cd();

  h_8x8_sum_emcal_above_threshold->SetTitle("EMCAL 8x8 Sums Above Threshold;#eta <8x8> sum; #phi <8x8> sum");
  h_8x8_sum_emcal_above_threshold->Draw("colz");
  
  TC[canvasindex]->cd();
  TPad *bottom_left_pad = new TPad("bottom_left_pad", "", 0.0, 0.0, 0.5, 0.450);
  bottom_left_pad->SetTicks(1,1);
  bottom_left_pad->SetTopMargin(0.2);
  bottom_left_pad->Draw();

  bottom_left_pad->SetLogz();
  bottom_left_pad->cd();

  h_sample_diff_emcal->SetTitle("EMCAL Output Timing; Relative BC of L1 Trigger; EMCAL Board");
  h_sample_diff_emcal->Draw("colz");
  
  TC[canvasindex]->cd();

  //Calculate 8x8 EMCal sum excessing 10% of the total integral 
  int nbinsx = h_8x8_sum_emcal_above_threshold->GetNbinsX();
  int nbinsy = h_8x8_sum_emcal_above_threshold->GetNbinsY();
  float integral = h_8x8_sum_emcal_above_threshold->Integral();
  std::vector<std::tuple<int,int,float>> emcalhotbin;

  for(int ibinx = 1; ibinx<=nbinsx; ibinx++){
      for(int ibiny=1; ibiny<=nbinsy; ibiny++){
          float con = h_8x8_sum_emcal_above_threshold->GetBinContent(ibinx,ibiny);
          if(con > integral*0.1) emcalhotbin.push_back(std::make_tuple(ibinx,ibiny,con/integral));
      }
  }
          
  TPad *bottom_right_pad = new TPad("bottom_right_pad", "", 0.5, 0.0, 1.0, 0.450);
  bottom_right_pad->SetTicks(1,1);
  bottom_right_pad->SetTopMargin(0.2);
  bottom_right_pad->Draw();
  bottom_right_pad->cd();

  double textsize = 0.04;
  TLatex PrintHot;
  //PrintHot.SetTextFont(40);
  PrintHot.SetTextSize(textsize);
  PrintHot.SetNDC();          // set to normalized coordinates
  PrintHot.SetTextAlign(23);  // center/top alignment
  std::ostringstream hotchannelstream;
  std::string hotstring;
  
  float y_position = 0.9;
  float x_position = 0.4;
  
  hotchannelstream << "8x8 Photon Trigger - Hot Trigger Areas : Total " << emcalhotbin.size() << std::endl;
  hotstring = hotchannelstream.str();
  PrintHot.DrawText(x_position, y_position, hotstring.c_str());
  y_position -=0.06;

  for (auto& entry : emcalhotbin) {
      int etabin = std::get<0>(entry);
      int phibin = std::get<1>(entry);
      float frac = std::get<2>(entry)*100.;

      std::string text = "#eta bin : " + std::to_string(etabin) + ", #phi bin : " + std::to_string(phibin) + Form(", fraction : %.1f",frac) + " (%)";
      PrintHot.DrawLatex(x_position, y_position, text.c_str());
      y_position -= (textsize * 1.2);
      if (y_position < 0) break; 
  }

  TC[canvasindex]->cd();
  TPad *tr = new TPad("tr", "this does not show", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();
  tr->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << "EMCAL 8x8 Photon Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}
int LL1MonDraw::DrawFifth(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 4;
  if (!gROOT->FindObject("LL1Mon4"))
  {
    MakeCanvas("LL1Mon4");
  }

// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");
  gStyle->SetOptStat(0);

  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_jet_input= (TH2*) cl->getHisto("LL1MON_0","h_jet_input");
  if (!h_jet_input)
    {
      DrawDeadServer(transparent[canvasindex]);
      TC[canvasindex]->SetEditable(false);
      return -1;
    }
  

  TH2 *h_sample_diff_jet_input = (TH2*) cl->getHisto("LL1MON_0","h_sample_diff_jet_input");
  if (!h_sample_diff_jet_input)
    {
      DrawDeadServer(transparent[canvasindex]);
      TC[canvasindex]->SetEditable(false);
      return -1;
    }

  TH1 *h_hit_format = (TH1*) cl->getHisto("LL1MON_0", "h_hit_format");
  if (!h_hit_format)
    {
      DrawDeadServer(transparent[canvasindex]);
      TC[canvasindex]->SetEditable(false);
      return -1;
    }

  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");

  TC[canvasindex]->cd();
  TPad *left_pad = new TPad("left_pad", "", 0.0, 0.0,0.5, 0.9);
  left_pad->SetTicks(1,1);
  left_pad->Draw();
  left_pad->cd();
  left_pad->SetLogz();


  std::string hf_name = "HCAL";
  int hitformat = static_cast<int>(h_hit_format->GetBinCenter(h_hit_format->GetMaximumBin()));
  if (hitformat%2 == 0)
    {
      hf_name = "EMCAL";
    }
  std::string title = "Jet Input (" + hf_name + ") 2x2 Tower Sums; #eta ; #phi";
  h_jet_input->SetTitle("Jet Input (HCAL) 2x2 Tower Sums; #eta ; #phi");
  h_jet_input->Draw("colz");
  
  TC[canvasindex]->cd();
  TPad *right_pad = new TPad("right_pad", "", 0.5, 0.0,1.0, 0.9);
  right_pad->SetTicks(1,1);
  right_pad->Draw();
  right_pad->cd();
  right_pad->SetLogz();
 
  h_jet_input->SetTitle("Jet Input (HCAL); #eta 2x2 Sum ; #phi 2x2 Sum");
  h_sample_diff_jet_input->Draw("colz");
 

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TC[canvasindex]->cd();
  TPad *tr = new TPad("tr", "this does not show", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();
  tr->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int LL1MonDraw::DrawSixth(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 5;
  if (!gROOT->FindObject("LL1Mon5"))
  {
    MakeCanvas("LL1Mon5");
  }

// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");
  gStyle->SetOptStat(0);

  OnlMonClient *cl = OnlMonClient::instance();
  TH2 *h_jet_output= (TH2*) cl->getHisto("LL1MON_0","h_jet_output");
  if (!h_jet_output)
    {
      DrawDeadServer(transparent[canvasindex]);
      TC[canvasindex]->SetEditable(false);
      return -1;
    }
  h_jet_output->SetTitle("Jet <8x8> Overlapping Total Sums; #eta <8x8> Sum; #phi <8x8> Sum");
  TH2 *h_jet_output_above_threshold= (TH2*) cl->getHisto("LL1MON_0","h_jet_output_above_threshold_0");
  if (!h_jet_output_above_threshold)
    {
      DrawDeadServer(transparent[canvasindex]);
      TC[canvasindex]->SetEditable(false);
      return -1;
    }
  h_jet_output_above_threshold->SetTitle("Jet <8x8> Overlapping Hits above Threshold; #eta <8x8> Sum; #phi <8x8> Sum");
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");

  transparent[canvasindex]->cd();
  TPad *left_pad = new TPad("left_pad", "", 0.0, 0.0, 0.5, 0.9);
  left_pad->SetTicks(1,1);
  left_pad->Draw();
  left_pad->cd();

  h_jet_output->Draw("colz");
  
  TC[canvasindex]->cd();
  TPad *right_pad = new TPad("right_pad", "", 0.5, 0.0, 1.0, 0.9);
  right_pad->SetTicks(1,1);
  right_pad->Draw();
  right_pad->SetLogz();
  right_pad->cd();

  h_jet_output_above_threshold->Draw("colz");
  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TC[canvasindex]->cd();
  TPad *tr = new TPad("tr", "this does not show", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();
  tr->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int LL1MonDraw::DrawSeventh(const std::string & /* what */)
{
  // index of TC and transparent TPad array 
  // so we don't make cut and paste errors
  int canvasindex = 6;
  if (!gROOT->FindObject("LL1Mon6"))
  {
    MakeCanvas("LL1Mon6");
  }

// set canvas to editable before the dead server drawing otherwise
// canvas will not be updated
  TC[canvasindex]->SetEditable(true);
  TC[canvasindex]->Clear("D");
  gStyle->SetOptStat(0);

  OnlMonClient *cl = OnlMonClient::instance();
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");

  for (int i = 0 ; i < 16; i++)
    {
      std::string histname = "h_2x2_sum_emcal_" + std::to_string(i);
      TH1 *h_emcal= (TH1*) cl->getHisto("LL1MON_0",histname.c_str());

      if (!h_emcal)
	{
	  DrawDeadServer(transparent[canvasindex]);
	  TC[canvasindex]->SetEditable(false);
	  return -1;
	}
      emcalpad[i]->cd();
      h_emcal->SetLineColor(kBlack);
      h_emcal->SetLineWidth(2);
      h_emcal->SetFillColor(kSpring + 2);
      h_emcal->Draw();
    }
  
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  // fill run number and event time into string
  runnostream << ThisName << "_3 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  TC[canvasindex]->cd();
  TPad *tr = new TPad("tr", "this does not show", 0, 0, 1, 1);
  tr->SetFillStyle(4000);
  tr->Draw();
  tr->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[canvasindex]->Update();
  TC[canvasindex]->SetTicks(1,1);
  TC[canvasindex]->Show();
  TC[canvasindex]->SetEditable(false);
  return 0;
}

int LL1MonDraw::SavePlot(const std::string &what, const std::string &type)
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

int LL1MonDraw::MakeHtml(const std::string &what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient *cl = OnlMonClient::instance();

  int icnt = 0;
  for (TCanvas *canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    icnt++;
    // Register the canvas png file to the menu and produces the png file.
    std::string pngfile = cl->htmlRegisterPage(*this, canvas->GetTitle(), std::to_string(icnt), "png");
    cl->CanvasToPng(canvas, pngfile);
  }
  // Now register also EXPERTS html pages, under the EXPERTS subfolder.

  // std::string logfile = cl->htmlRegisterPage(*this, "EXPERTS/Log", "log", "html");
  // std::ofstream out(logfile.c_str());
  // out << "<HTML><HEAD><TITLE>Log file for run " << cl->RunNumber()
  //     << "</TITLE></HEAD>" << std::endl;
  // out << "<P>Some log file output would go here." << std::endl;
  // out.close();

  // std::string status = cl->htmlRegisterPage(*this, "EXPERTS/Status", "status", "html");
  // std::ofstream out2(status.c_str());
  // out2 << "<HTML><HEAD><TITLE>Status file for run " << cl->RunNumber()
  //      << "</TITLE></HEAD>" << std::endl;
  // out2 << "<P>Some status output would go here." << std::endl;
  // out2.close();
  // cl->SaveLogFile(*this);
  return 0;
}
