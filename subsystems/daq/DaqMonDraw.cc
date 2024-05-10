#include "DaqMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TLine.h>
#include <TPad.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TText.h>

#include <cstring>  // for memset
#include <ctime>
#include <fstream>
#include <iostream>  // for operator<<, basic_ostream, basic_os...
#include <sstream>
#include <vector>  // for vector

DaqMonDraw::DaqMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int DaqMonDraw::Init()
{
  gStyle->SetOptStat(0);
  daqStyle = new TStyle("daqStyle", "daqStyle");

  // Int_t font = 42;  // Helvetica
  //  daqStyle->SetLabelFont(font, "x");
  //  daqStyle->SetTitleFont(font, "x");
  //  daqStyle->SetLabelFont(font, "y");
  //  daqStyle->SetTitleFont(font, "y");
  //  daqStyle->SetLabelFont(font, "z");
  //  daqStyle->SetTitleFont(font, "z");
  daqStyle->SetOptStat(0);
  daqStyle->SetPadTickX(1);
  daqStyle->SetPadTickY(1);

  Int_t colors[2];
  colors[0] = kRed;
  colors[1] = kGreen;

  daqStyle->SetPalette(2, colors);
  daqStyle->SetOptStat(0);
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  return 0;
}

int DaqMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "DaqMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "Calo ADC System Clock Check", -1, 0, xsize / 2, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    //  Int_t color[2];
    //  color[0] = kRed;
    //  color[1] = kGreen;

    //  gStyle->SetPalette(2, color);
    //  gStyle->SetOptStat(0);
    Pad[0] = new TPad("hist", "On the top", 0., 0.2, 1., 1.);
    Pad[0]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
    gStyle->SetOptStat(0);
  }
  else if (name == "DaqMon2")
  {
    gStyle->SetOptStat(0);
    TC[1] = new TCanvas(name.c_str(), "Calo ADC System Clock Check Capture", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[1] = new TPad("pad2", "pad2", 0., 0.2, 1., 1.);
    Pad[1]->Draw();
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
    gStyle->SetOptStat(0);
  }
  else if (name == "DaqMon3")
  {
    gStyle->SetOptStat(0);
    TC[2] = new TCanvas(name.c_str(), "Calo ADC System Clock Check Capture", -1, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    transparent[2]->SetFillStyle(4000);
    transparent[2]->Draw();
    TC[2]->SetEditable(false);
    gStyle->SetOptStat(0);
  }
  return 0;
}

int DaqMonDraw::Draw(const std::string &what)
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
  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    idraw++;
  }
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int DaqMonDraw::DrawFirst(const std::string & /* what */)
{
  gStyle->SetOptStat(0);
  OnlMonClient *cl = OnlMonClient::instance();

  if (!gROOT->FindObject("DaqMon1"))
  {
    MakeCanvas("DaqMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  TC[0]->SetLeftMargin(0.1);
  Pad[0]->cd();
  Pad[0]->SetTicks(1, 1);
  Pad[0]->SetGrid(1, 1);

  int start = -1;
  TH2 *h_gl1_clock_diff[m_ServerSet.size()];
  int i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_gl1_clock_diff[i] = (TH2 *) cl->getHisto(*server, "h_gl1_clock_diff");
    if (!h_gl1_clock_diff[i])
    {
      continue;
    }
    h_gl1_clock_diff[i]->GetXaxis()->SetTitleSize(0);

    for (int ibinx = 1; ibinx <= h_gl1_clock_diff[i]->GetNbinsX(); ibinx++)
    {
      for (int ibiny = 1; ibiny <= h_gl1_clock_diff[i]->GetNbinsY(); ibiny++)
      {
        float content = h_gl1_clock_diff[i]->GetBinContent(ibinx, ibiny);
        if (content < 10)
        {
          h_gl1_clock_diff[i]->SetBinContent(ibinx, ibiny, 0);
        }
      }
    }

    if (start == -1)
    {
      start = i;
    }
    else if (i > start)
    {
      h_gl1_clock_diff[start]->Add(h_gl1_clock_diff[i], 1);
    }
    i++;
  }
  if (start < 0)
  {
    return 0;
  }

  int nbinsx = h_gl1_clock_diff[start]->GetNbinsX();
  int nbinsy = h_gl1_clock_diff[start]->GetNbinsY();
  for (int ibx = 1; ibx <= nbinsx; ibx++)
  {
    float tot = 0;
    for (int iby = 1; iby <= nbinsy; iby++)
    {
      tot += h_gl1_clock_diff[start]->GetBinContent(ibx, iby);
    }
    for (int iby = 1; iby <= nbinsy; iby++)
    {
      float con = h_gl1_clock_diff[start]->GetBinContent(ibx, iby);
      if (con > 0)
      {
        h_gl1_clock_diff[start]->SetBinContent(ibx, iby, con / tot * 100.);
      }
    }
  }
  Int_t color[2];
  color[0] = kRed;
  color[1] = kGreen;
  gStyle->SetPalette(2, color);
  gStyle->SetOptStat(0);
  h_gl1_clock_diff[start]->Draw("col");
  TLine line(h_gl1_clock_diff[start]->GetXaxis()->GetXmin(), 0.5, h_gl1_clock_diff[start]->GetXaxis()->GetXmax(), 0.5);
  line.SetLineColor(kBlack);
  line.Draw();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.025);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": Calo-GL1 Lock Full History, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[0]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  //  TC[0]->Show();
  TC[0]->SetEditable(false);
  gStyle->SetOptStat(0);
  /*
  line.Draw("same");
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  TC[0]->Update();
  line.Draw("same");
  gStyle->SetPalette(2, color);
  gStyle->SetOptStat(0);
  Pad[0]->Update();
  line.Draw("same");
  TC[0]->Update();
  line.Draw("same");
  //gStyle->SetOptStat(0);
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  //gStyle->SetPalette(2, color);
  //Pad[0]->Update();
  //TC[0]->Update();
  //gStyle->SetPalette(2, color);
  gStyle->SetOptStat(0);
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  TC[0]->Show();
  //TC[0]->SetEditable(true);
 */
  return 0;
}

