#include "MvtxMonDraw.h"

#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TDatime.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
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

MvtxMonDraw::MvtxMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  // this TimeOffsetTicks is neccessary to get the time axis right
  TDatime T0(2003, 01, 01, 00, 00, 00);
  TimeOffsetTicks = T0.Convert();
  dbvars = new OnlMonDB(ThisName);
  return;
}

int MvtxMonDraw::Init()
{
  return 0;
}

int MvtxMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "MvtxMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "MvtxMon Example Monitor", -1, 0, xsize / 2, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("mvtxpad1", "who needs this?", 0.1, 0.7, 0.9, 0.9, 0);
    Pad[1] = new TPad("mvtxpad2", "who needs this?", 0.1, 0.3, 0.9, 0.7, 0);
    Pad[2] = new TPad("mvtxpad3", "who needs this?", 0.1, 0.05, 0.9, 0.3, 0);
    Pad[0]->Draw();
    Pad[1]->Draw();
      Pad[2]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "MvtxMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "MvtxMon2 Example Monitor", -xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[3] = new TPad("mvtxpad4", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[4] = new TPad("mvtxpad5", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[3]->Draw();
    Pad[4]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "MvtxMon3")
  {
    TC[2] = new TCanvas(name.c_str(), "MvtxMon3 Example Monitor", xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[5] = new TPad("mvtxpad6", "who needs this?", 0.1, 0.5, 0.9, 0.9, 0);
    Pad[6] = new TPad("mvtxpad7", "who needs this?", 0.1, 0.05, 0.9, 0.45, 0);
    Pad[5]->Draw();
    Pad[6]->Draw();
    // this one is used to plot the run number on the canvas
    //        transparent[2] = new TPad("transparent2", "this does not show", 0, 0, 1, 1);
    //        transparent[2]->SetFillStyle(4000);
    //        transparent[2]->Draw();
    //      TC[2]->SetEditable(0);
  }
  else if (name == "MvtxMon_HitMap")
  {
    TC[3] = new TCanvas(name.c_str(), "MvtxMon Example Monitor", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    TC[3]->Divide(NSTAVE,NCHIP);
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();
    TC[3]->SetEditable(false);
    TC[3]->SetTopMargin(0.05);
    TC[3]->SetBottomMargin(0.05);
  }
  return 0;
}

int MvtxMonDraw::Draw(const std::string &what)
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
  if (what == "ALL" || what == "HITMAP")
  {
    iret += DrawHitMap(what);
    idraw++;
  }
  if (what == "ALL" || what == "HISTORY")
  {
    iret += DrawHistory(what);
    idraw++;
  }
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  return iret;
}

