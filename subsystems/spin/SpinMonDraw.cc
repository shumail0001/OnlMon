#include "SpinMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2I.h>
#include <TLegend.h>
#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

SpinMonDraw::SpinMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int SpinMonDraw::Init()
{
  return 0;
}

int SpinMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "SpinMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "SpinMon Example Monitor", -1, 0, xsize / 2, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("spinpad1", "who needs this?", 0.05, 0.75, 0.95, 0.9, 0);
    Pad[1] = new TPad("spinpad2", "who needs this?", 0.05, 0.575, 0.275, 0.75, 0);
    Pad[2] = new TPad("spinpad2", "who needs this?", 0.275, 0.575, 0.5, 0.75, 0);
    Pad[3] = new TPad("spinpad3", "who needs this?", 0.5, 0.575, 0.725, 0.75, 0);
    Pad[4] = new TPad("spinpad4", "who needs this?", 0.725, 0.575, 0.95, 0.75, 0);
    Pad[5] = new TPad("spinpad5", "who needs this?", 0.05, 0.40, 0.275, 0.575, 0);
    Pad[6] = new TPad("spinpad6", "who needs this?", 0.275, 0.40, 0.5, 0.575, 0);
    Pad[7] = new TPad("spinpad7", "who needs this?", 0.5, 0.40, 0.725, 0.575, 0);
    Pad[8] = new TPad("spinpad8", "who needs this?", 0.725, 0.40, 0.95, 0.575, 0);
    Pad[9] = new TPad("spinpad9", "who needs this?", 0.05, 0.225, 0.275, 0.40, 0);
    Pad[10] = new TPad("spinpad10", "who needs this?", 0.275, 0.225, 0.5, 0.40, 0);
    Pad[11] = new TPad("spinpad11", "who needs this?", 0.5, 0.225, 0.725, 0.40, 0);
    Pad[12] = new TPad("spinpad12", "who needs this?", 0.725, 0.225, 0.95, 0.40, 0);
    Pad[13] = new TPad("spinpad13", "who needs this?", 0.05, 0.05, 0.275, 0.225, 0);
    Pad[14] = new TPad("spinpad14", "who needs this?", 0.275, 0.05, 0.5, 0.225, 0);
    Pad[15] = new TPad("spinpad15", "who needs this?", 0.5, 0.05, 0.725, 0.225, 0);
    Pad[16] = new TPad("spinpad16", "who needs this?", 0.725, 0.05, 0.95, 0.225, 0);
    Pad[0]->Draw();
    Pad[1]->Draw();
    Pad[2]->Draw();
    Pad[3]->Draw();
    Pad[4]->Draw();
    Pad[5]->Draw();
    Pad[6]->Draw();
    Pad[7]->Draw();
    Pad[8]->Draw();
    Pad[9]->Draw();
    Pad[10]->Draw();
    Pad[11]->Draw();
    Pad[12]->Draw();
    Pad[13]->Draw();
    Pad[14]->Draw();
    Pad[15]->Draw();
    Pad[16]->Draw();

    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "SpinMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "SpinMon2 Example Monitor", -xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[17] = new TPad("spinpad3", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[18] = new TPad("spinpad4", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[17]->Draw();
    Pad[18]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  return 0;
}

int SpinMonDraw::Draw(const std::string &what)
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

