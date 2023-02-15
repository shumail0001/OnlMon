#include "HcalMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <phool/phool.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>
#include <TQObject.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

HcalMonDraw::HcalMonDraw(const std::string& name)
  : OnlMonDraw(name)
{
  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars = new OnlMonDB(ThisName);
  return;
}

int HcalMonDraw::Init()
{
  hcalStyle = new TStyle("hcalStyle", "hcalStyle");

  Int_t font = 42;  // Helvetica
  hcalStyle->SetLabelFont(font, "x");
  hcalStyle->SetTitleFont(font, "x");
  hcalStyle->SetLabelFont(font, "y");
  hcalStyle->SetTitleFont(font, "y");
  hcalStyle->SetLabelFont(font, "z");
  hcalStyle->SetTitleFont(font, "z");
  hcalStyle->SetOptStat(0);
  hcalStyle->SetPadTickX(1);
  hcalStyle->SetPadTickY(1);
  gROOT->SetStyle("hcalStyle");
  gROOT->ForceStyle();

  return 0;
}

int HcalMonDraw::MakeCanvas(const std::string& name)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "HcalMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "HcalMon Example Monitor", -1, 0.05, xsize / 3, ysize * 0.9);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("hist", "On the top", 0., 0.2, 1., 1.);
    Pad[0]->Draw();
    // Pad[1]->Draw();
    //  this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();

    // warning
    warning[0] = new TPad("warning0", "this does not show", 0, 0, 0.9, 0.2);
    warning[0]->SetFillStyle(4000);
    warning[0]->Draw();

    TC[0]->SetEditable(0);
  }
  else if (name == "HcalMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "HcalMon2 Example Monitor", xsize / 3, 0, xsize / 3, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[2] = new TPad("hcalpad3", "who needs this?", 0.0, 0.0, 1, 0.92, 0);
    // Pad[3] = new TPad("hcalpad4", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[2]->Draw();
    // Pad[3]->Draw();
    //  this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(0);
  }
  else if (name == "HcalMon3")
  {
    TC[3] = new TCanvas(name.c_str(), "HcalMon3 Example Monitor", xsize / 3, 0, xsize / 3, ysize * 0.9);
    gSystem->ProcessEvents();
    Pad[6] = new TPad("hcalpad6", "who needs this?", 0.0, 0.6, 1.0, 0.95, 0);
    Pad[7] = new TPad("hcalpad7", "who needs this?", 0.0, 0.3, 1.0, 0.6, 0);
    Pad[8] = new TPad("hcalpad8", "who needs this?", 0.0, 0.0, 1.0, 0.3, 0);
    Pad[6]->Draw();
    Pad[7]->Draw();
    Pad[8]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[3]->SetFillStyle(4000);
    transparent[3]->Draw();
    TC[3]->SetEditable(0);
  }
  else if (name == "HcalPopUp")
  {
    TC[4] = new TCanvas(name.c_str(), "pop up window", 0.05, 0.05, xsize / 3, ysize / 3);
    gSystem->ProcessEvents();
    Pad[9] = new TPad("hcalpad9", "for single tower running mean", 0.0, 0.0, 1, 0.92, 0);
    Pad[9]->Draw();
    transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
    transparent[4]->SetFillStyle(4000);
    transparent[4]->Draw();
    TC[4]->SetEditable(0);
  }
  return 0;
}

int HcalMonDraw::Draw(const std::string& what)
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
  /*
  if (what == "ALL" || what == "HISTORY")
  {
    iret += DrawHistory(what);
    idraw++;
  }
  */
  if (!idraw)
  {
    std::cout << PHWHERE << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}


