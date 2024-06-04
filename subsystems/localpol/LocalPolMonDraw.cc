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
#include <TLine.h>
#include <TMath.h>
#include <TPad.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <map>
#include <sstream>
#include <vector>  // for vector

LocalPolMonDraw::LocalPolMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int LocalPolMonDraw::Init()
{
  g_Asym = new TGraphErrors ***[2];
  g_AsymScramble = new TGraphErrors ***[2];
  h_Asym = new TH1D ***[2];
  h_ScrambleAsym = new TH1D ***[2];
  TString BeamName[2] = {"Blue", "Yell"};
  TString MethodName[2] = {"Arithmetic", "Geometric"};
  TString Orientation[2] = {"LR", "UD"};
  for (int beam = 0; beam < 2; beam++)
  {
    g_Asym[beam] = new TGraphErrors **[2];
    g_AsymScramble[beam] = new TGraphErrors **[2];
    h_Asym[beam] = new TH1D **[2];
    h_ScrambleAsym[beam] = new TH1D **[2];
    for (int method = 0; method < 2; method++)
    {
      g_Asym[beam][method] = new TGraphErrors *[2];
      g_AsymScramble[beam][method] = new TGraphErrors *[2];
      h_Asym[beam][method] = new TH1D *[2];
      h_ScrambleAsym[beam][method] = new TH1D *[2];
      for (int orient = 0; orient < 2; orient++)
      {
        // Since root badely handle the associated histograms for drawing the frame, ending up in instabilities and crashes
        // Poor alternative for the moment, to create Fresh TGraph each time we need
        g_Asym[beam][method][orient] = nullptr;
        g_AsymScramble[beam][method][orient] = nullptr;
      }
    }
  }

  // Equivalent of TH1F frame when drawing a TGraph for polar coordinate system
  gpolargram = new TGraphPolargram("locpolpolargram", 0, 0.05, -TMath::Pi(), TMath::Pi());
  gpolargram->SetToRadian();
  gpolargram->SetNdivPolar(216);
  gpolargram->SetNdivRadial(105);

  g_Polar = new TGraphPolar **[2];
  for (int beam = 0; beam < 2; beam++)
  {
    g_Polar[beam] = new TGraphPolar *[2];
    for (int method = 0; method < 2; method++)
    {
      // For the same reason as above, to prevent random crash inside root, create a fresh object each time we need
      g_Polar[beam][method] = nullptr;
    }
  }
  Pad = new TPad **[6];  // Who is taking care of the deletion?
  Pad[0] = new TPad *[16];
  Pad[1] = new TPad *[4];
  Pad[2] = new TPad *[1];
  Pad[3] = new TPad *[16];
  Pad[4] = new TPad *[12];
  Pad[5] = new TPad *[6];

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
    TC[0] = new TCanvas(name.c_str(), "LocalPolMon Asymmetries", -xsize / 2.5, 0, xsize / 2.5, ysize * 0.9);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    for (int ipad = 0; ipad < 16; ipad++)
    {
      Pad[0][ipad] = new TPad(Form("locpolpad0%d", ipad), "who needs this?", 0.0 + 0.25 * (ipad % 4), 0.95 - 0.2375 * (ipad / 4 + 1), 0.0 + 0.25 * (ipad % 4 + 1), 0.95 - 0.2375 * (ipad / 4), 0);
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
    for (int ipad = 0; ipad < 4; ipad++)
    {
      Pad[1][ipad] = new TPad(Form("locpolpad1%d", ipad), "who needs this?", 0.0 + 0.5 * (ipad % 2), 0.95 - 0.475 * (ipad / 2 + 1), 0.0 + 0.5 * (ipad % 2 + 1), 0.95 - 0.475 * (ipad / 2), 0);
      Pad[1][ipad]->SetTopMargin(0.15);
      Pad[1][ipad]->Draw();
    }

    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("locpoltransparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if(name=="LocalPolMon3"){
    // xpos negative: do not draw menu bar
    TC[2] = new TCanvas(name.c_str(), "LocalPolMon Spin Pattern", -xsize / 2.5, 0, xsize / 2.5, ysize);
    gSystem->ProcessEvents();
    Pad[2][0] = new TPad(Form("locpolpad2%d", 0), "who needs this?", 0.05 , 0.05, 0.95, 0.95, 0);
    Pad[2][0]->SetTopMargin(0.15);
    Pad[2][0]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[2] = new TPad("locpoltransparent2", "this does not show", 0, 0, 1, 1);
    transparent[2]->SetFillStyle(4000);
    transparent[2]->Draw();
    TC[2]->SetEditable(false);
    
  }
  else if(name=="LocalPolMon4"){
    // xpos negative: do not draw menu bar
    TC[3] = new TCanvas(name.c_str(), "Trigger distribution", -xsize / 2.5, 0, xsize / 2.5, ysize);
    gSystem->ProcessEvents();
    for (int ipad = 0; ipad < 16; ipad++)
    {
      Pad[3][ipad] = new TPad(Form("locpolpad3%d", ipad), "who needs this?", 0.0 + 0.25 * (ipad % 4), 0.95 - 0.2375 * (ipad / 4 + 1), 0.0 + 0.25 * (ipad % 4 + 1), 0.95 - 0.2375 * (ipad / 4), 0);
      Pad[3][ipad]->SetLeftMargin(0.15);
      Pad[3][ipad]->SetBottomMargin(0.15);
      Pad[3][ipad]->Draw();
    }
    // this one is used to plot the run number on the canvas
    transparent[3] = new TPad("locpoltransparent3", "this does not show", 0, 0, 1, 1);
    transparent[3]->SetFillStyle(4000);
    transparent[3]->Draw();
    TC[3]->SetEditable(false);
    
  }
  else if(name=="LocalPolMon5"){
    // xpos negative: do not draw menu bar
    TC[4] = new TCanvas(name.c_str(), "SMD distributions", -xsize / 2.5, 0, xsize / 2.5, ysize);
    gSystem->ProcessEvents();
    for (int ipad = 0; ipad < 12; ipad++)
    {
      Pad[4][ipad] = new TPad(Form("locpolpad4%d", ipad), "who needs this?", 0.0 + 0.25 * (ipad % 4), 0.95 - 0.316 * (ipad / 4 + 1), 0.0 + 0.25 * (ipad % 4 + 1), 0.95 - 0.316 * (ipad / 4), 0);
      Pad[4][ipad]->SetLeftMargin(0.15);
      Pad[4][ipad]->SetBottomMargin(0.15);
      Pad[4][ipad]->Draw();
    }
    // this one is used to plot the run number on the canvas
    transparent[4] = new TPad("locpoltransparent4", "this does not show", 0, 0, 1, 1);
    transparent[4]->SetFillStyle(4000);
    transparent[4]->Draw();
    TC[4]->SetEditable(false);
  }
  else if(name=="LocalPolMon6"){
    // xpos negative: do not draw menu bar
    TC[5] = new TCanvas(name.c_str(), "Waveforms", -xsize / 2.5, 0, xsize / 2.5, ysize);
    gSystem->ProcessEvents();
    for (int ipad = 0; ipad < 6; ipad++)
    {
      Pad[5][ipad] = new TPad(Form("locpolpad5%d", ipad), "who needs this?", 0.0 + 0.33 * (ipad % 3), 0.95 - 0.475 * (ipad / 3 + 1), 0.0 + 0.33 * (ipad % 3 + 1), 0.95 - 0.475 * (ipad / 3), 0);
      Pad[5][ipad]->SetLeftMargin(0.15);
      Pad[5][ipad]->SetBottomMargin(0.15);
      Pad[5][ipad]->Draw();
    }
    // this one is used to plot the run number on the canvas
    transparent[5] = new TPad("locpoltransparent5", "this does not show", 0, 0, 1, 1);
    transparent[5]->SetFillStyle(4000);
    transparent[5]->Draw();
    TC[5]->SetEditable(false);

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

  if (!gROOT->FindObject("LocalPolMon1"))
  {
    MakeCanvas("LocalPolMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  gStyle->SetOptStat(0);
  bool IsGood = true;
  TString BeamName[2] = {"Blue", "Yell"};
  TString MethodName[2] = {"Arithmetic", "Geometric"};
  TString Orientation[2] = {"LR", "UD"};
  for (int ibeam = 0; ibeam < 2; ibeam++)
  {
    for (int method = 0; method < 2; method++)
    {
      for (int orient = 0; orient < 2; orient++)
      {
        h_Asym[ibeam][method][orient] = (TH1D *) cl->getHisto("LOCALPOLMON_0", Form("h_Asym%s%s%s", BeamName[ibeam].Data(), MethodName[method].Data(), Orientation[orient].Data()));
        if (!h_Asym[ibeam][method][orient])
        {
          IsGood = false;
        }
        h_ScrambleAsym[ibeam][method][orient] = (TH1D *) cl->getHisto("LOCALPOLMON_0", Form("h_AsymScramble%s%s%s", BeamName[ibeam].Data(), MethodName[method].Data(), Orientation[orient].Data()));
        if (!h_ScrambleAsym[ibeam][method][orient])
        {
          IsGood = false;
        }
      }
    }
  }
  TProfile *h_times = (TProfile *) cl->getHisto("LOCALPOLMON_0", "h_times");
  if (!h_times)
  {
    IsGood = false;
  }
  if (!IsGood)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }

  for (int ibeam = 0; ibeam < 2; ibeam++)
  {
    for (int orient = 0; orient < 2; orient++)
    {
      for (int method = 0; method < 2; method++)
      {
        if (g_Asym[ibeam][method][orient])
        {
          delete g_Asym[ibeam][method][orient];
          g_Asym[ibeam][method][orient] = nullptr;
        }
        g_Asym[ibeam][method][orient] = new TGraphErrors();
        g_Asym[ibeam][method][orient]->SetName(Form("g_Asym%s%s%s", BeamName[ibeam].Data(), MethodName[method].Data(), Orientation[orient].Data()));
        g_Asym[ibeam][method][orient]->SetMarkerStyle(kFullCircle);
        g_Asym[ibeam][method][orient]->SetMarkerColor(kBlue);
        g_Asym[ibeam][method][orient]->SetLineColor(kBlue);
        g_Asym[ibeam][method][orient]->SetTitle(Form("%s %s %s Asym.", BeamName[ibeam].Data(), Orientation[orient].Data(), MethodName[method].Data()));

        if (g_AsymScramble[ibeam][method][orient])
        {
          delete g_AsymScramble[ibeam][method][orient];
          g_AsymScramble[ibeam][method][orient] = nullptr;
        }
        g_AsymScramble[ibeam][method][orient] = new TGraphErrors();
        g_AsymScramble[ibeam][method][orient]->SetName(Form("g_AsymScramble%s%s%s", BeamName[ibeam].Data(), MethodName[method].Data(), Orientation[orient].Data()));
        g_AsymScramble[ibeam][method][orient]->SetMarkerStyle(kFullCircle);
        g_AsymScramble[ibeam][method][orient]->SetMarkerColor(kGray);
        g_AsymScramble[ibeam][method][orient]->SetLineColor(kGray);
        g_AsymScramble[ibeam][method][orient]->SetTitle(Form("Unpol %s %s %s Asym.", BeamName[ibeam].Data(), Orientation[orient].Data(), MethodName[method].Data()));

        int N = h_times->GetNbinsX();
        for (int i = 0; i < N; i++)
        {
          if (h_Asym[ibeam][method][orient]->GetBinError(i + 1) == 0)
          {
            break;
          }
          g_Asym[ibeam][method][orient]->SetPoint(i, h_times->GetBinContent(i + 1), h_Asym[ibeam][method][orient]->GetBinContent(i + 1));
          g_Asym[ibeam][method][orient]->SetPointError(i, h_times->GetBinError(i + 1), h_Asym[ibeam][method][orient]->GetBinError(i + 1));

          g_AsymScramble[ibeam][method][orient]->SetPoint(i, h_times->GetBinContent(i + 1), h_ScrambleAsym[ibeam][method][orient]->GetBinContent(i + 1));
          g_AsymScramble[ibeam][method][orient]->SetPointError(i, h_times->GetBinError(i + 1), h_ScrambleAsym[ibeam][method][orient]->GetBinError(i + 1));
          m_time[i] = (long int) h_times->GetBinContent(i + 1);
        }
      }
    }
  }
  long int start = min_element(m_time.begin(), m_time.end(), [](const std::pair<int, long int> &lhs, const std::pair<int, long int> &rhs)
                               { return lhs.second < rhs.second; })
                       ->second;
  long int stop = max_element(m_time.begin(), m_time.end(), [](const std::pair<int, long int> &lhs, const std::pair<int, long int> &rhs)
                              { return lhs.second < rhs.second; })
                      ->second;

  if (stop - start < 15 * 60)
  {  // less than 15 min, we set it to 15min
    stop = start + 15 * 60;
  }
  else
  {
    stop = stop + 60;  // we add 1 min
  }
  start = start - 60;

  if (hframe)
  {
    delete hframe;
    hframe = nullptr;
  }
  hframe = new TH1F("locpolframe", "", 100, start, stop);
  hframe->SetStats(kFALSE);

  if (hscrambleframe)
  {
    delete hscrambleframe;
    hscrambleframe = nullptr;
  }
  hscrambleframe = new TH1F("locpolscrambleframe", "", 100, start, stop);
  hscrambleframe->SetStats(kFALSE);

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
  for (int ibeam = 0; ibeam < 2; ibeam++)
  {
    for (int orient = 0; orient < 2; orient++)
    {
      for (int method = 0; method < 2; method++)
      {
        Pad[0][8 * ibeam + 2 * method + orient]->cd();
        hframe->Draw();
        hframe->GetXaxis()->SetTimeDisplay(1);
        hframe->GetXaxis()->SetTimeFormat("%H:%M%F1970-01-01 00:00:00s0");
        hframe->GetXaxis()->SetNdivisions(507);
        hframe->GetXaxis()->SetTitle("Time (hh:mm)");
        hframe->GetXaxis()->SetTitleSize(0.06);
        hframe->GetXaxis()->SetTitleOffset(1.1);
        hframe->GetXaxis()->SetLabelSize(0.06);
        hframe->GetXaxis()->SetLabelOffset(0.025);

        hframe->GetYaxis()->SetTitle("Fwd Asymmetry");
        hframe->GetYaxis()->SetTitleSize(0.06);
        hframe->GetYaxis()->SetTitleOffset(1.1);
        hframe->GetYaxis()->CenterTitle(true);
        hframe->GetYaxis()->SetRangeUser(-0.1, 0.1);
        hframe->GetYaxis()->SetLabelSize(0.06);
        l.DrawLine(start, 0.00, stop, 0.00);
        lref.DrawLine(start, 0.01, stop, 0.01);
        lref.DrawLine(start, 0.02, stop, 0.02);
        g_Asym[ibeam][method][orient]->Draw("epsame");
        PrintTitle.DrawText(0.5, 0.95, Form("%s %s %s Asym.", BeamName[ibeam].Data(), Orientation[orient].Data(), MethodName[method].Data()));

        Pad[0][8 * ibeam + 2 * method + orient + 4]->cd();
        hscrambleframe->Draw();
        hscrambleframe->GetXaxis()->SetTimeDisplay(1);
        hscrambleframe->GetXaxis()->SetTimeFormat("%H:%M%F1970-01-01 00:00:00s0");
        hscrambleframe->GetXaxis()->SetNdivisions(507);
        hscrambleframe->GetXaxis()->SetTitle("Time (hh:mm)");
        hscrambleframe->GetXaxis()->SetTitleSize(0.06);
        hscrambleframe->GetXaxis()->SetTitleOffset(1.1);
        hscrambleframe->GetXaxis()->SetLabelSize(0.06);
        hscrambleframe->GetXaxis()->SetLabelOffset(0.025);

        hscrambleframe->GetYaxis()->SetTitle("Bwd Asymmetry");
        hscrambleframe->GetYaxis()->SetTitleSize(0.06);
        hscrambleframe->GetYaxis()->SetTitleOffset(1.1);
        hscrambleframe->GetYaxis()->CenterTitle(true);
        hscrambleframe->GetYaxis()->SetRangeUser(-0.1, 0.1);
        hscrambleframe->GetYaxis()->SetLabelSize(0.06);
        l.DrawLine(start, 0, stop, 0);
        lref.DrawLine(start, -0.01, stop, -0.01);
        lref.DrawLine(start, 0.01, stop, 0.01);
        g_AsymScramble[ibeam][method][orient]->Draw("epsame");
        PrintTitle.DrawText(0.5, 0.95, Form("Unpol. %s %s %s Asym.", BeamName[ibeam].Data(), Orientation[orient].Data(), MethodName[method].Data()));
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

  TString BeamName[2] = {"Blue", "Yell"};
  TString MethodName[2] = {"Arithmetic", "Geometric"};
  TString Orientation[2] = {"LR", "UD"};
  for (int beam = 0; beam < 2; beam++)
  {
    for (int method = 0; method < 2; method++)
    {
      if (g_Polar[beam][method])
      {
        delete g_Polar[beam][method];
        g_Polar[beam][method] = nullptr;
      }
      g_Polar[beam][method] = new TGraphPolar();
      g_Polar[beam][method]->SetName(Form("g_Polar%s%s", BeamName[beam].Data(), MethodName[method].Data()));
      g_Polar[beam][method]->SetMarkerStyle(kFullCircle);
      g_Polar[beam][method]->SetMarkerColor(kRed);
      g_Polar[beam][method]->SetLineColor(kRed);
      g_Polar[beam][method]->SetPoint(0, 0, 0);
      g_Polar[beam][method]->SetPolargram(gpolargram);
    }
  }

  bool IsGood = true;
  for (int ibeam = 0; ibeam < 2; ibeam++)
  {
    for (int method = 0; method < 2; method++)
    {
      for (int orient = 0; orient < 2; orient++)
      {
        h_Asym[ibeam][method][orient] = (TH1D *) cl->getHisto("LOCALPOLMON_0", Form("h_Asym%s%s%s", BeamName[ibeam].Data(), MethodName[method].Data(), Orientation[orient].Data()));
        if (!h_Asym[ibeam][method][orient])
        {
          IsGood = false;
        }
        h_ScrambleAsym[ibeam][method][orient] = (TH1D *) cl->getHisto("LOCALPOLMON_0", Form("h_AsymScramble%s%s%s", BeamName[ibeam].Data(), MethodName[method].Data(), Orientation[orient].Data()));
        if (!h_ScrambleAsym[ibeam][method][orient])
        {
          IsGood = false;
        }
      }
    }
  }
  TProfile *h_times = (TProfile *) cl->getHisto("LOCALPOLMON_0", "h_times");
  if (!h_times)
  {
    IsGood = false;
  }
  if (!IsGood)
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    return -1;
  }
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(kWhite);
  gROOT->ForceStyle();
  TText PrintTitle;
  PrintTitle.SetTextFont(62);
  PrintTitle.SetTextSize(0.06);
  PrintTitle.SetNDC();          // set to normalized coordinates
  PrintTitle.SetTextAlign(23);  // center/top alignment
  for (int ibeam = 0; ibeam < 2; ibeam++)
  {
    for (int method = 0; method < 2; method++)
    {
      int N = h_Asym[ibeam][method][0]->GetNbinsX();
      for (int i = 0; i < N; i++)
      {
        double x = h_Asym[ibeam][method][1]->GetBinContent(i + 1);  // left right
        double y = h_Asym[ibeam][method][0]->GetBinContent(i + 1);  // top-bottom
        double ex = h_Asym[ibeam][method][1]->GetBinError(i + 1);
        double ey = h_Asym[ibeam][method][0]->GetBinError(i + 1);
        double theta = atan2(y, x);
        double radius = sqrt(x * x + y * y);
        if (radius < 1e-7)
        {
          radius = 1e-7;
        }
        double etheta = sqrt(y * y * ex * ex + x * x * ey * ey) / pow(radius, 2);
        double eradius = sqrt(x * x * ex * ex + y * y * ey * ey) / radius;
        g_Polar[ibeam][method]->SetPoint(i + 1, theta, radius);
        g_Polar[ibeam][method]->SetPointError(i + 1, etheta, eradius);
      }
      Pad[1][2 * method + ibeam]->cd();
      // if(gPad->FindObject("polargram"))   std::cout<<"polargram found\n";
      // if(gPad->FindObject("locpolpolargram")) std::cout<<"mypolagram found\n";
      g_Polar[ibeam][method]->GetPolargram()->Draw();
      g_Polar[ibeam][method]->Draw("epsame");
      PrintTitle.DrawText(0.15, 0.98, Form("%s %s", BeamName[ibeam].Data(), MethodName[method].Data()));
      Pad[1][2 * method + ibeam]->Update();
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


int LocalPolMonDraw::DrawThird(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  if (!gROOT->FindObject("LocalPolMon3"))
  {
    MakeCanvas("LocalPolMon3");
  }
  TC[2]->SetEditable(true);
  TC[2]->Clear("D");
  gStyle->SetOptStat(0);
  TH2I* hspin=(TH2I*)cl->getHisto("LOCALPOLMON_0","hspinpattern");
  if(!hspin){
    DrawDeadServer(transparent[2]);
    TC[2]->SetEditable(false);
    return -1;
  }
  
  Pad[2][0]->cd();
  hspin->DrawCopy("colz");
  
  TC[2]->Update();
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
  transparent[2]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[2]->Update();
  TC[2]->Show();
  TC[2]->SetEditable(false);
  return 0;
}

int LocalPolMonDraw::DrawFourth(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  if (!gROOT->FindObject("LocalPolMon4"))
  {
    MakeCanvas("LocalPolMon4");
  }
  TC[3]->SetEditable(true);
  TC[3]->Clear("D");
  gStyle->SetOptStat(0);
  TH1D* htrig[16];
  for(int i=0; i<16; i++){
    htrig[i]=(TH1D*)cl->getHisto("LOCALPOLMON_0",Form("h_trigger%d",i));
    if(!htrig[i]){
      DrawDeadServer(transparent[3]);
      TC[3]->SetEditable(false);
      return -1;
    }
    else{
      Pad[3][i]->cd();
      htrig[i]->SetFillColor(kBlue);
      htrig[i]->DrawCopy();
    }   
  }
  
  TC[3]->Update();
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
  transparent[3]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(false);
  return 0;
}

int LocalPolMonDraw::DrawFifth(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  if (!gROOT->FindObject("LocalPolMon5"))
  {
    MakeCanvas("LocalPolMon5");
  }
  TC[4]->SetEditable(true);
  TC[4]->Clear("D");
  gStyle->SetOptStat(0);

  TH1D* htmp;
  Pad[4][0]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hmultiplicitySMD_NH");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }
  Pad[4][1]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hmultiplicitySMD_NV");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }
  Pad[4][2]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hmultiplicitySMD_SH");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }
  Pad[4][3]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hmultiplicitySMD_SV");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }

  Pad[4][4]->cd();
  Pad[4][4]->cd()->SetLogy();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hadcsumSMD_NH");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }
  Pad[4][5]->cd();
  Pad[4][5]->cd()->SetLogy();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hadcsumSMD_NV");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }
  Pad[4][6]->cd();
  Pad[4][6]->cd()->SetLogy();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hadcsumSMD_SH");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }
  Pad[4][7]->cd();
  Pad[4][7]->cd()->SetLogy();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hadcsumSMD_SV");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->DrawCopy();
  }

  Pad[4][8]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_NH_up");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kRed);
    htmp->DrawCopy();
  }
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_NH_dn");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kBlue);
    htmp->DrawCopy("same");
  }
  Pad[4][9]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_NV_up");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kRed);
    htmp->DrawCopy();
  }
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_NV_dn");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kBlue);
    htmp->DrawCopy("same");
  }
  Pad[4][10]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_SH_up");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kRed);
    htmp->DrawCopy();
  }
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_SH_dn");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kBlue);
    htmp->DrawCopy("same");
  }
  Pad[4][11]->cd();
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_SV_up");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kRed);
    htmp->DrawCopy();
  }
  htmp=(TH1D*)cl->getHisto("LOCALPOLMON_0","hpositionSMD_SV_dn");
  if(!htmp){
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }
  else{
    htmp->SetLineColor(kBlue);
    htmp->DrawCopy("same");
  }


  TC[4]->Update();
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
  transparent[4]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(false);
  return 0;
}

int LocalPolMonDraw::DrawSixth(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  
  if (!gROOT->FindObject("LocalPolMon6"))
  {
    MakeCanvas("LocalPolMon6");
  }
  TC[5]->SetEditable(true);
  TC[5]->Clear("D");
  gStyle->SetOptStat(0);
  for(int i=0; i<6; i++){
    Pad[5][i]->cd();
    TH2D* h2=(TH2D*)cl->getHisto("LOCALPOLMON_0",Form("hwaveform%d",i));
    if(!h2){
      DrawDeadServer(transparent[5]);
      TC[5]->SetEditable(false);
      return -1;
    }
    else{
      h2->DrawCopy("colz");
    }
  }
  TC[5]->Update();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << "_5 Run " << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[5]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  TC[5]->Update();
  TC[5]->Show();
  TC[5]->SetEditable(false);
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

  return 0;
}