int DaqMonDraw::DrawSecond(const std::string & /* what */)
{
  gStyle->SetOptStat(0);
  OnlMonClient *cl = OnlMonClient::instance();

  if (!gROOT->FindObject("DaqMon2"))
  {
    MakeCanvas("DaqMon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  TC[1]->SetLeftMargin(0.1);
  Pad[1]->cd();
  Pad[1]->SetTicks(1, 1);
  Pad[1]->SetGridy(1);

  int start = -1;
  TH2 *h_gl1_clock_diff_capture[m_ServerSet.size()];
  int i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_gl1_clock_diff_capture[i] = (TH2 *) cl->getHisto(Form("DAQMON_%d", i), "h_gl1_clock_diff_capture");
    if (!h_gl1_clock_diff_capture[i])
    {
      continue;
    }
    if (start == -1)
    {
      start = i;
    }
    else if (i > start)
    {
      h_gl1_clock_diff_capture[start]->Add(h_gl1_clock_diff_capture[i], 1);
    }
    i++;
  }
  if (start < 0)
  {
    return 0;
  }

  Int_t color[2];
  color[0] = kRed;
  color[1] = kGreen;
  gStyle->SetPalette(2, color);
  gStyle->SetOptStat(0);
  h_gl1_clock_diff_capture[start]->Draw("col");

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.025);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  time_t evttime = getTime();
  // fill run number and event time into string
  runnostream << ThisName << ": Calo-GL1 Lock Snapshot, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime);
  runstring = runnostream.str();
  transparent[1]->cd();
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  TC[1]->SetEditable(false);
  gStyle->SetOptStat(0);
  /*
  line.Draw("same");
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  TC[0]->Update();
  line.Draw("same");
  gStyle->SetPalette(2, color);
  gStyle->SetOptStat(0);
  Pad[0]->Update();
  line.Draw("same");
  TC[0]->Update();
  line.Draw("same");
  //gStyle->SetOptStat(0);
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  //gStyle->SetPalette(2, color);
  //Pad[0]->Update();
  //TC[0]->Update();
  //gStyle->SetPalette(2, color);
  gStyle->SetOptStat(0);
  gROOT->SetStyle("daqStyle");
  gROOT->ForceStyle();
  TC[0]->Show();
  //TC[0]->SetEditable(true);
 */
  return 0;
}
int DaqMonDraw::SavePlot(const std::string &what, const std::string &type)
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
int DaqMonDraw::MakeHtml(const std::string &what)
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

time_t DaqMonDraw::getTime()
{
  OnlMonClient *cl = OnlMonClient::instance();
  time_t currtime = cl->EventTime("CURRENT");
  return currtime;
}

int DaqMonDraw::DrawServerStats()
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("DaqMon3"))
  {
    MakeCanvas("DaqMon3");
  }
  TC[2]->Clear("D");
  TC[2]->SetEditable(true);
  transparent[2]->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  PrintRun.SetTextSize(0.04);
  PrintRun.SetTextColor(1);
  PrintRun.DrawText(0.5, 0.99, "Server Statistics");

  PrintRun.SetTextSize(0.02);
  double vdist = 0.05;
  double vpos = 0.9;
  for (const auto &server : m_ServerSet)
  {
    std::ostringstream txt;
    auto servermapiter = cl->GetServerMap(server);
    txt << "Server " << server
        << ", run number " << std::get<1>(servermapiter->second)
        << ", event count: " << std::get<2>(servermapiter->second)
        << ", current time " << ctime(&(std::get<3>(servermapiter->second)));
    if (std::get<0>(servermapiter->second))
    {
      PrintRun.SetTextColor(3);
    }
    else
    {
      PrintRun.SetTextColor(2);
    }
    PrintRun.DrawText(0.5, vpos, txt.str().c_str());
    vpos -= vdist;
  }
  TC[2]->Update();
  TC[2]->Show();
  TC[2]->SetEditable(false);

  return 0;
}