int HcalMonDraw::DrawFirst(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();
  TH2D* hist1 = (TH2D*) cl->getHisto("h2_hcal_rm");
  TH2D* h2_hcal_mean = (TH2D*) cl->getHisto("h2_hcal_mean");
  TH1F* h_event = (TH1F*) cl->getHisto("h_event");
  if (!gROOT->FindObject("HcalMon1"))
  {
    MakeCanvas("HcalMon1");
  }
  if (!hist1)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(0);
    return -1;
  }
  h2_hcal_mean->Scale(1. / h_event->GetEntries());
  hist1->Divide(h2_hcal_mean);



  TC[0]->SetEditable(1);
  TC[0]->Clear("D");
  Pad[0]->cd();

  hist1->GetXaxis()->SetTitle("eta index");
  hist1->GetYaxis()->SetTitle("phi index");
  hist1->GetXaxis()->CenterTitle();
  hist1->GetYaxis()->CenterTitle();
  hist1->GetXaxis()->SetNdivisions(24);
  hist1->GetYaxis()->SetNdivisions(232);

  float tsize = 0.025;
  hist1->GetXaxis()->SetLabelSize(tsize);
  hist1->GetYaxis()->SetLabelSize(tsize);
  hist1->GetZaxis()->SetLabelSize(tsize);
  hist1->GetXaxis()->SetTitleSize(tsize);
  hist1->GetYaxis()->SetTitleSize(tsize);
  hist1->GetXaxis()->SetTickLength(0.02);

  hist1->GetZaxis()->SetRangeUser(0, 2);

  TLine* line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = new TLine(0, (i_line + 1) * 2, 24, (i_line + 1) * 2);
    line_sector[i_line]->SetLineColor(1);
    line_sector[i_line]->SetLineWidth(4);
    line_sector[i_line]->SetLineStyle(1);
  }
  TLine* line_board1 = new TLine(8, 0, 8, 64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(4);
  line_board1->SetLineStyle(1);
  TLine* line_board2 = new TLine(16, 0, 16, 64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(4);
  line_board2->SetLineStyle(1);

  TLine* line_iphi[64];
  for (int i_line = 0; i_line < 64; i_line++)
  {
    line_iphi[i_line] = new TLine(0, (i_line + 1), 24, (i_line + 1));
    line_iphi[i_line]->SetLineColor(1);
    line_iphi[i_line]->SetLineWidth(1);
    line_iphi[i_line]->SetLineStyle(1);
  }
  TLine* line_ieta[64];
  for (int i_line = 0; i_line < 24; i_line++)
  {
    line_ieta[i_line] = new TLine((i_line + 1), 0, (i_line + 1), 64);
    line_ieta[i_line]->SetLineColor(1);
    line_ieta[i_line]->SetLineWidth(1);
    line_ieta[i_line]->SetLineStyle(1);
  }

  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.07);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.11);

  hist1->Draw("colz");
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line]->Draw();
  }
  line_board1->Draw();
  line_board2->Draw();

  for (int i_line = 0; i_line < 64; i_line++)
  {
    line_iphi[i_line]->Draw();
  }
  for (int i_line = 0; i_line < 24; i_line++)
  {
    line_ieta[i_line]->Draw();
  }

  Int_t palette[3] = {1, 8, 2};
  hcalStyle->SetPalette(3, palette);
  gROOT->SetStyle("hcalStyle");
  gROOT->ForceStyle();
  gStyle->SetPalette(3, palette);
  double_t levels[4] = {0, 0.9, 1.1, 2};
  hist1->SetContour(4, levels);

  FindHotTower(warning[0], hist1);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": tower running mean divided by template";
  runnostream2 << "Run" << cl->RunNumber() << ", Time: " << ctime(&evttime);
  transparent[0]->cd();
  runstring = runnostream.str();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());

  // this connects the clicks on TCavas to the HandleEvent method that makes a pop up window
  // and display the running mean history of the tower correponding to the bin you click on
  TC[0]->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "HcalMonDraw", this,
                 "HandleEvent(int,int,int,TObject*)");

  TC[0]->Update();
  TC[0]->Show();

  TC[0]->SetEditable(0);


  //TC[0]->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "TCanvas", TC[0],
  //          "Paint()");
  return 0;
}