int MvtxMonDraw::DrawFirst(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  //TH1 *mvtxmon_hist1 =  cl->getHisto("MVTXMON_0","mvtxmon_hist1");
  //TH1 *mvtxmon_hist2 =  cl->getHisto("MVTXMON_0","mvtxmon_hist1");

  TH2 *mvtxmon_ChipStaveOcc = dynamic_cast<TH2*>( cl->getHisto("MVTXMON_0","mvtxmon_ChipStaveOcc"));
  TH1 *mvtxmon_ChipStave1D =  cl->getHisto("MVTXMON_0","mvtxmon_ChipStave1D");
  TH1 *mvtxmon_ChipFiredHis =  cl->getHisto("MVTXMON_0","mvtxmon_ChipFiredHis");

  if (!gROOT->FindObject("MvtxMon1"))
  {
    MakeCanvas("MvtxMon1");
  }
  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  Pad[0]->cd();
  if (mvtxmon_ChipStaveOcc)
  {
    mvtxmon_ChipStaveOcc->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }
  Pad[1]->cd();
  if (mvtxmon_ChipStave1D)
  {
    mvtxmon_ChipStave1D->DrawCopy();
  }
  Pad[2]->cd();
  if (mvtxmon_ChipFiredHis)
  {
    mvtxmon_ChipFiredHis->DrawCopy();
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

int MvtxMonDraw::DrawSecond(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
    TH1 *mvtxmon_EvtHitChip =  cl->getHisto("MVTXMON_0","mvtxmon_EvtHitChip");
  TH1 *mvtxmon_EvtHitDis =  cl->getHisto("MVTXMON_0","mvtxmon_EvtHitDis");
  if (!gROOT->FindObject("MvtxMon2"))
  {
    MakeCanvas("MvtxMon2");
  }
  TC[1]->SetEditable(true);
  TC[1]->Clear("D");
  Pad[3]->cd();
  if (mvtxmon_EvtHitChip)
  {
    mvtxmon_EvtHitChip->DrawCopy();
  }
  else
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    return -1;
  }
  Pad[4]->cd();
  if (mvtxmon_EvtHitDis)
  {
    mvtxmon_EvtHitDis->DrawCopy();
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
  TC[1]->SetEditable(false);
  return 0;
}

int MvtxMonDraw::DrawHitMap(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  //TH1 *mvtxmon_hist1 =  cl->getHisto("MVTXMON_0","mvtxmon_hist1");
  //TH1 *mvtxmon_hist2 =  cl->getHisto("MVTXMON_0","mvtxmon_hist1");
  TH2 *mvtxmon_HitMap[NSTAVE][NCHIP] = {nullptr};

  for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){
			mvtxmon_HitMap[i][j] = dynamic_cast<TH2*>( cl->getHisto("MVTXMON_0",Form("mvtxmon_HitMap_%d_%d",i,j)));
    }
  }

  if (!gROOT->FindObject("MvtxMon_HitMap"))
  {
    MakeCanvas("MvtxMon_HitMap");
  }

  TC[3]->SetEditable(true);
  TC[3]->Clear("D");
  for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){
      TC[3]->cd(i+NSTAVE*j+1);
      if (mvtxmon_HitMap[i][j])
      {
        mvtxmon_HitMap[i][j]->GetXaxis()->SetTitle("Col");
			  mvtxmon_HitMap[i][j]->GetYaxis()->SetTitle("Row");
			  mvtxmon_HitMap[i][j]->GetXaxis()->CenterTitle();
			  mvtxmon_HitMap[i][j]->GetYaxis()->CenterTitle();
			  mvtxmon_HitMap[i][j]->GetYaxis()->SetTitleOffset(1.4);
			  mvtxmon_HitMap[i][j]->SetTitle(Form("2D HitMap: Stave %d and Chip ID %d",i,j));
			  mvtxmon_HitMap[i][j]->DrawCopy("COLZ");
      }
      else
      {
        DrawDeadServer(transparent[0]);
        TC[3]->SetEditable(false);
        return -1;
      }
    }
  }
  TC[3]->cd();
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
  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(false);
  return 0;
}

int MvtxMonDraw::MakePS(const std::string &what)
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

int MvtxMonDraw::MakeHtml(const std::string &what)
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

int MvtxMonDraw::DrawHistory(const std::string & /* what */)
{
  int iret = 0;
  // you need to provide the following vectors
  // which are filled from the db
  std::vector<float> var;
  std::vector<float> varerr;
  std::vector<time_t> timestamp;
  std::vector<int> runnumber;
  std::string varname = "mvtxmondummy";
  // this sets the time range from whihc values should be returned
  time_t begin = 0;            // begin of time (1.1.1970)
  time_t end = time(nullptr);  // current time (right NOW)
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << __PRETTY_FUNCTION__ << " Error in db access" << std::endl;
    return iret;
  }
  if (!gROOT->FindObject("MvtxMon3"))
  {
    MakeCanvas("MvtxMon3");
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
  Pad[5]->cd();
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

  varname = "mvtxmoncount";
  iret = dbvars->GetVar(begin, end, varname, timestamp, runnumber, var, varerr);
  if (iret)
  {
    std::cout << __PRETTY_FUNCTION__ << " Error in db access" << std::endl;
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
  Pad[6]->cd();
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