int SpinMonDraw::DrawFirst(const std::string & /* what */)
{

  const int NTRIG = 16;
  
  OnlMonClient *cl = OnlMonClient::instance();
  TH2I *spinpatternBlueUp = (TH2I*)cl->getHisto("SPINMON_0","h2_spinpatternBlueUp");
  TH2I *spinpatternBlueDown = (TH2I*)cl->getHisto("SPINMON_0","h2_spinpatternBlueDown");
  TH2I *spinpatternBlueUnpol = (TH2I*)cl->getHisto("SPINMON_0","h2_spinpatternBlueUnpol");
  TH2I *spinpatternYellowUp = (TH2I*)cl->getHisto("SPINMON_0","h2_spinpatternYellowUp");
  TH2I *spinpatternYellowDown = (TH2I*)cl->getHisto("SPINMON_0","h2_spinpatternYellowDown");
  TH2I *spinpatternYellowUnpol = (TH2I*)cl->getHisto("SPINMON_0","h2_spinpatternYellowUnpol");

  TH1I* gl1_counter[NTRIG];
  for (int i = 0; i < NTRIG; i++){
    gl1_counter[i] = (TH1I*)cl->getHisto("SPINMON_0",Form("gl1_counter_trig%d",i));
  }

  if (!gROOT->FindObject("SpinMon1"))
  {
    MakeCanvas("SpinMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  gStyle->SetOptStat(0);
  //gStyle->SetLabelSize(0.5,"X");
  Pad[0]->cd();
  
  float labelsize = 0.1;
  if (!spinpatternBlueUp || !spinpatternBlueDown || !spinpatternBlueUnpol || !spinpatternYellowUp || !spinpatternYellowDown || !spinpatternYellowUnpol)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }
  else
  {
    spinpatternBlueUp->SetFillColor(6);
    spinpatternBlueUp->GetXaxis()->SetLabelSize(labelsize);
    spinpatternBlueUp->DrawCopy("box");
    spinpatternBlueDown->SetFillColor(7); 
    spinpatternBlueDown->GetXaxis()->SetLabelSize(labelsize);
    spinpatternBlueDown->DrawCopy("box,same");
    spinpatternBlueUnpol->SetFillColor(4); 
    spinpatternBlueDown->GetXaxis()->SetLabelSize(labelsize);
    spinpatternBlueUnpol->DrawCopy("box,same");
    spinpatternYellowUp->SetFillColor(2); 
    spinpatternYellowUp->DrawCopy("box,same");
    spinpatternYellowUp->GetXaxis()->SetLabelSize(labelsize);
    spinpatternYellowDown->SetFillColor(3); 
    spinpatternYellowDown->DrawCopy("box,same");
    spinpatternYellowDown->GetXaxis()->SetLabelSize(labelsize);
    spinpatternYellowUnpol->SetFillColor(5); 
    spinpatternYellowUnpol->DrawCopy("box,same");
    spinpatternYellowUnpol->GetXaxis()->SetLabelSize(labelsize);

    TLegend* leg_blue = new TLegend(0.01, 0.05, 0.075, 0.95);
    leg_blue->SetFillStyle(0);
    leg_blue->SetBorderSize(1);
    leg_blue->AddEntry(spinpatternBlueUp,"Blue Up","F");
    leg_blue->AddEntry(spinpatternBlueDown,"Blue Down","F");
    leg_blue->AddEntry(spinpatternBlueUnpol,"Blue Unpol","F");

    TLegend* leg_yellow = new TLegend(0.925, 0.05, 0.99, 0.95);
    leg_yellow->SetFillStyle(0);
    leg_yellow->SetBorderSize(1);
    leg_yellow->AddEntry(spinpatternYellowUp,"Yellow Up","F");
    leg_yellow->AddEntry(spinpatternYellowDown,"Yellow Down","F");
    leg_yellow->AddEntry(spinpatternYellowUnpol,"Yellow Unpol","F");

    leg_blue->Draw("same");
    leg_yellow->Draw("same");
  }

  labelsize = 0.8;
  float titlesize = 1.5;
  for (int i = 0; i < NTRIG; i++){
    Pad[i+1]->cd();
    if (gl1_counter[i]){
      gl1_counter[i]->SetTitleSize(titlesize);
      gl1_counter[i]->GetXaxis()->SetLabelSize(labelsize);
      gl1_counter[i]->DrawCopy("HIST");
    }
    else{
      DrawDeadServer(transparent[0]);
      TC[0]->SetEditable(false);
      return -1;
    }
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
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 1., runstring.c_str());
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);
  return 0;
}

int SpinMonDraw::DrawSecond(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  if (!gROOT->FindObject("SpinMon2"))
  {
    MakeCanvas("SpinMon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[17]->cd();

  Pad[18]->cd();

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
  TC[1]->SetEditable(false);
  return 0;
}

int SpinMonDraw::SavePlot(const std::string &what, const std::string &type)
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

int SpinMonDraw::MakeHtml(const std::string &what)
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