int HcalMonDraw::DrawSecond(const std::string& /* what */)
{
  const int Nsector = 32;
  OnlMonClient* cl = OnlMonClient::instance();
  TH1F* h_sectorAvg_total = (TH1F*) cl->getHisto("h_sectorAvg_total");
  TH1F* h_event = (TH1F*) cl->getHisto("h_event");
  TH1F* h_rm_sectorAvg[Nsector];
  for (int ih = 0; ih < Nsector; ih++)
  {
    h_rm_sectorAvg[ih] = (TH1F*) cl->getHisto(Form("h_rm_sectorAvg_s%d", ih));
  }

  if (!gROOT->FindObject("HcalMon2"))
  {
    MakeCanvas("HcalMon2");
  }

  TC[1]->SetEditable(1);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (!h_rm_sectorAvg[0] || !h_event || !h_sectorAvg_total)
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(0);
    return -1;
  }

  h_sectorAvg_total->Scale(1. / h_event->GetEntries());

  for (int ih = 0; ih < Nsector; ih++)
  {
    h_rm_sectorAvg[ih]->Scale(1. / h_sectorAvg_total->GetBinContent(ih + 1));
    for (int ib = 1; ib < h_rm_sectorAvg[ih]->GetNbinsX(); ib++)
    {
      h_rm_sectorAvg[ih]->SetBinContent(ib, ih + h_rm_sectorAvg[ih]->GetBinContent(ib));
    }
  }

  gStyle->SetTitleFontSize(0.03);

  gStyle->SetOptStat(0);

  TH1F* frame = new TH1F("frame", "", 100, 0, 100);
  frame->Draw("AXIS");
  frame->GetXaxis()->SetTitle("time");
  frame->GetYaxis()->SetTitle("sector running mean / template + sector #");
  frame->GetXaxis()->CenterTitle();
  frame->GetYaxis()->CenterTitle();
  // frame->GetXaxis()->SetNdivisions(20);
  // frame->GetYaxis()->SetNdivisions(232);
  float tsize = 0.03;
  frame->GetXaxis()->SetLabelSize(tsize);
  frame->GetYaxis()->SetLabelSize(tsize);
  frame->GetXaxis()->SetTitleSize(tsize);
  frame->GetYaxis()->SetTitleSize(tsize);

  frame->GetXaxis()->SetRangeUser(0, 100);
  frame->GetYaxis()->SetRangeUser(0, 32.75);

  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetTopMargin(0.01);

  for (int ih = 0; ih < Nsector; ih++)
  {
    h_rm_sectorAvg[ih]->Draw("same hist ][");
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_running mean, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(0);
  return 0;
}

int HcalMonDraw::DrawThird(const std::string& /* what */)
{
  OnlMonClient* cl = OnlMonClient::instance();
  TH1F* h_waveform_twrAvg = (TH1F*) cl->getHisto("h_waveform_twrAvg");
  TH1F* h_waveform_time = (TH1F*) cl->getHisto("h_waveform_time");
  TH1F* h_waveform_pedestal = (TH1F*) cl->getHisto("h_waveform_pedestal");

  if (!gROOT->FindObject("HcalMon3"))
  {
    MakeCanvas("HcalMon3");
    std::cout << std::endl
              << "made canvas" << std::endl;
  }

  TC[3]->SetEditable(1);
  TC[3]->Clear("D");
  Pad[6]->cd();
  if (!h_waveform_twrAvg || !h_waveform_time || !h_waveform_pedestal)
  {
    DrawDeadServer(transparent[3]);
    TC[3]->SetEditable(0);
    return -1;
  }

  gStyle->SetTitleFontSize(0.03);

  h_waveform_twrAvg->Draw("lhist");

  float tsize = 0.06;
  h_waveform_twrAvg->GetXaxis()->SetNdivisions(16);
  h_waveform_twrAvg->GetXaxis()->SetTitle("sample #");
  h_waveform_twrAvg->GetYaxis()->SetTitle("Avg tower ADC in latest event");
  h_waveform_twrAvg->GetXaxis()->SetLabelSize(tsize);
  h_waveform_twrAvg->GetYaxis()->SetLabelSize(tsize);
  h_waveform_twrAvg->GetXaxis()->SetTitleSize(tsize);
  h_waveform_twrAvg->GetYaxis()->SetTitleSize(tsize);
  h_waveform_twrAvg->GetXaxis()->SetTitleOffset(1.2);
  h_waveform_twrAvg->GetYaxis()->SetTitleOffset(0.75);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.10);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": Pulse fitting, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[3]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  Pad[7]->cd();

  gStyle->SetTitleFontSize(0.06);

  float tsize2 = 0.08;
  h_waveform_time->Draw("hist");
  h_waveform_time->GetXaxis()->SetNdivisions(16);
  h_waveform_time->GetXaxis()->SetTitle("Sample #");
  h_waveform_time->GetYaxis()->SetTitle("Towers");
  h_waveform_time->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_time->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_time->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_time->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_time->GetXaxis()->SetTitleOffset(1.0);
  h_waveform_time->GetYaxis()->SetTitleOffset(0.65);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.10);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  Pad[8]->cd();

  gStyle->SetTitleFontSize(0.06);

  h_waveform_pedestal->Draw("hist");
  h_waveform_pedestal->GetXaxis()->SetNdivisions(16);
  h_waveform_pedestal->GetXaxis()->SetTitle("ADC Pedistal");
  h_waveform_pedestal->GetYaxis()->SetTitle("Towers");
  h_waveform_pedestal->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_pedestal->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_pedestal->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_pedestal->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_pedestal->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_pedestal->GetYaxis()->SetTitleOffset(0.65);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.10);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();

  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(0);

  return 0;
}

int HcalMonDraw::FindHotTower(TPad* warningpad, TH2D* hhit)
{
  int nhott = 0;
  int ndeadt = 0;
  int displaylimit = 15;
  // get histogram
  std::ostringstream hottowerlist;
  std::ostringstream deadtowerlist;
  float hot_threshold = 1.33;
  float dead_threshold = 0.66;

  for (int ieta = 0; ieta < 24; ieta++)
  {
    for (int iphi = 0; iphi < 64; iphi++)
    {
      double nhit = hhit->GetBinContent(ieta + 1, iphi + 1);

      if (nhit > hot_threshold)
      {
        if (nhott <= displaylimit)
          hottowerlist << " (" << ieta << "," << iphi << ")";
        nhott++;
      }

      if (nhit < dead_threshold)
      {
        if (ndeadt <= displaylimit)
          deadtowerlist << " (" << ieta << "," << iphi << ")";
        ndeadt++;
      }
    }
  }

  if (nhott > displaylimit)
    hottowerlist << "... " << nhott << " total";
  if (ndeadt > displaylimit)
    deadtowerlist << "... " << ndeadt << " total";

  // draw warning here
  warningpad->cd();
  TText Warn;
  Warn.SetTextFont(62);
  Warn.SetTextSize(0.06);
  Warn.SetTextColor(2);
  Warn.SetNDC();
  Warn.SetTextAlign(23);
  Warn.DrawText(0.5, 1, "Hot towers:");
  Warn.DrawText(0.5, 0.9, hottowerlist.str().c_str());

  Warn.SetTextColor(4);
  Warn.SetTextAlign(22);
  Warn.DrawText(0.5, 0.7, "Dead towers:");
  Warn.DrawText(0.5, 0.6, deadtowerlist.str().c_str());

  warningpad->Update();
  return 0;
}

// int HcalMonDraw::DrawSecond(const std::string & /* what */)
/*
{
  OnlMonClient *cl = OnlMonClient::instance();
  TH2D* hist1 = (TH2D*)cl->getHisto("h2_hcal_rm");

 if (!gROOT->FindObject("HcalMon2"))
   {
     MakeCanvas("HcalMon2");
   }


  TC[1]->SetEditable(1);
  TC[1]->Clear("D");
  Pad[2]->cd();
  if (!hist1)
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(0);
    return -1;
  }



  gStyle->SetTitleFontSize(0.03);

  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);


  hist1->GetXaxis()->SetTitle("ieta");
  hist1->GetYaxis()->SetTitle("iphi");
  hist1->GetXaxis()->SetTitleSize(0.025);
  hist1->GetYaxis()->SetTitleSize(0.025);
  hist1->GetXaxis()->CenterTitle();
  hist1->GetYaxis()->CenterTitle();
  hist1->GetXaxis()->SetNdivisions(24);
  hist1->GetYaxis()->SetNdivisions(232);
  hist1->GetXaxis()->SetLabelSize(0.02);
  hist1->GetYaxis()->SetLabelSize(0.02);
  hist1->GetZaxis()->SetLabelSize(0.018);


  TLine *line_sector[32];
  for(int i_line=0;i_line<32;i_line++)
    {
      line_sector[i_line] = new TLine(0,(i_line+1)*2,24,(i_line+1)*2);
      line_sector[i_line]->SetLineColor(1);
      line_sector[i_line]->SetLineWidth(1.2);
      line_sector[i_line]->SetLineStyle(1);
    }
  TLine *line_board1 = new TLine(8,0,8,64);
  line_board1->SetLineColor(1);
  line_board1->SetLineWidth(1.2);
  line_board1->SetLineStyle(1);
  TLine *line_board2 = new TLine(16,0,16,64);
  line_board2->SetLineColor(1);
  line_board2->SetLineWidth(1.2);
  line_board2->SetLineStyle(1);

  gPad->SetTopMargin(0.04);
  gPad->SetBottomMargin(0.06);
  gPad->SetLeftMargin(0.06);
  gPad->SetRightMargin(0.11);
  gPad->SetTickx();
  gPad->SetTicky();

  hist1->Draw("colz");
  for(int i_line=0;i_line<32;i_line++)
    {
      line_sector[i_line]->Draw();
    }
  line_board1->Draw();
  line_board2->Draw();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_running mean, Run" << cl->RunNumber()
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

/* currently not using the hits 2D plot
int HcalMonDraw::DrawDeadServer(TPad *transparentpad)
{
  transparentpad->cd();
  TText FatalMsg;
  FatalMsg.SetTextFont(62);
  FatalMsg.SetTextSize(0.1);
  FatalMsg.SetTextColor(4);
  FatalMsg.SetNDC();          // set to normalized coordinates
  FatalMsg.SetTextAlign(23);  // center/top alignment
  FatalMsg.DrawText(0.5, 0.9, "HCAL MONITOR");
  FatalMsg.SetTextAlign(22);  // center/center alignment
  FatalMsg.DrawText(0.5, 0.5, "SERVER");
  FatalMsg.SetTextAlign(21);  // center/bottom alignment
  FatalMsg.DrawText(0.5, 0.1, "DEAD");
  transparentpad->Update();
  return 0;
}
*/

int HcalMonDraw::MakePS(const std::string& what)
{
  OnlMonClient* cl = OnlMonClient::instance();
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

int HcalMonDraw::MakeHtml(const std::string& what)
{
  int iret = Draw(what);
  if (iret)  // on error no html output please
  {
    return iret;
  }

  OnlMonClient* cl = OnlMonClient::instance();

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

int HcalMonDraw::DrawHistory(const std::string& /* what */)
{
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  std::vector<float> var;
  std::vector<float> varerr;
  std::vector<time_t> timestamp;
  std::vector<int> runnumber;
  std::string varname = "hcalmondummy";
  // this sets the time range from whihc values should be returned
  time_t begin = 0;            // begin of time (1.1.1970)
  time_t end = time(nullptr);  // current time (right NOW)
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << PHWHERE << " Error in db access" << std::endl;
    return iret;
  }
  if (!gROOT->FindObject("HcalMon3"))
  {
    MakeCanvas("HcalMon3");
  }
  // timestamps come sorted in ascending order
  float* x = new float[var.size()];
  float* y = new float[var.size()];
  float* ex = new float[var.size()];
  float* ey = new float[var.size()];
  int n = var.size();
  for (unsigned int i = 0; i < var.size(); i++)
  {
    //       std::cout << "timestamp: " << ctime(&timestamp[i])
    //     << ", run: " << runnumber[i]
    //     << ", var: " << var[i]
    //     << ", varerr: " << varerr[i]
    //     << std::endl;
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

  varname = "hcalmoncount";
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
    //     << ", run: " << runnumber[i]
    //     << ", var: " << var[i]
    //     << ", varerr: " << varerr[i]
    //     << std::endl;
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

//this is the method to idetify which bin you are clicking on and make the pop up window of TH1
void HcalMonDraw::HandleEvent(int event, int x, int y, TObject *selected)
{
  if (event == 1) {
    printf("Canvas %s: event=%d, x=%d, y=%d, selected=%s\n", "a",
           event, x, y, selected->IsA()->GetName());

    double xx = Pad[0]->AbsPixeltoX(x);
    double xhis  = Pad[0]->PadtoX(xx);
    int binx = (int) xhis;
    double yy = Pad[0]->AbsPixeltoY(y);
    double yhis  = Pad[0]->PadtoY(yy);
    int biny = (int) yhis;
    printf("ieta=%d, iphi=%d \n", binx, biny);
    if (binx < 0 || binx > 23) return;
    if (biny < 0 || biny > 63) return;


    OnlMonClient* cl = OnlMonClient::instance();

    TH1F* h_rm_tower = (TH1F*) cl->getHisto(Form("h_rm_tower_%d_%d", binx, biny));;

    if (!gROOT->FindObject("HcalPopUp"))
    {
      MakeCanvas("HcalPopUp");
    }
    if (!h_rm_tower)
    {
      DrawDeadServer(transparent[4]);
      TC[4]->SetEditable(0);
      return;
    }
    TC[4]->SetEditable(1);
    TC[4]->Clear("D");
    Pad[9]->cd();
    gStyle->SetOptStat(0);
    h_rm_tower->Draw();
    TC[4]->Update();
    TC[4]->Show();
    TC[4]->SetEditable(0);

  }
}

