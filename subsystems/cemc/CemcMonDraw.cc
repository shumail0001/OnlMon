#include "CemcMonDraw.h"

#include <onlmon/OnlMonClient.h>

#include <calobase/TowerInfoDefs.h>

#include <TAxis.h>  // for TAxis
#include <TCanvas.h>
#include <TFile.h>
#include <TFrame.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TQObject.h>
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

CemcMonDraw::CemcMonDraw(const std::string &name)
  : OnlMonDraw(name)
{
  return;
}

int CemcMonDraw::Init()
{
  std::cout << "initializing" << std::endl;

  cemcStyle = new TStyle("cemcStyle", "cemcStyle");
  gStyle->SetOptStat(0);
  Int_t font = 42;  // Helvetica
  cemcStyle->SetLabelFont(font, "x");
  cemcStyle->SetTitleFont(font, "x");
  cemcStyle->SetLabelFont(font, "y");
  cemcStyle->SetTitleFont(font, "y");
  cemcStyle->SetLabelFont(font, "z");
  cemcStyle->SetTitleFont(font, "z");
  cemcStyle->SetOptStat(0);
  cemcStyle->SetPadTickX(1);
  cemcStyle->SetPadTickY(1);

  gROOT->SetStyle("cemcStyle");
  gROOT->ForceStyle();

  const char *CEMCcalib = getenv("CEMCCALIB");
  TFile *inputTemplate = nullptr;
  TFile *inputTemplate_alltrig = nullptr;
  if (!CEMCcalib)
  {
    std::cout << "CEMCCALIB environment variable not set, empty refence will be used" << std::endl;
    h2_template_hit = new TH2D("templateHit", "", 96, 0, 96, 256, 0, 256);
  }
  else
  {
    // std::string Templatefilename=std::string(CEMCcalib)+"/"+"Template_40929_100ADC_hits.root";
    // std::string Templatefilename=std::string(CEMCcalib)+"/"+"Template_40929_50ADC_hits.root";
    // std::string Templatefilename = std::string(CEMCcalib) + "/" + "Template_40929_30ADC_hits.root";
    //std::string Templatefilename = std::string(CEMCcalib) + "/" + "Template_43451_100ADC_hits.root";
    //std::string Templatefilename = std::string(CEMCcalib) + "/" + "Template_46915_100ADC_hits.root";
    std::string Templatefilename = std::string(CEMCcalib) + "/" + "Template_52876_100ADC_hits.root";    
    
    inputTemplate = new TFile(Templatefilename.c_str(), "READ");
    if (!inputTemplate->IsOpen())
    {
      std::cout << Templatefilename << " could not be opened. Empty reference will be used" << std::endl;
      h2_template_hit = new TH2D("templateHit", "", 96, 0, 96, 256, 0, 256);
    }
    else
    {
      h2_template_hit = (TH2 *) inputTemplate->Get("h2_cemc_hits_template");
      if (!h2_template_hit)
      {
        std::cout << "h2_cemc_hits_template could not be retrieved from file. Empty reference will be used" << std::endl;
        h2_template_hit = new TH2D("templateHit", "", 96, 0, 96, 256, 0, 256);
      }
    }
    std::string Templatefilename_alltrig = std::string(CEMCcalib) + "/" + "Template_49435_100ADC_alltrig.root";
    inputTemplate_alltrig = new TFile(Templatefilename_alltrig.c_str(), "READ");
    if (!inputTemplate_alltrig->IsOpen())
    {
      std::cout << Templatefilename_alltrig << " could not be opened. Empty reference will be used" << std::endl;
      h2_template_hit_alltrig = new TH2D("templateHit_alltrig", "", 96, 0, 96, 256, 0, 256);
    }
    else
    {
      h2_template_hit_alltrig = (TH2 *) inputTemplate_alltrig->Get("h2_cemc_rmhits_alltrig");
      if (!h2_template_hit_alltrig)
      {
        std::cout << "h2_cemc_hits_template could not be retrieved from file. Empty reference will be used" << std::endl;
        h2_template_hit_alltrig = new TH2D("templateHit_alltrig", "", 96, 0, 96, 256, 0, 256);
      }
    }
  }

  h1_zs = new TH1F("h1_zs", "unsuppressed rate ", 100, 0, 1.1);
  h1_zs_low = new TH1F("h1_zs_low", "unsuppressed rate ", 100, 0, 1.1);
  h1_zs_high = new TH1F("h1_zs_high", "unsuppressed rate ", 100, 0, 1.1);

  MakeZSPalette();

  return 0;
}

int CemcMonDraw::MakeCanvas(const std::string &name)
{
  OnlMonClient *cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "CemcMon1")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[0] = new TCanvas(name.c_str(), "CemcMon Tower Hits", -1, ysize, xsize / 3, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[0] = new TPad("cemcpad0", "hit map", 0., 0.15, 1., 0.95);
    Pad[0]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[0] = new TPad("transparent0", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillStyle(4000);
    transparent[0]->Draw();

    // warning
    warning[0] = new TPad("warning0", "hot tower warnings", 0, 0, 1, 0.15);
    warning[0]->SetFillStyle(4000);
    warning[0]->Draw();
    TC[0]->SetEditable(false);
  }
  else if (name == "CemcMon2")
  {
    // xpos negative: do not draw menu bar
    TC[1] = new TCanvas(name.c_str(), "Expert: CemcMon2 Packet Information",  -1, ysize, xsize / 3, ysize);
    gSystem->ProcessEvents();
    Pad[1] = new TPad("cemcpad1", "packet event check", 0.0, 0.6, 1.0 / 2, 0.95, 0);
    Pad[2] = new TPad("cemcpad2", "packet size", 0.0, 0.3, 1.0 / 2, 0.6, 0);
    Pad[3] = new TPad("cemcpad3", "packet channels", 0.0, 0.0, 1.0 / 2, 0.3, 0);
    Pad[1]->Draw();
    Pad[2]->Draw();
    Pad[3]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[1] = new TPad("transparent1", "this does not show", 0, 0, 1., 1);
    transparent[1]->SetFillStyle(4000);
    transparent[1]->Draw();

    // packet warnings
    warning[1] = new TPad("warning1", "packet warnings", 0.5, 0, 1, 1);
    warning[1]->SetFillStyle(4000);
    warning[1]->Draw();
    TC[1]->SetEditable(false);
  }
  else if (name == "CemcMon3")
  {
    TC[2] = new TCanvas(name.c_str(), "CemcMon Waveform Processing", -1, ysize, xsize / 3, ysize);
    gSystem->ProcessEvents();
    Pad[4] = new TPad("cemcpad4", "who needs this?", 0.0, 0.6, 1.0, 0.95, 0);
    Pad[5] = new TPad("cemcpad5", "who needs this?", 0.0, 0.3, 1.0, 0.6, 0);
    Pad[6] = new TPad("cemcpad6", "who needs this?", 0.0, 0.0, 1.0, 0.3, 0);
    Pad[4]->Draw();
    Pad[5]->Draw();
    Pad[6]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[2] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[2]->SetFillStyle(4000);
    transparent[2]->Draw();
    TC[2]->SetEditable(false);
  }
  else if (name == "CemcMon4")
  {
    TC[3] = new TCanvas(name.c_str(), "Expert: CemcMon Waveform Processing", -1, ysize, xsize / 3, ysize);
    gSystem->ProcessEvents();
    Pad[7] = new TPad("cemcpad7", "who needs this?", 0.0, 0.6, 1.0, 0.95, 0);
    Pad[8] = new TPad("cemcpad8", "who needs this?", 0.0, 0.3, 1.0, 0.6, 0);
    Pad[9] = new TPad("cemcpad9", "who needs this?", 0.0, 0.0, 1.0, 0.3, 0);
    Pad[7]->Draw();
    Pad[8]->Draw();
    Pad[9]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[3] = new TPad("transparent3", "this does not show", 0, 0, 1, 1);
    transparent[3]->SetFillStyle(4000);
    transparent[3]->Draw();
    TC[3]->SetEditable(false);
  }
  else if (name == "CemcMon5")
  {
    TC[4] = new TCanvas(name.c_str(), "Expert: Trigger Info", -1, ysize, xsize / 3, ysize);
    gSystem->ProcessEvents();
    Pad[10] = new TPad("cemcpad10", "who needs this?", 0.0, 0.6, 0.45, 0.95, 0);
    Pad[11] = new TPad("cemcpad11", "who needs this?", 0.45, 0.6, 0.9, 0.95, 0);
    Pad[12] = new TPad("cemcpad12", "who needs this?", 0.0, 0.3, 0.45, 0.6, 0);
    Pad[13] = new TPad("cemcpad13", "who needs this?", 0.45, 0.3, 0.9, 0.6, 0);
    Pad[14] = new TPad("cemcpad14", "who needs this?", 0.0, 0.0, 0.9, 0.3, 0);
    Pad[10]->Draw();
    Pad[11]->Draw();
    Pad[12]->Draw();
    Pad[13]->Draw();
    Pad[14]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[4] = new TPad("transparent4", "this does not show", 0, 0, 1, 1);
    transparent[4]->SetFillStyle(4000);
    transparent[4]->Draw();
    TC[4]->SetEditable(false);
  }
  else if (name == "CemcMonServerStats")
  {
    TC[5] = new TCanvas(name.c_str(), "CemcMon6 Server Stats", -1, 0, xsize, ysize);
    gSystem->ProcessEvents();
    // this one is used to plot the run number on the canvas
    transparent[5] = new TPad("transparent5", "this does not show", 0, 0, 1, 1);
    transparent[5]->SetFillColor(kGray);
    transparent[5]->Draw();
    TC[5]->SetEditable(false);
  }
  else if (name == "CemcAllTrigHits")
  {
    // xpos (-1) negative: do not draw menu bar
    TC[6] = new TCanvas(name.c_str(), "CemcMon Tower Hits all trig", -1, ysize, xsize / 3, ysize);
    // root is pathetic, whenever a new TCanvas is created root piles up
    // 6kb worth of X11 events which need to be cleared with
    // gSystem->ProcessEvents(), otherwise your process will grow and
    // grow and grow but will not show a definitely lost memory leak
    gSystem->ProcessEvents();
    Pad[15] = new TPad("cemcpad15", "hit map", 0., 0.15, 1., 0.95);
    Pad[15]->Draw();
    // this one is used to plot the run number on the canvas
    transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
    transparent[6]->SetFillStyle(4000);
    transparent[6]->Draw();

    // warning
    warning[2] = new TPad("warning6", "hot tower warnings", 0, 0, 1, 0.15);
    warning[2]->SetFillStyle(4000);
    warning[2]->Draw();
    TC[6]->SetEditable(false);
  }

  // Commented until potential replacement with TProfile3D
  // else if (name == "CemcPopup"){
  //   PopUpCanvas = new TCanvas(name.c_str(),"Waveforms Expert",-xsize-0.3,0,xsize*0.3,ysize*0.9);
  //   gSystem->ProcessEvents();
  //   double step=1./8;
  //   for(int i=0; i<8; i++){
  //     for(int j=0; j<8; j++){
  //   	PopUpPad[i][j]=new TPad(Form("PopUpPad%d_%d",i,j),"this does not show",i*step,j*step,(i+1)*step,(j+1)*step);
  //	PopUpPad[i][j]->Draw();
  //     }
  //   }
  //   PopUpTransparent=new TPad("PopUpTransparent","this does not show",0,0,1,1);
  //   PopUpTransparent->SetFillStyle(4000);
  //   PopUpTransparent->Draw();
  //   PopUpCanvas->SetEditable(false);
  // }
  // else if (name == "CemcMon7" ){
  //   TC[6] = new TCanvas(name.c_str(),"Basic waveform summary Expert", -xsize/3 , 0, xsize/3, ysize*0.9);
  //   gSystem->ProcessEvents();
  //   Pad[15]=new TPad("cemcpad15","who needs this?",0.0,0.475,0.5,0.950);
  //   Pad[15]->SetRightMargin(0.15);
  //   Pad[15]->Draw();
  //   Pad[16]=new TPad("cemcpad16","who needs this?",0.5,0.475,1.0,0.950);
  //   Pad[16]->Draw();
  //   Pad[17]=new TPad("cemcpad17","who needs this?",0.0,0.0,0.5,0.475);
  //   Pad[17]->SetRightMargin(0.15);
  //   Pad[17]->Draw();
  //   Pad[18]=new TPad("cemcpad18","who needs this?",0.5,0.0,1.0,0.475);
  //   Pad[18]->Draw();
  //   transparent[6] = new TPad("transparent6", "this does not show", 0, 0, 1, 1);
  //   transparent[6]->SetFillStyle(4000);
  //   transparent[6]->Draw();
  //   TC[6]->SetEditable(0);
  // }
  else if (name == "CemcMon8")
  {
    TC[7] = new TCanvas(name.c_str(), "Channel unsuppressed event fraction", -1, ysize, xsize / 3, ysize);
    gSystem->ProcessEvents();
    Pad[19] = new TPad("cemcpad19", "who needs this?", 0.00, 0.3, 1.00, 0.95);
    Pad[19]->SetRightMargin(0.15);
    Pad[19]->Draw();
    Pad[20] = new TPad("hcalpad24", "1d zs rate", 0.0, 0.0, 1.00, 0.3);
    Pad[20]->SetRightMargin(0.15);
    Pad[20]->Draw();
    transparent[7] = new TPad("transparent7", "this does not show", 0, 0, 1, 1);
    transparent[7]->SetFillStyle(4000);
    transparent[7]->Draw();
    TC[7]->SetEditable(false);
  }
  else if (name == "CemcBadChi2")
  {
    TC[8] = new TCanvas(name.c_str(), "Bad Chi2", -1, ysize, xsize / 2, ysize);
    gSystem->ProcessEvents();
    Pad[21] = new TPad("cemcpad21", "who needs this?", 0.00, 0.00, 1.00, 0.95);
    Pad[21]->SetRightMargin(0.15);
    Pad[21]->Draw();
    transparent[8] = new TPad("transparent8", "this does not show", 0, 0, 1, 1);
    transparent[8]->SetFillStyle(4000);
    transparent[8]->Draw();
    TC[8]->SetEditable(false);
  }

  return 0;
}

int CemcMonDraw::Draw(const std::string &what)
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
  
  // if (what == "ALL" || what == "SIXTH")
  //{
  //   iret += DrawSixth(what);
  //   idraw++;
  // }
  
  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    idraw++;
  }
  if (what == "ALL" || what == "ALLTRIGHITS")
  {
    iret += DrawAllTrigHits(what);
    idraw++;
  }
  if (what == "ALL" || what == "BADCHI2")
  {
    iret += DrawBadChi2(what);
    idraw++;
  }


// DO NOT CHANGE THE ORDER, DrawSeventh crashes DrawServerStats with an X11 error in the virtual framebuffer in the html
// DO NOT ADD ANY OTHER METHOD AFTER THIS which gets called by "ALL"
  if (what == "ALL")
  {
    iret += DrawSeventh("SEVENTH");
    idraw++;
    // iret += DrawSeventh("ALLTRIGZS");
    // idraw++;
  }
  if(what == "SEVENTH" || what == "ALLTRIGZS")
  {
    iret += DrawSeventh(what);
    idraw++;
  }
  
  if (!idraw)
  {
    std::cout << __PRETTY_FUNCTION__ << " Unimplemented Drawing option: " << what << std::endl;
    iret = -1;
  }
  if (std::fabs(iret) != idraw)  // at least one succeeded
  {
    return 0;
  }
  return iret;
}

int CemcMonDraw::DrawFirst(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // watch the absolute insanity as we merge all these
  // histograms from across seven different machines
  if (!h_cemc_datahits)
  {
    h_cemc_datahits = new TH2D("h_cemc_datahits", "", 96, 0, 96, 256, 0, 256);
  }
  else
  {
    h_cemc_datahits->Reset();
  }
  TH2 *htmp2d;
  int deadservercount = 0;
  int Nservers = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    Nservers++;
    htmp2d = (TH2 *) cl->getHisto(*server, "h2_cemc_rmhits");
    // htmp2d=(TH2 *) cl->getHisto(*server, "h2_cemc_rm");
    if (htmp2d)
    {
      h_cemc_datahits->Add(htmp2d);
    }
    else
    {
      deadservercount++;
    }
  }
  int avgevents = 0;
  int neventhist = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    TH1* h_eventSource  = cl->getHisto(*server, "h1_event");
    if (h_eventSource )
    {
      avgevents += h_eventSource->GetEntries();
      neventhist++;
    }
  }
  if (neventhist)
  {
    avgevents /= neventhist;
  }
  

  // TH2 *hist1[m_ServerSet.size()];
  // const int nHists = 4;
  // int start[nHists];
  // start[0] = -1;
  // int i = 0;
  // for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  //{
  //   hist1[i] = (TH2 *) cl->getHisto(*server, "h2_cemc_rm");
  //   if (hist1[i] && start[0] == -1)
  //   {
  //     start[0] = i;
  //   }
  //   if (start[0] > -1 && hist1[i])
  //   {
  //     hist1[i]->SetName(Form("h2_cemc_rm_%d", i));
  //     if (i != start[0])
  //     {
  //       hist1[start[0]]->Add(hist1[i], 1);
  //     }
  //   }
  //   i++;
  // }
  //
  // TH2 *h2_cemc_mean[m_ServerSet.size()];
  // start[1] = -1;
  // i = 0;
  // for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  //{
  //   h2_cemc_mean[i] = (TH2 *) cl->getHisto(*server, "h2_cemc_mean");
  //   if (h2_cemc_mean[i] && start[1] == -1)
  //   {
  //     start[1] = i;
  //   }
  //   if (start[1] > -1 && h2_cemc_mean[i])
  //   {
  //     h2_cemc_mean[i]->SetName(Form("h2_cemc_mean_%d", i));
  //     if (i != start[1])
  //     {
  //       h2_cemc_mean[start[1]]->Add(h2_cemc_mean[i], 1);
  //     }
  //   }
  //   i++;
  // }
  //
  // TH1 *h_event[m_ServerSet.size()];
  // TH1 *h_eventSource[m_ServerSet.size()];
  // start[2] = -1;
  // float maxEvent = 1;
  // i = 0;
  // for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  //{
  //   h_eventSource[i] = cl->getHisto(*server, "h1_event");
  //   if (h_eventSource[i] && start[2] == -1)
  //   {
  //     start[2] = i;
  //   }
  //   if (start[2] > -1 && h_eventSource[i])
  //   {
  //     h_event[i] =  h_eventSource[i]->Clone();
  //
  //     h_event[i]->SetName(Form("h1_event_%d", i));
  //     if (/*i != start[2]*/ h_event[i]->GetEntries() > maxEvent)
  //     {
  //       // h_event[start[2]] -> Add(h_event[i],1);
  //       maxEvent = h_event[i]->GetEntries();
  //     }
  //   }
  //   i++;
  // }

  if (!gROOT->FindObject("CemcMon1"))
  {
    MakeCanvas("CemcMon1");
  }

  if (deadservercount == Nservers)
  {
    DrawDeadServer(transparent[0]);
    TC[0]->SetEditable(false);
    return -1;
  }

  // TH1 *adcCount[m_ServerSet.size()];
  // start[3] = -1;
  // i = 0;
  // for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  //{
  //   adcCount[i] = cl->getHisto(*server, "h1_cemc_adc");
  //
  //   if (adcCount[i] && start[3] == -1)
  //   {
  //     start[3] = i;
  //   }
  //   if (start[3] > -1 && adcCount[i])
  //   {
  //     adcCount[i]->SetName(Form("h1_cemc_adc_%d", i));
  //     if (i != start[3])
  //     {
  //       adcCount[start[3]]->Add(adcCount[i], 1);
  //     }
  //   }
  // }

  // if (maxEvent > 0)
  //{
  //   if (maxEvent > templateDepth)
  //   {
  //     h2_cemc_mean[start[1]]->Scale(1. / templateDepth);
  //   }
  //   else
  //   {
  //     h2_cemc_mean[start[1]]->Scale(1. / maxEvent);
  //   }
  //   if (adcCount[start[3]]->GetMean())
  //   {
  //     h2_cemc_mean[start[1]]->Scale(1. / adcCount[start[3]]->GetMean());
  //   }
  //
  //   if (adcCount[start[3]]->GetMean())
  //   {
  //     hist1[start[0]]->Scale(1. / adcCount[start[3]]->GetMean());
  //   }
  //
  //   for (int k = 0; k < nTowersEta; k++)
  //   {
  //     for (int j = 0; j < nTowersPhi; j++)
  //     {
  //       // if(k < 8) continue;
  //       if (h2_cemc_mean[start[1]]->GetBinContent(k + 1, j + 1) < 0.75 && hist1[start[0]]->GetBinContent(k + 1, j + 1) < 0.75)
  //       {
  //         hist1[start[0]]->SetBinContent(k + 1, j + 1, h2_cemc_mean[start[1]]->GetBinContent(k + 1, j + 1));
  //       }
  //       else
  //       {
  //         hist1[start[0]]->SetBinContent(k + 1, j + 1, hist1[start[0]]->GetBinContent(k + 1, j + 1) / h2_cemc_mean[start[1]]->GetBinContent(k + 1, j + 1));
  //       }
  //     }
  //   }
  // }
  // else
  //{
  //   for (int k = 0; k < nTowersEta; k++)
  //   {
  //     for (int j = 0; j < nTowersPhi; j++)
  //     {
  //       // if(k < 8) continue;
  //
  //       hist1[start[0]]->SetBinContent(k + 1, j + 1, 0);
  //     }
  //   }
  // }

  TC[0]->SetEditable(true);
  TC[0]->Clear("D");
  Pad[0]->cd();

  // hist1[start[0]]->GetXaxis()->SetTitle("eta index");
  // hist1[start[0]]->GetYaxis()->SetTitle("phi index");
  // hist1[start[0]]->GetZaxis()->SetTitle("Tower Running Mean/ Histogram Running Mean");
  // hist1[start[0]]->GetXaxis()->CenterTitle();
  // hist1[start[0]]->GetYaxis()->CenterTitle();
  // hist1[start[0]]->GetZaxis()->CenterTitle();
  // hist1[start[0]]->GetXaxis()->SetNdivisions(12, kFALSE);
  // hist1[start[0]]->GetYaxis()->SetNdivisions(32, kFALSE);
  //
  // float tsize = 0.03;
  // hist1[start[0]]->GetXaxis()->SetLabelSize(tsize);
  // hist1[start[0]]->GetYaxis()->SetLabelSize(tsize);
  // hist1[start[0]]->GetYaxis()->SetTitleOffset(1.4);
  // hist1[start[0]]->GetZaxis()->SetLabelSize(tsize);
  // hist1[start[0]]->GetXaxis()->SetTitleSize(tsize);
  // hist1[start[0]]->GetYaxis()->SetTitleSize(tsize);
  // hist1[start[0]]->GetXaxis()->SetTickLength(0.02);
  h_cemc_datahits->Divide(h2_template_hit);
  h_cemc_datahits->GetXaxis()->SetTitle("eta index");
  h_cemc_datahits->GetYaxis()->SetTitle("phi index");
  h_cemc_datahits->GetZaxis()->SetTitle("Tower Running Mean/Tower reference");
  h_cemc_datahits->GetXaxis()->CenterTitle();
  h_cemc_datahits->GetYaxis()->CenterTitle();
  h_cemc_datahits->GetZaxis()->CenterTitle();
  h_cemc_datahits->GetXaxis()->SetNdivisions(12, kFALSE);
  h_cemc_datahits->GetYaxis()->SetNdivisions(32, kFALSE);

  float tsize = 0.03;
  h_cemc_datahits->GetXaxis()->SetLabelSize(tsize);
  h_cemc_datahits->GetYaxis()->SetLabelSize(tsize);
  h_cemc_datahits->GetYaxis()->SetTitleOffset(1.4);
  h_cemc_datahits->GetZaxis()->SetLabelSize(tsize);
  h_cemc_datahits->GetXaxis()->SetTitleSize(tsize);
  h_cemc_datahits->GetYaxis()->SetTitleSize(tsize);
  h_cemc_datahits->GetXaxis()->SetTickLength(0.02);

 

  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.07);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.12);

  // modify palette to black, green, and red
  // Int_t palette[3] = {kGray + 2, 8, 2};
  Int_t palette[4] = {kBlack, kBlue, 8, 2};
  cemcStyle->SetPalette(4, palette);
  gROOT->SetStyle("cemcStyle");
  gROOT->ForceStyle();
  gStyle->SetPalette(4, palette);
  double_t levels[5] = {0, 0.01, 0.5, 2, 4};
  h_cemc_datahits->GetZaxis()->SetRangeUser(0, 4);
  h_cemc_datahits->SetContour(5, levels);
  gStyle->SetOptStat(0);
  h_cemc_datahits->DrawCopy("colz");
  // h2_cemc_mean[start[1]]->DrawCopy("colz");
  TLine line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = TLine(0, (i_line + 1) * 8, 96, (i_line + 1) * 8);
    line_sector[i_line].SetLineColor(1);
    line_sector[i_line].SetLineWidth(1);
    line_sector[i_line].SetLineStyle(1);
  }

  const int numVertDiv = 12;
  int dEI = 96 / numVertDiv;
  TLine l_board[numVertDiv - 1];
  for (int il = 1; il < numVertDiv; il++)
  {
    l_board[il - 1] = TLine(dEI * il, 0, dEI * il, 256);
    l_board[il - 1].SetLineColor(1);
    l_board[il - 1].SetLineWidth(1);
    l_board[il - 1].SetLineStyle(1);
    if (il == 6)
    {
      l_board[il - 1].SetLineWidth(2);
    }
  }

  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line].DrawLine(0, (i_line + 1) * 8, 96, (i_line + 1) * 8);
  }

  for (int il = 1; il < numVertDiv; il++)
  {
    l_board[il - 1].DrawLine(dEI * il, 0, dEI * il, 256);
  }

  // FindHotTower(warning[0], hist1[start[0]]);
  FindHotTower(warning[0], h_cemc_datahits, true);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::ostringstream runnostream3;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": tower occupancy running mean/template";
  runnostream2 << " threshold: 100ADC, Run " << cl->RunNumber()<<", Event: " << avgevents;
  runnostream3 << "Time: " << ctime(&evttime.first);
  
  transparent[0]->cd();
  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.96, runstring.c_str());
  runstring = runnostream3.str();
  PrintRun.DrawText(0.5, 0.93, runstring.c_str());

  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);
  if (save)
  {
    TC[0]->SaveAs("plots/towerHits.pdf");
  }
  return 0;
}

int CemcMonDraw::DrawAllTrigHits(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // watch the absolute insanity as we merge all these
  // histograms from across seven different machines
  if (!h_cemc_datahits)
  {
    h_cemc_datahits = new TH2D("h_cemc_datahits", "", 96, 0, 96, 256, 0, 256);
  }
  else
  {
    h_cemc_datahits->Reset();
  }
  TH2 *htmp2d;
  int deadservercount = 0;
  int Nservers = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    Nservers++;
    htmp2d = (TH2 *) cl->getHisto(*server, "h2_cemc_rmhits_alltrig");
  
    if (htmp2d)
    {
      h_cemc_datahits->Add(htmp2d);
    }
    else
    {
      deadservercount++;
    }
  }
  int avgevents = 0;
  int neventhist = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    TH1* h_eventSource  = cl->getHisto(*server, "h1_event");
    if (h_eventSource )
    {
      avgevents += h_eventSource->GetEntries();
      neventhist++;
    }
  }
  if (neventhist)
  {
    avgevents /= neventhist;
  }
  

 
  if (!gROOT->FindObject("CemcAllTrigHits"))
  {
    MakeCanvas("CemcAllTrigHits");
  }

  if (deadservercount == Nservers)
  {
    DrawDeadServer(transparent[6]);
    TC[6]->SetEditable(false);
    return -1;
  }

  

  TC[6]->SetEditable(true);
  TC[6]->Clear("D");
  Pad[15]->cd();

  h_cemc_datahits->Divide(h2_template_hit_alltrig);
  
  h_cemc_datahits->GetXaxis()->SetTitle("eta index");
  h_cemc_datahits->GetYaxis()->SetTitle("phi index");
  h_cemc_datahits->GetZaxis()->SetTitle("Tower Running Mean with all trig");
  h_cemc_datahits->GetXaxis()->CenterTitle();
  h_cemc_datahits->GetYaxis()->CenterTitle();
  h_cemc_datahits->GetZaxis()->CenterTitle();
  h_cemc_datahits->GetXaxis()->SetNdivisions(12, kFALSE);
  h_cemc_datahits->GetYaxis()->SetNdivisions(32, kFALSE);

  float tsize = 0.03;
  h_cemc_datahits->GetXaxis()->SetLabelSize(tsize);
  h_cemc_datahits->GetYaxis()->SetLabelSize(tsize);
  h_cemc_datahits->GetYaxis()->SetTitleOffset(1.4);
  h_cemc_datahits->GetZaxis()->SetLabelSize(tsize);
  h_cemc_datahits->GetXaxis()->SetTitleSize(tsize);
  h_cemc_datahits->GetYaxis()->SetTitleSize(tsize);
  h_cemc_datahits->GetXaxis()->SetTickLength(0.02);
  h_cemc_datahits->GetZaxis()->SetTitleOffset(1.6);

 

  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.07);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.2);

  // modify palette to black, green, and red
  
  Int_t palette[4] = {kBlack, kBlue, 8, 2};
  cemcStyle->SetPalette(4, palette);
  gROOT->SetStyle("cemcStyle");
  gROOT->ForceStyle();
  gStyle->SetPalette(4, palette);
  double_t levels[5] = {0, 0.01, 0.5, 2, 4};
  h_cemc_datahits->GetZaxis()->SetRangeUser(0, 4);
  h_cemc_datahits->SetContour(5, levels);

  gStyle->SetOptStat(0);
  h_cemc_datahits->DrawCopy("colz");
  // h2_cemc_mean[start[1]]->DrawCopy("colz");
  TLine line_sector[32];
  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line] = TLine(0, (i_line + 1) * 8, 96, (i_line + 1) * 8);
    line_sector[i_line].SetLineColor(1);
    line_sector[i_line].SetLineWidth(1);
    line_sector[i_line].SetLineStyle(1);
  }

  const int numVertDiv = 12;
  int dEI = 96 / numVertDiv;
  TLine l_board[numVertDiv - 1];
  for (int il = 1; il < numVertDiv; il++)
  {
    l_board[il - 1] = TLine(dEI * il, 0, dEI * il, 256);
    l_board[il - 1].SetLineColor(1);
    l_board[il - 1].SetLineWidth(1);
    l_board[il - 1].SetLineStyle(1);
    if (il == 6)
    {
      l_board[il - 1].SetLineWidth(2);
    }
  }

  for (int i_line = 0; i_line < 32; i_line++)
  {
    line_sector[i_line].DrawLine(0, (i_line + 1) * 8, 96, (i_line + 1) * 8);
  }

  for (int il = 1; il < numVertDiv; il++)
  {
    l_board[il - 1].DrawLine(dEI * il, 0, dEI * il, 256);
  }

  FindHotTower(warning[2], h_cemc_datahits, true);
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::ostringstream runnostream3;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": tower occupancy/template running mean with all trig";
  runnostream2 << " threshold: 100ADC, Run " << cl->RunNumber();
  runnostream3 << "Time: " << ctime(&evttime.first);
  
  transparent[6]->cd();
  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());
  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.96, runstring.c_str());
  runstring = runnostream3.str();
  PrintRun.DrawText(0.5, 0.93, runstring.c_str());

  TC[6]->Update();
  TC[6]->Show();
  TC[6]->SetEditable(false);
  if (save)
  {
    TC[6]->SaveAs("plots/towerHits.pdf");
  }
  return 0;
}

int CemcMonDraw::DrawSecond(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();
  gStyle->SetOptStat(0);
  TH1 *h_event[m_ServerSet.size()];
  TH1 *h_eventSource[m_ServerSet.size()];
  int start[4];
  start[0] = -1;
  float maxEvent = -1;
  int i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_eventSource[i] = cl->getHisto(*server, "h1_event");
    if (h_eventSource[i] && start[0] == -1)
    {
      start[0] = i;
    }
    if (start[0] > -1 && h_eventSource[i])
    {
      h_event[i] =  (TH1 *) h_eventSource[i]->Clone();
      h_event[i]->SetName(Form("h1_event_%d", i));
      if (/*i != start[0] &&*/ h_event[i]->GetEntries() > maxEvent)
      {
        // h_event[start[0]] -> Add(h_event[i],1);
        maxEvent = h_event[i]->GetEntries();
      }
    }
    i++;
  }

  TH1 *h1_packet_number[m_ServerSet.size()];
  start[1] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h1_packet_number[i] = cl->getHisto(*server, "h1_packet_number");
    if (h1_packet_number[i] && start[1] == -1)
    {
      start[1] = i;
    }
    if (start[1] > -1 && h1_packet_number[i])
    {
      h1_packet_number[i]->SetName(Form("h1_papcket_number_%d", i));
      if (i != start[1])
      {
        h1_packet_number[start[1]]->Add(h1_packet_number[i], 1);
      }
    }
    i++;
  }

  TH1 *h1_packet_length[m_ServerSet.size()];
  start[2] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h1_packet_length[i] = cl->getHisto(*server, "h1_packet_length");
    if (h1_packet_length[i] && start[2] == -1)
    {
      start[2] = i;
    }
    if (start[2] > -1 && h1_packet_length[i])
    {
      h1_packet_length[i]->SetName(Form("h1_papcket_length_%d", i));
      if (i != start[2])
      {
        h1_packet_length[start[2]]->Add(h1_packet_length[i], 1);
      }
    }
    i++;
  }

  TH1 *h1_packet_chans[m_ServerSet.size()];
  start[3] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h1_packet_chans[i] =  cl->getHisto(*server, "h1_packet_chans");
    if (h1_packet_chans[i] && start[3] == -1)
    {
      start[3] = i;
    }
    if (start[3] > -1 && h1_packet_chans[i])
    {
      h1_packet_chans[i]->SetName(Form("h1_papcket_chans_%d", i));
      if (i != start[3])
      {
        h1_packet_chans[start[3]]->Add(h1_packet_chans[i], 1);
      }
    }
    i++;
  }

  if (start[0] < 0 || start[1] < 0 || start[2] < 0)
  {
    DrawDeadServer(transparent[1]);
    TC[1]->SetEditable(false);
    return -1;
  }

  // h_event[start[0]] -> Scale(1./divisor);

  if (maxEvent > 0)
  {
    // h1_packet_number[start[1]] -> Scale(1./h_event[start[0]] -> GetBinContent(1));
    // h1_packet_length[start[2]] -> Scale(1./h_event[start[0]] -> GetBinContent(1));
    // h1_packet_chans[start[3]] -> Scale(1./h_event[start[0]] -> GetBinContent(1));
    h1_packet_number[start[1]]->Scale(1. / maxEvent);
    h1_packet_length[start[2]]->Scale(1. / maxEvent);
    h1_packet_chans[start[3]]->Scale(1. / maxEvent);
  }

  if (!gROOT->FindObject("CemcMon2"))
  {
    MakeCanvas("CemcMon2");
  }

  TC[1]->SetEditable(true);
  TC[1]->Clear("D");

  TLine *one = new TLine(6000.5, 1, 6128.5, 1);
  one->SetLineStyle(7);

  TLine *goodSize = new TLine(6000.5, 1565, 6128.5, 1565);
  goodSize->SetLineStyle(7);

  TLine *goodChans = new TLine(6000.5, 192, 6128.5, 192);
  goodChans->SetLineStyle(7);

  float param = 0.95;
  // float param = 0.99;

  TLegend *leg = new TLegend(0.3, 0.20, 0.95, 0.50);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  TLine *warnLineOne = new TLine(6000.5, param * 1, 6128.5, param * 1);
  warnLineOne->SetLineStyle(7);
  warnLineOne->SetLineColor(2);

  leg->AddEntry(warnLineOne, Form("%g%% Threshold", param * 100), "l");

  TLine *warnLineOneS = new TLine(6000.5, param * 1, 6128.5, param * 1);
  warnLineOneS->SetLineStyle(10);
  warnLineOneS->SetLineColor(2);

  leg->AddEntry(warnLineOneS, Form("%g%% Threshold, High Eta, South", 100 * param), "l");

  TLine *warnLineSize = new TLine(6000.5, param * 5981., 6128.5, param * 5981.);
  warnLineSize->SetLineStyle(7);
  warnLineSize->SetLineColor(2);

  TLine *warnLineSizeS = new TLine(6000.5, param * 3991., 6128.5, param * 3991.);
  warnLineSizeS->SetLineStyle(10);
  warnLineSizeS->SetLineColor(2);

  TLine *warnLineChans = new TLine(6000.5, param * 192., 6128.5, param * 192.);
  warnLineChans->SetLineStyle(7);
  warnLineChans->SetLineColor(2);

  TLine *warnLineChansS = new TLine(6000.5, param * 128., 6128.5, param * 128.);
  warnLineChansS->SetLineStyle(10);
  warnLineChansS->SetLineColor(2);

  Pad[1]->cd();
  float tsize = 0.08;
  h1_packet_number[start[1]]->GetYaxis()->SetRangeUser(0.0, 1.3);
  std::vector<std::vector<int>> badPackets;
  badPackets.push_back(getBadPackets(h1_packet_number[start[1]], 0, param));

  h1_packet_number[start[1]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_packet_number[start[1]]->GetXaxis()->SetTitle("packet #");
  h1_packet_number[start[1]]->GetYaxis()->SetTitle("% Of Events Present");
  h1_packet_number[start[1]]->GetXaxis()->SetLabelSize(tsize - 0.01);
  h1_packet_number[start[1]]->GetYaxis()->SetLabelSize(tsize - 0.01);
  h1_packet_number[start[1]]->GetXaxis()->SetTitleSize(tsize - 0.01);
  h1_packet_number[start[1]]->GetYaxis()->SetTitleSize(tsize - 0.01);
  h1_packet_number[start[1]]->GetXaxis()->SetTitleOffset(1);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();
  h1_packet_number[start[1]]->DrawCopy("hist");
  one->Draw("same");
  warnLineOne->Draw("same");
  leg->Draw("same");

  Pad[2]->cd();
  badPackets.push_back(getBadPackets(h1_packet_length[start[2]], 1, 0));  // Not cutting on packet length anymore because of zero suppression

  h1_packet_length[start[2]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_packet_length[start[2]]->GetXaxis()->SetTitle("packet #");
  h1_packet_length[start[2]]->GetYaxis()->SetTitle("Average Packet Size");
  h1_packet_length[start[2]]->GetXaxis()->SetLabelSize(tsize - .01);
  h1_packet_length[start[2]]->GetYaxis()->SetLabelSize(tsize);
  h1_packet_length[start[2]]->GetXaxis()->SetTitleSize(tsize - .01);
  h1_packet_length[start[2]]->GetYaxis()->SetTitleSize(tsize);
  h1_packet_length[start[2]]->GetXaxis()->SetTitleOffset(1);
  h1_packet_length[start[2]]->GetYaxis()->SetTitleOffset(0.9);
  h1_packet_length[start[2]]->GetYaxis()->SetRangeUser(0, 4000);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();
  h1_packet_length[start[2]]->DrawCopy("hist");
  goodSize->Draw("same");
  // warnLineSize -> Draw("same");
  // warnLineSizeS -> Draw("same");

  Pad[3]->cd();
  h1_packet_chans[start[3]]->GetYaxis()->SetRangeUser(0, 212);
  badPackets.push_back(getBadPackets(h1_packet_chans[start[3]], 2, param));
  h1_packet_chans[start[3]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_packet_chans[start[3]]->GetXaxis()->SetTitle("packet #");
  h1_packet_chans[start[3]]->GetYaxis()->SetTitle("Average # of Channels");
  h1_packet_chans[start[3]]->GetXaxis()->SetLabelSize(tsize - .01);
  h1_packet_chans[start[3]]->GetYaxis()->SetLabelSize(tsize);
  h1_packet_chans[start[3]]->GetXaxis()->SetTitleSize(tsize - .01);
  h1_packet_chans[start[3]]->GetYaxis()->SetTitleSize(tsize);
  h1_packet_chans[start[3]]->GetXaxis()->SetTitleOffset(0.8);
  h1_packet_chans[start[3]]->GetYaxis()->SetTitleOffset(0.8);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();
  h1_packet_chans[start[3]]->DrawCopy("hist");
  goodChans->Draw("same");
  warnLineChans->Draw("same");
  warnLineChansS->Draw("same");

  warning[1]->cd();
  TLegend *badPacks = new TLegend(0, 1 / 4., 1, 0.9);
  TPaveText *title = new TPaveText(0, 0.9, 1, 0.95);
  title->AddText("Bad Packets");
  badPacks->SetNColumns(8);
  badPacks->SetTextSize(0.03);
  float badboys = 0;
  if (maxEvent > 0)
  {
    for (int k = 0; k < 3; k++)
    {
      for (int j = 0; j < (int) badPackets[k].size(); j++)
      {
        // there's most certainly a better way to do this but it's 5:00 on day 5 of owl shift
        // just want to prevent a packet showing up multiple times and crowding the screen
        if (badPackets[k][j] == 0)
        {
          continue;  // need this to prevent seg faulting
        }

        if (k == 0)
        {
          badPacks->AddEntry("", Form("%d", badPackets[k][j]), "");
          badboys++;
        }
        else if (k == 1)
        {
          if (!(std::count(badPackets[k - 1].begin(), badPackets[k - 1].end(), badPackets[k][j])))
          {
            badPacks->AddEntry("", Form("%d", badPackets[k].at(j)), "");
            badboys++;
          }
        }
        else if (k == 2)
        {
          if (!(std::count(badPackets[k - 1].begin(), badPackets[k - 1].end(), badPackets[k][j])))
          {
            badPacks->AddEntry("", Form("%d", badPackets[k].at(j)), "");
            badboys++;
          }
          if (!(std::count(badPackets[k - 2].begin(), badPackets[k - 2].end(), badPackets[k][j])) && !(std::count(badPackets[k - 1].begin(), badPackets[k - 1].end(), badPackets[k][j])))
          {
            badPacks->AddEntry("", Form("%d", badPackets[k].at(j)), "");
            badboys++;
          }
        }
      }
    }
  }
  badPacks->Draw();
  title->Draw();
  TPaveText *desc = new TPaveText(0, 0, 1, 0.25);
  desc->AddText(Form("Currently %.2g%% of packets are reporting a problem", badboys / 128. * 100));
  desc->AddText("Packets will be reported bad as above for the following reasons:");
  desc->AddText(Form("A packet appears in less than %g %% of events", param * 100));
  // desc -> AddText(Form("A packet is less than %g %% of size 5981",param*100));
  desc->AddText(Form("A packet sees fewer than %g %% of 192 channels", param * 100));
  desc->Draw();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.02);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::ostringstream runnostream2;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string

  runnostream << "Packet Information";
  runnostream2 << " Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);
  transparent[1]->cd();

  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, .99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, .966, runstring.c_str());
  TC[1]->Update();
  TC[1]->Show();
  TC[1]->SetEditable(false);
  if (save)
  {
    TC[1]->SaveAs("plots/packets.pdf");
  }
  return 0;
}

int CemcMonDraw::DrawThird(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *h2_waveform_twrAvg[m_ServerSet.size()];
  int start[3];
  start[0] = -1;
  int i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h2_waveform_twrAvg[i] = (TH2 *) cl->getHisto(*server, "h2_waveform_twrAvg");
    if (h2_waveform_twrAvg[i] && start[0] == -1)
    {
      start[0] = i;
    }
    if (start[0] > -1 && h2_waveform_twrAvg[i])
    {
      h2_waveform_twrAvg[i]->SetName(Form("h2_waveform_twrAvg_%d", i));
      h2_waveform_twrAvg[start[0]]->Add(h2_waveform_twrAvg[i], 1);
    }
    i++;
  }

  TH1 *h1_waveform_time[m_ServerSet.size()];
  start[1] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h1_waveform_time[i] =  cl->getHisto(*server, "h1_waveform_time");
    if (h1_waveform_time[i] && start[1] == -1)
    {
      start[1] = i;
    }
    if (start[1] > -1 && h1_waveform_time[i])
    {
      h1_waveform_time[i]->SetName(Form("h1_waveform_time_%d", i));
      h1_waveform_time[start[1]]->Add(h1_waveform_time[i], 1);
    }
    i++;
  }

  TH1 *h1_waveform_pedestal[m_ServerSet.size()];
  start[2] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h1_waveform_pedestal[i] =  cl->getHisto(*server, "h1_waveform_pedestal");

    if (h1_waveform_pedestal[i] && start[2] == -1)
    {
      start[2] = i;
    }
    if (start[2] > -1 && h1_waveform_pedestal[i])
    {
      h1_waveform_pedestal[i]->SetName(Form("h1_waveform_pedestal_%d", i));
      h1_waveform_pedestal[start[2]]->Add(h1_waveform_pedestal[i], 1);
    }
    i++;
  }

  if (!gROOT->FindObject("CemcMon3"))
  {
    MakeCanvas("CemcMon3");
  }

  TC[2]->SetEditable(true);
  TC[2]->Clear("D");
  Pad[4]->cd();
  if (start[0] < 0 || start[1] < 0 || start[2] < 0)
  {
    DrawDeadServer(transparent[2]);
    TC[2]->SetEditable(false);
    return -1;
  }
  std::cout << "start drawing" << std::endl;
  gStyle->SetTitleFontSize(0.03);
  TProfile *profiled = h2_waveform_twrAvg[start[0]]->ProfileX();
  float ymaxp = profiled->GetMaximum();
  ymaxp = ymaxp * 20 > pow(2, 14) ? pow(2, 14) : 20 * ymaxp;
  h2_waveform_twrAvg[start[0]]->GetYaxis()->SetRangeUser(0, ymaxp);
  /*
  TProfile *profile_y = h2_waveform_twrAvg[start[0]]->ProfileY();
  profile_y->Rebin(5);

  // Define the range
  double x_min = 100;
  double x_max = 7 * ymaxp;
  int n_points_in_range = 0;
  // Loop through the bins of the Profile Y histogram and count the bins within the range
  int n_bins = profile_y->GetNbinsX();
  for (int j = 1; j <= n_bins; ++i)
  {
    double bin_center = profile_y->GetBinCenter(j);
    if (profile_y->GetBinContent(j) == 0)
    {
      continue;
    }
    if (bin_center >= x_min && bin_center <= x_max)
    {
      n_points_in_range++;
    }
  }

  double *x_vals = new double[n_points_in_range];
  double *y_vals = new double[n_points_in_range];

  // Extract the Profile Y values and their corresponding Y positions within the range
  int point_index = 0;
  for (int j = 1; j <= n_bins; ++j)
  {
    double bin_center = profile_y->GetBinCenter(j);
    if (profile_y->GetBinContent(j) == 0)
    {
      continue;
    }
    if (bin_center >= x_min && bin_center <= x_max)
    {
      y_vals[point_index] = bin_center;
      x_vals[point_index] = profile_y->GetBinContent(i);
      point_index++;
    }
  }
  std::cout<<"start drawing 2"<<std::endl;
  */
  float tsize = 0.06;
  float tsize2 = 0.08;

  h2_waveform_twrAvg[start[0]]->GetXaxis()->SetNdivisions(16);
  h2_waveform_twrAvg[start[0]]->GetXaxis()->SetTitle("sample #");
  h2_waveform_twrAvg[start[0]]->GetYaxis()->SetTitle("Waveform ADC [ADC]");
  h2_waveform_twrAvg[start[0]]->GetXaxis()->SetLabelSize(tsize2);
  h2_waveform_twrAvg[start[0]]->GetYaxis()->SetLabelSize(tsize2 - .01);
  h2_waveform_twrAvg[start[0]]->GetXaxis()->SetTitleSize(tsize2);
  h2_waveform_twrAvg[start[0]]->GetYaxis()->SetTitleSize(tsize);
  h2_waveform_twrAvg[start[0]]->GetXaxis()->SetTitleOffset(1.);
  h2_waveform_twrAvg[start[0]]->GetYaxis()->SetTitleOffset(1.25);
  // float ymaxp = 5000;
  h2_waveform_twrAvg[start[0]]->GetYaxis()->SetRangeUser(0, ymaxp);

  
  gStyle->SetOptStat(0);
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.16);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();
  gStyle->SetPalette(kBird);
  h2_waveform_twrAvg[start[0]]->DrawCopy("colz");
  // over lay the profile draw only the marker
  /*
  TGraph* graph = new TGraph(n_points_in_range, x_vals, y_vals);
  graph->SetMarkerStyle(20);
  graph->SetMarkerSize(1);
  graph->SetMarkerColor(1);
  graph->Draw("P same");
  */
  TLine windowLow1(SampleLowBoundary, 0, SampleLowBoundary, ymaxp);
  windowLow1.SetLineWidth(3);
  windowLow1.DrawLine(SampleLowBoundary, 0, SampleLowBoundary, ymaxp);

  TLine windowHigh1(SampleHighBoundary, 0, SampleHighBoundary, ymaxp);
  windowHigh1.SetLineWidth(3);
  windowHigh1.DrawLine(SampleHighBoundary, 0, SampleHighBoundary, ymaxp);

  gStyle->SetPalette(57);
  gPad->SetLogz();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": Pulse fitting";
  runnostream2 << "Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);

  transparent[2]->cd();

  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());

  Pad[5]->cd();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();

  gStyle->SetTitleFontSize(0.06);

  h1_waveform_time[start[1]]->SetStats(kFALSE);
  h1_waveform_time[start[1]]->Draw("hist");
  h1_waveform_time[start[1]]->GetXaxis()->SetNdivisions(16);
  h1_waveform_time[start[1]]->GetXaxis()->SetTitle("Sample #");
  h1_waveform_time[start[1]]->GetYaxis()->SetTitle("Fraction of Towers");
  h1_waveform_time[start[1]]->GetXaxis()->SetLabelSize(tsize2);
  h1_waveform_time[start[1]]->GetYaxis()->SetLabelSize(tsize2);
  h1_waveform_time[start[1]]->GetXaxis()->SetTitleSize(tsize2);
  h1_waveform_time[start[1]]->GetYaxis()->SetTitleSize(tsize2);
  h1_waveform_time[start[1]]->GetXaxis()->SetTitleOffset(1.0);
  h1_waveform_time[start[1]]->GetYaxis()->SetTitleOffset(.85);
  h1_waveform_time[start[1]]->SetFillColorAlpha(kBlue, 0.1);
  gPad->Update();
  if (h1_waveform_time[start[1]]->GetEntries())
  {
    h1_waveform_time[start[1]]->Scale(1. / h1_waveform_time[start[1]]->GetEntries());
  }
  h1_waveform_time[start[1]]->GetYaxis()->SetRangeUser(0, 1.);


  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gStyle->SetOptStat(0);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();
  TLine windowLow2(SampleLowBoundary, 0, SampleLowBoundary, gPad->GetFrame()->GetY2());
  windowLow2.SetLineWidth(3);
  windowLow2.DrawLine(SampleLowBoundary, 0, SampleLowBoundary, gPad->GetFrame()->GetY2());

  TLine windowHigh2(SampleHighBoundary, 0, SampleHighBoundary, gPad->GetFrame()->GetY2());
  windowHigh2.SetLineWidth(3);
  windowHigh2.DrawLine(SampleHighBoundary, 0, SampleHighBoundary, gPad->GetFrame()->GetY2());

  TLine meantime(h1_waveform_time[start[1]]->GetMean(), 0, h1_waveform_time[start[1]]->GetMean(), gPad->GetFrame()->GetY2());
  meantime.SetLineWidth(3);
  meantime.SetLineColor(kRed);
  meantime.DrawLine(h1_waveform_time[start[1]]->GetMean(), 0, h1_waveform_time[start[1]]->GetMean(), gPad->GetFrame()->GetY2());

  Pad[6]->cd();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();

  gPad->SetLogy(kFALSE);
  gStyle->SetTitleFontSize(0.06);
  h1_waveform_pedestal[start[2]]->SetStats(false);
  h1_waveform_pedestal[start[2]]->Draw("hist");
  h1_waveform_pedestal[start[2]]->GetXaxis()->SetNdivisions(505, kTRUE);
  h1_waveform_pedestal[start[2]]->GetXaxis()->SetTitle("ADC Pedestal");
  h1_waveform_pedestal[start[2]]->GetYaxis()->SetTitle("Fraction of Towers");
  h1_waveform_pedestal[start[2]]->GetXaxis()->SetLabelSize(tsize2);
  h1_waveform_pedestal[start[2]]->GetYaxis()->SetLabelSize(tsize2);
  h1_waveform_pedestal[start[2]]->GetXaxis()->SetTitleSize(tsize2);
  h1_waveform_pedestal[start[2]]->GetYaxis()->SetTitleSize(tsize2);
  h1_waveform_pedestal[start[2]]->GetXaxis()->SetTitleOffset(1);
  h1_waveform_pedestal[start[2]]->GetYaxis()->SetTitleOffset(0.85);
  h1_waveform_pedestal[start[2]]->SetFillColorAlpha(kBlue, 0.1);
  gPad->Update();

  TLine windowLow3(1000, 0, 1000, gPad->GetFrame()->GetY2());
  windowLow3.SetLineWidth(3);
  windowLow3.DrawLine(1000, 0, 1000, gPad->GetFrame()->GetY2());

  TLine windowHigh3(2000, 0, 2000, gPad->GetFrame()->GetY2());
  windowHigh3.SetLineWidth(3);
  windowHigh3.DrawLine(2000, 0, 2000, gPad->GetFrame()->GetY2());

  gPad->SetLogy();
  gPad->Update();

  if (h1_waveform_pedestal[start[2]]->GetEntries())
  {
    h1_waveform_pedestal[start[2]]->Scale(1. / h1_waveform_pedestal[start[2]]->GetEntries());
  }
  // h1_waveform_pedestal -> GetXaxis() -> SetRangeUser(1000,2000);
  gStyle->SetOptStat(0);

  TC[2]->Update();
  // gStyle->SetOptStat(0);

  TC[2]->Show();
  // gStyle->SetOptStat(0);

  TC[2]->SetEditable(false);
  // gStyle->SetOptStat(0);

  if (save)
  {
    TC[2]->SaveAs("plots/waveform.pdf");
  }
  // gStyle->SetOptStat(0);

  return 0;
}

int CemcMonDraw::DrawFourth(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH1 *h_waveform_sigDiff[m_ServerSet.size()];
  int start[3];
  start[0] = -1;
  int i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_waveform_sigDiff[i] =  cl->getHisto(*server, "h1_fitting_sigDiff");
    if (h_waveform_sigDiff[i] && start[0] == -1)
    {
      start[0] = i;
    }
    if (start[0] > -1 && h_waveform_sigDiff[i])
    {
      h_waveform_sigDiff[i]->SetName(Form("h_fitting_sigDiff_%d", i));
      h_waveform_sigDiff[start[0]]->Add(h_waveform_sigDiff[i], 1);
    }
    i++;
  }

  TH1 *h_waveform_pedDiff[m_ServerSet.size()];
  start[1] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_waveform_pedDiff[i] =  cl->getHisto(*server, "h1_fitting_pedDiff");
    if (h_waveform_pedDiff[i] && start[1] == -1)
    {
      start[1] = i;
    }
    if (start[1] > -1 && h_waveform_pedDiff[i])
    {
      h_waveform_pedDiff[i]->SetName(Form("h_fitting_pedDiff_%d", i));
      h_waveform_pedDiff[start[1]]->Add(h_waveform_pedDiff[i], 1);
    }
    i++;
  }

  TH1 *h_waveform_timeDiff[m_ServerSet.size()];
  start[2] = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_waveform_timeDiff[i] =  cl->getHisto(*server, "h1_fitting_timeDiff");
    if (h_waveform_timeDiff[i] && start[2] == -1)
    {
      start[2] = i;
    }
    if (start[2] > -1 && h_waveform_timeDiff[i])
    {
      h_waveform_timeDiff[i]->SetName(Form("h_fitting_timeDiff_%d", i));
      h_waveform_timeDiff[start[2]]->Add(h_waveform_timeDiff[i], 1);
    }
    i++;
  }

  if (!gROOT->FindObject("CemcMon4"))
  {
    MakeCanvas("CemcMon4");
  }

  TC[3]->SetEditable(true);
  TC[3]->Clear("D");
  if (start[0] < 0 || start[1] < 0 || start[2] < 0)
  {
    DrawDeadServer(transparent[3]);
    TC[3]->SetEditable(false);
    return -1;
  }

  gStyle->SetTitleFontSize(0.03);

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "Waveform Template vs. Fast Fitting";
  runnostream2 << "Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);

  transparent[3]->cd();

  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());

  Pad[7]->cd();

  gStyle->SetTitleFontSize(0.06);

  float tsize2 = 0.08;
  h_waveform_sigDiff[start[0]]->GetXaxis()->SetNdivisions(16);
  h_waveform_sigDiff[start[0]]->GetXaxis()->SetTitle("Fast ADC/Template ADC");
  h_waveform_sigDiff[start[0]]->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_sigDiff[start[0]]->GetXaxis()->SetLabelSize(tsize2 - .01);
  h_waveform_sigDiff[start[0]]->GetYaxis()->SetLabelSize(tsize2 - .01);
  h_waveform_sigDiff[start[0]]->GetXaxis()->SetTitleSize(tsize2 - .01);
  h_waveform_sigDiff[start[0]]->GetYaxis()->SetTitleSize(tsize2 - .01);
  h_waveform_sigDiff[start[0]]->GetXaxis()->SetTitleOffset(.9);
  h_waveform_sigDiff[start[0]]->GetYaxis()->SetTitleOffset(1.1);
  if (h_waveform_sigDiff[start[0]]->GetEntries())
  {
    h_waveform_sigDiff[start[0]]->Scale(1. / h_waveform_sigDiff[start[0]]->GetEntries());
  }
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();
  h_waveform_sigDiff[start[0]]->DrawCopy("hist");

  Pad[8]->cd();

  gStyle->SetTitleFontSize(0.06);

  h_waveform_pedDiff[start[1]]->GetXaxis()->SetNdivisions(16);
  h_waveform_pedDiff[start[1]]->GetXaxis()->SetTitle("Fast Pedestal/Template Pedestal");
  h_waveform_pedDiff[start[1]]->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_pedDiff[start[1]]->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_pedDiff[start[1]]->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_pedDiff[start[1]]->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_pedDiff[start[1]]->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_pedDiff[start[1]]->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_pedDiff[start[1]]->GetYaxis()->SetTitleOffset(0.9);
  if (h_waveform_pedDiff[start[1]]->GetEntries())
  {
    h_waveform_pedDiff[start[1]]->Scale(1. / h_waveform_pedDiff[start[1]]->GetEntries());
  }
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();
  h_waveform_pedDiff[start[1]]->DrawCopy("hist");

  Pad[9]->cd();

  h_waveform_timeDiff[start[2]]->GetXaxis()->SetNdivisions(16);
  h_waveform_timeDiff[start[2]]->GetXaxis()->SetTitle("Fast Peak Time - Template Peak Time");
  h_waveform_timeDiff[start[2]]->GetYaxis()->SetTitle("Fraction of Towers");
  h_waveform_timeDiff[start[2]]->GetXaxis()->SetLabelSize(tsize2);
  h_waveform_timeDiff[start[2]]->GetYaxis()->SetLabelSize(tsize2);
  h_waveform_timeDiff[start[2]]->GetXaxis()->SetTitleSize(tsize2);
  h_waveform_timeDiff[start[2]]->GetYaxis()->SetTitleSize(tsize2);
  h_waveform_timeDiff[start[2]]->GetXaxis()->SetTitleOffset(0.9);
  h_waveform_timeDiff[start[2]]->GetYaxis()->SetTitleOffset(0.9);
  if (h_waveform_timeDiff[start[2]]->GetEntries())
  {
    h_waveform_timeDiff[start[2]]->Scale(1. / h_waveform_timeDiff[start[2]]->GetEntries());
  }
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gPad->SetTicky();
  gPad->SetTickx();
  h_waveform_timeDiff[start[2]]->DrawCopy("hist");

  TC[3]->Update();
  TC[3]->Show();
  TC[3]->SetEditable(false);
  gStyle->SetOptStat(0);
  if (save)
  {
    TC[3]->SaveAs("plots/waveformExpert.pdf");
  }
  return 0;
}

int CemcMonDraw::DrawFifth(const std::string & /* what */)
{
  OnlMonClient *cl = OnlMonClient::instance();

  TH2 *h_cemc_hits_trig[64][m_ServerSet.size()];
  int start_trig[64] = {-1};
  for (int i = 1; i < 64; i++)
  {
    start_trig[i] = -1;
  }
  int i = 0;

  for (int itrig = 0; itrig < 64; itrig++)
  {
    i = 0;

    for (auto server = ServerBegin(); server != ServerEnd(); ++server)
    {
      h_cemc_hits_trig[itrig][i] = (TH2 *) cl->getHisto(*server, Form("h2_cemc_hits_trig_bit_%d", itrig));
      if (h_cemc_hits_trig[itrig][i] && start_trig[itrig] == -1)
      {
        start_trig[itrig] = i;
      }
      if (start_trig[itrig] > -1 && h_cemc_hits_trig[itrig][i])
      {
        h_cemc_hits_trig[itrig][i]->SetName(Form("h_cemc_hits_trig_bit_%d_%d", itrig, i));
        if (start_trig[itrig] != i)
        {
          h_cemc_hits_trig[itrig][start_trig[itrig]]->Add(h_cemc_hits_trig[itrig][i], 1);
        }
      }
      i++;
    }
  }

  TH1 *h_cemc_trig[m_ServerSet.size()];
  int start = -1;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_cemc_trig[i] = cl->getHisto(*server, "h1_cemc_trig");
    if (h_cemc_trig[i] && start == -1)
    {
      start = i;
    }
    if (start > -1 && h_cemc_trig[i])
    {
      h_cemc_trig[i]->SetName(Form("h_cemc_trig_%d", i));
      if (start != i)
      {
        h_cemc_trig[start]->Add(h_cemc_trig[i], 1);
      }
    }
    i++;
  }

  TProfile *h_evtRec;
  i = 0;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    h_evtRec = (TProfile *) cl->getHisto(*server, "h_evtRec");
    if (h_evtRec)
    {
      break;
    }
  }
  
  if (!gROOT->FindObject("CemcMon5"))
  {
    MakeCanvas("CemcMon5");
  }

  TC[4]->SetEditable(true);
  TC[4]->Clear("D");
  if (!h_cemc_trig[start])
  {
    DrawDeadServer(transparent[4]);
    TC[4]->SetEditable(false);
    return -1;
  }

  for (int itrig = 0; itrig < 64; itrig++)
  {
    if (!h_cemc_hits_trig[itrig][start_trig[itrig]])
    {
      DrawDeadServer(transparent[4]);
      TC[4]->SetEditable(false);
      return -1;
    }
  }

  // vector of pairs (Number of entries, Trigger bit)
  std::vector<std::pair<float, int>> n_entries;

  for (int itrig = 0; itrig < 64; itrig++)
  {
    n_entries.push_back(std::make_pair(h_cemc_hits_trig[itrig][start_trig[itrig]]->GetEntries(), itrig));
  }

  // Sort it in ascending order of entries
  std::sort(n_entries.begin(), n_entries.end());
  // Reverse it to get the vector in descending order
  std::reverse(n_entries.begin(), n_entries.end());

  // Get the 4 priority trigger bits to be displayed
  std::vector<int> priority_triggers;

  for (int itrig = 0; itrig < 64; itrig++)
  {
    // Priority to the bits between 24 and 31 (draw photon trig first)
    if (n_entries[itrig].second >= 24 && n_entries[itrig].second <= 31)
    {
      if (n_entries[itrig].first > 0. && priority_triggers.size() < 4)
      {
        priority_triggers.push_back(n_entries[itrig].second);
      }
    }
  }

  // If trigger bits from 24 to 31 do not have 4 with entries, plot the others
  if (priority_triggers.size() < 4)
  {
    for (int itrig = 0; itrig < 64; itrig++)
    {
      if (priority_triggers.size() < 4 && n_entries[itrig].second < 24)
      {
        priority_triggers.push_back(n_entries[itrig].second);
      }
    }
  }

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": Trigger Info, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[4]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  Pad[10]->cd();
  gStyle->SetTitleFontSize(0.06);

  float tsize = 0.08;
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetXaxis()->SetTitle(Form("(trigger bit %d)  ieta", priority_triggers[0]));
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetYaxis()->SetTitle("iphi");
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetXaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetYaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetXaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetYaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetXaxis()->SetTitleOffset(1.2);
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->GetYaxis()->SetTitleOffset(0.75);
  gPad->SetLogz();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();
  h_cemc_hits_trig[priority_triggers[0]][start_trig[priority_triggers[0]]]->Draw("colz");

  Pad[11]->cd();
  gStyle->SetTitleFontSize(0.06);

  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetXaxis()->SetTitle(Form("(trigger bit %d)  ieta", priority_triggers[1]));
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetYaxis()->SetTitle("iphi");
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetXaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetYaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetXaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetYaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetXaxis()->SetTitleOffset(1.2);
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->GetYaxis()->SetTitleOffset(0.75);
  gPad->SetLogz();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();
  h_cemc_hits_trig[priority_triggers[1]][start_trig[priority_triggers[1]]]->Draw("colz");

  Pad[12]->cd();
  gStyle->SetTitleFontSize(0.06);

  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetXaxis()->SetTitle(Form("(trigger bit %d)  ieta", priority_triggers[2]));
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetYaxis()->SetTitle("iphi");
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetXaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetYaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetXaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetYaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetXaxis()->SetTitleOffset(1.2);
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->GetYaxis()->SetTitleOffset(0.75);
  gPad->SetLogz();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();
  h_cemc_hits_trig[priority_triggers[2]][start_trig[priority_triggers[2]]]->Draw("colz");

  Pad[13]->cd();
  gStyle->SetTitleFontSize(0.06);

  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetXaxis()->SetNdivisions(510, kTRUE);
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetXaxis()->SetTitle(Form("(trigger bit %d)  ieta", priority_triggers[3]));
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetYaxis()->SetTitle("iphi");
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetXaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetYaxis()->SetLabelSize(tsize);
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetXaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetYaxis()->SetTitleSize(tsize);
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetXaxis()->SetTitleOffset(1.2);
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->GetYaxis()->SetTitleOffset(0.75);
  gPad->SetLogz();
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(57);
  gPad->SetTicky();
  gPad->SetTickx();
  h_cemc_hits_trig[priority_triggers[3]][start_trig[priority_triggers[3]]]->Draw("colz");

  Pad[14]->cd();
  gStyle->SetTitleFontSize(0.06);

  float nEvtRec = 0.;

  if (h_evtRec)
  {
    nEvtRec = h_evtRec->GetBinContent(1);
  }

  h_cemc_trig[start]->SetTitle(Form("Receiving %0.3f of events from event reciever", nEvtRec));
  h_cemc_trig[start]->GetXaxis()->SetNdivisions(510, kTRUE);
  h_cemc_trig[start]->GetXaxis()->SetTitle("trigger index");
  h_cemc_trig[start]->GetYaxis()->SetTitle("events");
  h_cemc_trig[start]->GetXaxis()->SetLabelSize(tsize);
  h_cemc_trig[start]->GetYaxis()->SetLabelSize(tsize);
  h_cemc_trig[start]->GetXaxis()->SetTitleSize(tsize);
  h_cemc_trig[start]->GetYaxis()->SetTitleSize(tsize);
  h_cemc_trig[start]->GetXaxis()->SetTitleOffset(0.9);
  h_cemc_trig[start]->GetYaxis()->SetTitleOffset(0.85);
  gPad->SetTopMargin(0.06);
  gPad->SetBottomMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetLeftMargin(0.15);
  gStyle->SetOptStat(0);
  gPad->SetTicky();
  gPad->SetTickx();
  h_cemc_trig[start]->Draw("histo");

  TC[4]->Update();
  TC[4]->Show();
  TC[4]->SetEditable(false);

  return 0;
}

int CemcMonDraw::FindHotTower(TPad *warningpad, TH2 *hhit, bool usetemplate)
{
  float nhott = 0;
  float ndeadt = 0;
  float ncoldt = 0;
  int displaylimit = 10;
  std::ostringstream hottowerlist;
  std::ostringstream deadtowerlist;
  std::ostringstream coldtowerlist;
  float hot_threshold = 2.0;
  float dead_threshold = 0.01;
  float cold_threshold = 0.5;
  if(!usetemplate){
    float mean = 0;
    float rms = 0;
    int ntower = 0;
    for (int ieta = 0; ieta < nTowersEta; ieta++)
    {
      for (int iphi = 0; iphi < nTowersPhi; iphi++)
      {
        if ((ieta < 40 && ieta >= 32) && ((iphi >= 144) && (iphi < 152)))
        {
          continue;  // uninstrumented
        }
        else if ((ieta < 64 && ieta >= 56) && ((iphi >= 32) && (iphi < 40)))
        {
          continue;  // uninstrumented
        }
        if (hhit->GetBinContent(ieta + 1, iphi + 1) == 0)
        {
          continue;
        }
        double nhit = hhit->GetBinContent(ieta + 1, iphi + 1);
        mean += nhit;
        rms += nhit * nhit;
        ntower++;
      }
    }
    mean /= ntower;
    rms = sqrt(rms / ntower - mean * mean);
    hot_threshold = mean + 10 * rms;
    cold_threshold = mean - 3 * rms;
    dead_threshold = 0.01*mean;
  }


  // float nTowerTotal = 24576. - 384.;  // to account for the non-functioning towers at the edge of the south
  for (int ieta = 0; ieta < nTowersEta; ieta++)
  {
    for (int iphi = 0; iphi < nTowersPhi; iphi++)
    {
      if ((ieta < 40 && ieta >= 32) && ((iphi >= 144) && (iphi < 152)))
      {
        continue;  // uninstrumented
      }
      else if ((ieta < 64 && ieta >= 56) && ((iphi >= 32) && (iphi < 40)))
      {
        continue;  // uninstrumented
      }
      if (hhit->GetBinContent(ieta + 1, iphi + 1) == 0)
      {
        continue;
      }
      double nhit = hhit->GetBinContent(ieta + 1, iphi + 1);

      if (nhit > hot_threshold)
      {
        if (nhott <= displaylimit)
        {
          hottowerlist << " (" << ieta << "," << iphi << ")";
        }
        nhott++;
      }

      if (nhit < dead_threshold)
      {
        if (ndeadt <= displaylimit)
        {
          deadtowerlist << " (" << ieta << "," << iphi << ")";
        }
        ndeadt++;
      }
      else if (nhit < cold_threshold)
      {
        if (ncoldt <= displaylimit)
        {
          coldtowerlist << " (" << ieta << "," << iphi << ")";
        }
        ncoldt++;
      }
    }
  }

  if (nhott > displaylimit)
  {
    hottowerlist << "... " << nhott << " total";
  }
  if (ndeadt > displaylimit)
  {
    deadtowerlist << "... " << ndeadt << " total";
  }
  if (ncoldt > displaylimit)
  {
    coldtowerlist << "... " << ncoldt << " total";
  }
  if (nhott == 0)
  {
    hottowerlist << " None";
  }
  if (ndeadt == 0)
  {
    deadtowerlist << " None";
  }
  if (ncoldt == 0)
  {
    coldtowerlist << " None";
  }
  // draw warning here
  warningpad->cd();
  // TPaveText *dead = new TPaveText(0.01, 0.7, 0.33, 1);
  // dead->SetFillColor(kGray + 2);
  // dead->SetTextColor(kWhite);
  // dead->AddText(Form("Cold towers: %.3g%%", 100 * ndeadt / nTowerTotal));
  // TPaveText *good = new TPaveText(0.33, 0.7, 0.66, 1);
  // good->SetFillColor(kGreen + 1);
  // good->AddText(Form("Good towers: %.3g%%", 100 * (nTowerTotal - ndeadt - nhott) / nTowerTotal));
  // TPaveText *hot = new TPaveText(0.66, 0.7, 1, 1);
  // hot->SetFillColor(kRed - 9);
  // hot->AddText(Form("Hot towers: %.3g%%", 100 * nhott / nTowerTotal));
  // TPaveText *warn = new TPaveText(0.01, 0.1, 1, 0.7);
  // warn->SetTextSize(0.1);
  // warn->AddText("Helpful Numbers: 1 Box = Interface Board (IB)");
  // warn->AddText("3 Boxes (horiz) = 1 Packet; 6 Boxes (horiz) = 1 Sector");
  // warn->AddText("For now, watch for entire IB's or sectors going dead");
  // dead->Draw();
  // good->Draw();
  // hot->Draw();
  // warn->Draw();
  TText warn;
  warn.SetTextFont(62);
  warn.SetTextSize(0.09);
  warn.SetTextColor(2);
  warn.SetNDC();
  warn.SetTextAlign(23);
  warn.DrawText(0.5, 0.9, "Hot towers (ieta,iphi):");
  warn.DrawText(0.5, 0.8, hottowerlist.str().c_str());

  warn.SetTextColor(1);
  warn.SetTextAlign(23);
  warn.DrawText(0.5, 0.3, "Dead towers (ieta,iphi):");
  warn.DrawText(0.5, 0.2, deadtowerlist.str().c_str());

  warn.SetTextColor(4);
  warn.SetTextAlign(23);
  warn.DrawText(0.5, 0.6, "Cold towers (ieta,iphi):");
  warn.DrawText(0.5, 0.5, coldtowerlist.str().c_str());  

  warningpad->Update();
  return 0;
}

int CemcMonDraw::SavePlot(const std::string &what, const std::string &type)
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

int CemcMonDraw::MakeHtml(const std::string &what)
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

std::vector<int> CemcMonDraw::getBadPackets(TH1 *hist, int what, float cutoff)
{
  float params[3] = {1., 5981., 192.};
  float params2[3] = {1., 3991., 128.};
  // float cutoff = 0.75;
  std::vector<int> badpacks = {0};
  std::vector<int> uninPackets = {6022, 6026, 6030, 6034, 6038, 6042};
  // 6022, 6026, 6030, 6034, 6038, 6042
  for (int i = 1; i < hist->GetNbinsX(); i++)
  {
    if (std::count(uninPackets.begin(), uninPackets.end(), i + 6000))
    {
      if ((hist->GetBinContent(i) < params2[what] * cutoff || hist->GetBinContent(i) > params2[what]))
      {
        badpacks.push_back(i + 6000);
      }
    }
    else
    {
      if ((hist->GetBinContent(i) < params[what] * cutoff || hist->GetBinContent(i) > params[what]))
      {
        badpacks.push_back(i + 6000);
      }
    }
  }

  // if((hist -> GetBinContent(i) < params[what]*cutoff ||  hist -> GetBinContent(i) > params[what]) &&  !(((6000 + i - 2.)/4.) == floor(((6000 + i - 2)/4))) ) badpacks.push_back(i+6000);

  // else if((hist -> GetBinContent(i) < params2[what]*cutoff || hist -> GetBinContent(i) > params2[what]) &&  (((6000 + i - 2.)/4.) == floor(((6000 + i - 2)/4))) ) badpacks.push_back(i+6000);

  return badpacks;
}

int CemcMonDraw::DrawServerStats()
{
  OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("CemcMonServerStats"))
  {
    MakeCanvas("CemcMonServerStats");
  }
  TC[5]->Clear("D");
  TC[5]->SetEditable(true);
  transparent[5]->cd();
  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  PrintRun.SetTextSize(0.04);
  PrintRun.SetTextColor(1);
  PrintRun.DrawText(0.5, 0.99, "Server Statistics");

  PrintRun.SetTextSize(0.02);
  double vdist = 0.04;
  double vpos = 0.92;
  for (const auto &server : m_ServerSet)
  {
    std::ostringstream txt;
    auto servermapiter = cl->GetServerMap(server);
    if (servermapiter == cl->GetServerMapEnd())
    {
      txt << "Server " << server
          << " is dead ";
      PrintRun.SetTextColor(kRed);
    }
    else
    {
      int gl1counts = std::get<4>(servermapiter->second);
      txt << "Server " << server
          << ", run number " << std::get<1>(servermapiter->second)
          << ", event count: " << std::get<2>(servermapiter->second);
      if (gl1counts >= 0)
	{
          txt << ", gl1 count: " << std::get<4>(servermapiter->second);
	}
        txt  << ", current time " << ctime(&(std::get<3>(servermapiter->second)));
      if (std::get<0>(servermapiter->second))
      {
        PrintRun.SetTextColor(kGray + 2);
      }
      else
      {
        PrintRun.SetTextColor(kRed);
      }
    }
    PrintRun.DrawText(0.5, vpos, txt.str().c_str());
    vpos -= vdist;
  }
  TC[5]->Update();
  TC[5]->Show();
  TC[5]->SetEditable(false);

  return 0;
}

int CemcMonDraw::DrawBadChi2(const std::string & /* what */)
{
   OnlMonClient *cl = OnlMonClient::instance();
  if (!gROOT->FindObject("CemcBadChi2"))
  {
    MakeCanvas("CemcBadChi2");
  }
  TC[8]->Clear("D");
  TC[8]->SetEditable(true);
  transparent[8]->cd();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.03);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::ostringstream runnostream2;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << "Tower with bad chi2";
  runnostream2 << "Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);

  runstring = runnostream.str();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  runstring = runnostream2.str();
  PrintRun.DrawText(0.5, 0.966, runstring.c_str());

  TProfile2D *p2_bad_chi2 = nullptr;
  TProfile2D *proftmp;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    proftmp = (TProfile2D *) cl->getHisto(*server, "p2_bad_chi2");
    if (proftmp)
    {
      if (p2_bad_chi2)
      {
        p2_bad_chi2->Add(proftmp);
      }
      else
      {
        p2_bad_chi2 = proftmp;
      }
    }
  }
  Pad[21] -> cd();
  std::vector<std::pair<int, int>> badchi2;
  //loop over TProfile to find bad chi2
  for (int i = 1; i <= p2_bad_chi2->GetNbinsX(); i++)
  {
    for (int j = 1; j <= p2_bad_chi2->GetNbinsY(); j++)
    {
      float bad_chi2_prob = p2_bad_chi2->GetBinContent(i, j);
      if (bad_chi2_prob > 0.5)
      {
        //do stuff here find the sector and IB and display the text
        badchi2.push_back(std::make_pair(i-1, j-1));
      }
    }
  }
  //text display
  double vdist = 0.04; // Vertical distance between lines
  double vpos = 0.92;  // Starting vertical position
  int displayedTowers = 0;
  TText printChi2;
  printChi2.SetTextFont(62);
  printChi2.SetNDC();          // Set to normalized coordinates
  printChi2.SetTextAlign(23);  // Center/top alignment
  printChi2.SetTextSize(0.04);
  printChi2.SetTextColor(1);
  for (const auto& [x, y] : badchi2)
  {
    bool ifknownbad = false;
    for (const auto& [i, j] : hotChannels)
    {
      if (i == x && j == y)
      {
        ifknownbad = true;
        break;
      }
    }
    float badChi2Rate = p2_bad_chi2->GetBinContent(x + 1, y + 1); // Adjusting for ROOT's 1-based indexing
    std::ostringstream txt;
    txt << "Tower(" << x << "," << y << "): bad chi2 rate=" << badChi2Rate;
    // If the tower is known to be bad, draw it in red
    if (ifknownbad)
    {
      printChi2.SetTextColor(kRed);
    }
    else
    {
      printChi2.SetTextColor(kBlack);
    }
    
    printChi2.DrawText(0.5, vpos, txt.str().c_str());

    vpos -= vdist; // Move to the next line
    displayedTowers++;
    if (vpos < 0.1) break; // Prevent drawing outside the canvas
  }

  int remainingTowers = badchi2.size() - displayedTowers;
  if (remainingTowers > 0)
  {
    // Use TText to draw a message indicating how many more towers there are
    std::ostringstream moreTxt;
    moreTxt << "And " << remainingTowers << " more...";
    printChi2.DrawText(0.5, 0.05, moreTxt.str().c_str()); // Draw at the bottom
  }

  TC[8]->Update();
  TC[8]->Show();
  TC[8]->SetEditable(false);
 
  return 0;

}

int CemcMonDraw::DrawSeventh(const std::string &  what)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // watch the absolute insanity as we merge all these
  // histograms from across seven different machines

  TProfile2D *p2_zsFrac_etaphiCombined = nullptr;
  TProfile2D *proftmp;
  for (auto server = ServerBegin(); server != ServerEnd(); ++server)
  {
    if(what=="SEVENTH")
    {
      proftmp = (TProfile2D *) cl->getHisto(*server, "p2_zsFrac_etaphi");
    }
    else
    {
      proftmp = (TProfile2D *) cl->getHisto(*server, "p2_zsFrac_etaphi_all");
    }
    if (proftmp)
    {
      if (p2_zsFrac_etaphiCombined)
      {
        p2_zsFrac_etaphiCombined->Add(proftmp);
      }
      else
      {
        p2_zsFrac_etaphiCombined = proftmp;
      }
    }
  }

  if (!gROOT->FindObject("CemcMon8"))
  {
    MakeCanvas("CemcMon8");
  }
  TC[7]->SetEditable(true);
  TC[7]->Clear("D");
  if(what == "SEVENTH")
  {
    TC[7]->SetTitle("Channel unsuppressed event fraction");
  }
  else
  {
    TC[7]->SetTitle("Channel unsuppressed event fraction (all triggers)");
  }
  if (!p2_zsFrac_etaphiCombined)
  {
    DrawDeadServer(transparent[7]);
    TC[7]->SetEditable(false);
    return -1;
  }

  Pad[19]->cd();

  double sum = 0;
  int count = 0;

  //reset
  h1_zs->Reset();
  h1_zs_low->Reset();
  h1_zs_high->Reset();

  for (int i = 1; i <= p2_zsFrac_etaphiCombined->GetNbinsX(); i++)
  {
    for (int j = 1; j <= p2_zsFrac_etaphiCombined->GetNbinsY(); j++)
    {
      float rate = p2_zsFrac_etaphiCombined->GetBinContent(i + 1, j + 1);
      if (rate <= 0.04)
      {
        h1_zs_low->Fill(rate);
      }
      else if (rate > 0.2)
      {
        h1_zs_high->Fill(rate);
      }
      else
      {
        h1_zs->Fill(rate);
      }
      sum += rate;
      count++;
    }
  }
  
  double maxx = (sum / count) * 5 > 1.1 ? 1.1 : (sum / count) * 5;
  h1_zs->GetXaxis()->SetRangeUser(0, maxx);

  double averagezs = sum / count * 100;

  gPad->SetTopMargin(0.02);
  gPad->SetBottomMargin(0.12);
  gPad->SetLeftMargin(0.12);
  gPad->SetRightMargin(0.12);
  gStyle->SetTitleFontSize(0.06);
  //gStyle->SetPalette(57);
  gStyle->SetPalette(255, ZSPalette);
  gStyle->SetNumberContours(255);
  p2_zsFrac_etaphiCombined->GetXaxis()->SetTitle("eta index");
  p2_zsFrac_etaphiCombined->GetYaxis()->SetTitle("phi index");
  p2_zsFrac_etaphiCombined->SetTitle(Form("Average unsuppressed rate: %.3f%%", averagezs));
  p2_zsFrac_etaphiCombined->GetXaxis()->SetLabelSize(0.05);
  p2_zsFrac_etaphiCombined->GetYaxis()->SetLabelSize(0.05);
  p2_zsFrac_etaphiCombined->GetXaxis()->SetTitleSize(0.05);
  p2_zsFrac_etaphiCombined->GetYaxis()->SetTitleSize(0.05);
  p2_zsFrac_etaphiCombined->GetXaxis()->SetTitleOffset(1.0);
  p2_zsFrac_etaphiCombined->GetYaxis()->SetTitleOffset(1.0);
  p2_zsFrac_etaphiCombined->SetMinimum(0);
  p2_zsFrac_etaphiCombined->SetMaximum(1);
  p2_zsFrac_etaphiCombined->SetStats(kFALSE);
  p2_zsFrac_etaphiCombined->DrawCopy("colz");

  Pad[20]->cd();
  gStyle->SetTitleFontSize(0.06);
  float tsize = 0.08;
  h1_zs->Draw();
  h1_zs->GetXaxis()->SetTitle("unsuppressed fraction");
  h1_zs->GetYaxis()->SetTitle("towers");
  h1_zs->GetXaxis()->SetLabelSize(tsize);
  h1_zs->GetYaxis()->SetLabelSize(tsize);
  h1_zs->GetXaxis()->SetTitleSize(tsize);
  h1_zs->GetYaxis()->SetTitleSize(tsize);
  h1_zs->GetXaxis()->SetTitleOffset(0.9);
  h1_zs->GetYaxis()->SetTitleOffset(0.85);
  h1_zs->GetXaxis()->SetNdivisions(510, kTRUE);
  h1_zs->SetFillColorAlpha(kBlue, 0.1);
  h1_zs_low->SetFillColorAlpha(kRed, 0.1);
  h1_zs_high->SetFillColorAlpha(kYellow, 0.1);
  h1_zs_low->Draw("same");
  h1_zs_high->Draw("same");
  gPad->SetBottomMargin(0.16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.15);
  gPad->SetTopMargin(0.1);
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
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  // fill run number and event time into string
  runnostream << ThisName << ": Unsuppressed event fraction, Run" << cl->RunNumber()
              << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[7]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  TC[7]->Update();
  TC[7]->Show();
  TC[7]->SetEditable(false);
  if (save) TC[7]->SaveAs("plots/UnsuppressedEventFraction.pdf");
  return 0;
}

/***********************
int CemcMonDraw::DrawSixth(const std::string &  ){

  TH2D* h2_maxima;
  TH2D* h2_timeofMax;
  TH2D* h2_pedestal;
  TH2D* h2_saturating;

  if(!gROOT->FindObject("h2_maxima")){
    h2_maxima = new TH2D("h2_maxima","ADC counts at peak position",96,0,96,256,0,256);
    h2_maxima->SetStats(kFALSE);
    h2_maxima->GetXaxis()->SetNdivisions(12,kFALSE);
    h2_maxima->GetYaxis()->SetNdivisions(32,kFALSE);
    h2_maxima->GetXaxis()->SetTitle("eta index");
    h2_maxima->GetYaxis()->SetTitle("phi index");
  }
  else {
    h2_maxima=(TH2D*)gROOT->FindObject("h2_maxima");
    h2_maxima->Reset();
  }
  if(!gROOT->FindObject("h2_timeOfMax")){
    h2_timeofMax=new TH2D("h2_timeOfMax","Waveform peak position",96,0,96,256,0,256);
    h2_timeofMax->SetStats(kFALSE);
    h2_timeofMax->GetXaxis()->SetNdivisions(12,kFALSE);
    h2_timeofMax->GetYaxis()->SetNdivisions(32,kFALSE);
    h2_timeofMax->GetXaxis()->SetTitle("eta index");
    h2_timeofMax->GetYaxis()->SetTitle("phi index");
  }
  else{
    h2_timeofMax=(TH2D*)gROOT->FindObject("h2_timeOfMax");
    h2_timeofMax->Reset();
  }
  if(!gROOT->FindObject("h2_pedestal")){
    h2_pedestal=new TH2D("h2_pedestal","ADC counts for pedestal",96,0,96,256,0,256);
    h2_pedestal->SetStats(kFALSE);
    h2_pedestal->GetXaxis()->SetNdivisions(12,kFALSE);
    h2_pedestal->GetYaxis()->SetNdivisions(32,kFALSE);
    h2_pedestal->GetXaxis()->SetTitle("eta index");
    h2_pedestal->GetYaxis()->SetTitle("phi index");
  }
  else{
    h2_pedestal=(TH2D*)gROOT->FindObject("h2_pedestal");
    h2_pedestal->Reset();
  }
  if(!gROOT->FindObject("h2_saturating")){
    h2_saturating=new TH2D("h2_saturating","Tower with saturated signals",96,0,96,256,0,256);
    h2_saturating->SetStats(kFALSE);
    h2_saturating->GetXaxis()->SetNdivisions(12,kFALSE);
    h2_saturating->GetYaxis()->SetNdivisions(32,kFALSE);
    h2_saturating->GetXaxis()->SetTitle("eta index");
    h2_saturating->GetYaxis()->SetTitle("phi index");
  }
  else{
    h2_saturating=(TH2D*)gROOT->FindObject("h2_saturating");
    h2_saturating->Reset();
  }

  if (!gROOT->FindObject("CemcMon7"))
    {
      MakeCanvas("CemcMon7");
    }
  OnlMonClient *cl = OnlMonClient::instance();
  for(int i=0; i<256; i++){
    for(int j=0; j<96; j++){
      double mean=0;
      int Nmean=0;
      bool unsat=true;
      int nSaturated=0;
      bool init=false;
      TProfile* ptmp=nullptr;
      for (auto server = ServerBegin(); server != ServerEnd(); ++server){
        //for(int iseb=0;iseb<nSEBs; iseb++){
        //ptmp=(TProfile*)cl->getHisto(Form("CEMCMON_%d",iseb),Form("h2_waveform_phi%d_eta%d",i,j));
        ptmp=(TProfile*)cl->getHisto(*server,Form("h2_waveform_phi%d_eta%d",i,j));
        if(ptmp){
          if(!init){
            if(!gROOT->FindObject(Form("Combined%s",ptmp->GetName()))){
              AllProfiles[i][j] = new TProfile(*(TProfile*)ptmp);//Is it really necessary? Most likely not
              AllProfiles[i][j]->SetName(Form("Combined%s",ptmp->GetName()));
              init=true;
            }
            else{
              AllProfiles[i][j]->Reset();
              AllProfiles[i][j]->Add(ptmp);
              init=true;
            }
          }
          else{
            AllProfiles[i][j]->Add(ptmp);
          }
        }
        else{
          std::cout<<"Could not retrieve "<<Form("h2_waveform_phi%d_eta%d",i,j)<<" from "<<*server<<std::endl;
        }
      }
      if(AllProfiles[i][j]){
        h2_maxima->Fill(j,i,AllProfiles[i][j]->GetMaximum());
        h2_timeofMax->Fill(j,i,AllProfiles[i][j]->GetXaxis()->GetBinCenter(AllProfiles[i][j]->GetMaximumBin()));
        if(AllProfiles[i][j]->GetBinError(AllProfiles[i][j]->GetMaximumBin())==0 && AllProfiles[i][j]->GetBinContent(AllProfiles[i][j]->GetMaximumBin())>0){
          unsat=false;
          nSaturated++;
        }
        for(int ibin=0; ibin<AllProfiles[i][j]->GetNbinsX(); ibin++){
          if(ibin+1==AllProfiles[i][j]->GetMaximumBin())continue;
          if(AllProfiles[i][j]->GetBinContent(ibin+1)/AllProfiles[i][j]->GetMaximum()>0.95&&AllProfiles[i][j]->GetBinContent(ibin+1)>10000){
            unsat=false;
            nSaturated++;
          }
        }
        for(int Rmean=1; Rmean<7; Rmean++){
          if(AllProfiles[i][j]->GetBinContent(Rmean)){
            mean+=AllProfiles[i][j]->GetBinContent(Rmean);
            Nmean++;
          }
        }
        h2_pedestal->Fill(j,i,Nmean>0?mean/Nmean:0);//AllProfiles[i][j]->GetBinContent(2));
        if(!unsat)h2_saturating->Fill(j,i,nSaturated);
      }
    }
  }
  TLine *line_sector[32];
  for(int i_line=0;i_line<32;i_line++)
    {
      line_sector[i_line] = new TLine(0,(i_line+1)*8,96,(i_line+1)*8);
      line_sector[i_line]->SetLineColor(1);
      line_sector[i_line]->SetLineWidth(1);
      line_sector[i_line]->SetLineStyle(1);
    }


  const int numVertDiv = 12;
  int dEI = 96/numVertDiv;
  TLine *l_board[numVertDiv-1];
  for(int il=1; il<numVertDiv; il++){
    l_board[il-1] = new TLine(dEI*il,0,dEI*il,256);
    l_board[il-1]->SetLineColor(1);
    l_board[il-1]->SetLineWidth(1);
    l_board[il-1]->SetLineStyle(1);
    if(il==6) l_board[il-1]->SetLineWidth(2);
  }

  TC[6]->SetEditable(1);
  TC[6]->Clear("D");
  Pad[15]->cd();
  gStyle->SetPalette(kBird);
  h2_maxima->DrawCopy("colz");
  for(int i_line=0;i_line<32;i_line++) line_sector[i_line]->Draw();
  for(int il=0; il<numVertDiv-1; il++) l_board[il]->Draw();
  Pad[16]->cd();
  gStyle->SetPalette(kBird);
  h2_timeofMax->DrawCopy("colz");
  for(int i_line=0;i_line<32;i_line++) line_sector[i_line]->Draw();
  for(int il=0; il<numVertDiv-1; il++) l_board[il]->Draw();
  Pad[17]->cd();
  gStyle->SetPalette(kBird);
  h2_pedestal->DrawCopy("colz");
  for(int i_line=0;i_line<32;i_line++) line_sector[i_line]->Draw();
  for(int il=0; il<numVertDiv-1; il++) l_board[il]->Draw();
  Pad[18]->cd();
  gStyle->SetPalette(kBird);
  h2_saturating->DrawCopy("colz");
  for(int i_line=0;i_line<32;i_line++) line_sector[i_line]->Draw();
  for(int il=0; il<numVertDiv-1; il++) l_board[il]->Draw();

  TText PrintRun;
  PrintRun.SetTextFont(62);
  PrintRun.SetTextSize(0.04);
  PrintRun.SetNDC();          // set to normalized coordinates
  PrintRun.SetTextAlign(23);  // center/top alignment
  std::ostringstream runnostream;
  std::string runstring;
  std::pair<time_t,int> evttime = cl->EventTime("CURRENT");
  runnostream << "Run " << cl->RunNumber() << ", Time: " << ctime(&evttime.first);
  runstring = runnostream.str();
  transparent[6]->cd();
  PrintRun.SetTextColor(evttime.second);
  PrintRun.DrawText(0.5, 0.99, runstring.c_str());

  TC[6]->Update();
  TC[6]->Show();
  TC[6]->SetEditable(0);
  TC[6]->Connect("ProcessedEvent(int, int, int, TObject*)","CemcMonDraw",this,"HandleEvent(int, int, int, TObject*)");
  if(save)TC[6] -> SaveAs("plots/Test.pdf");
  return 0;
}

void CemcMonDraw::HandleEvent(int event, int x, int y, TObject* sel){
  if(event==1){
    TCanvas* canvas=nullptr;
    TPad *mypad=nullptr;
    if(strcmp(sel->GetName(),"transparent0")==0){
      //canvas=TC[0];
      //mypad=Pad[0];
      return;//we do not want draw from there
    }
    else if(strcmp(sel->GetName(),"transparent6")==0){
      canvas=TC[6];
      if(canvas->AbsPixeltoX(x)<0.5){
        if(canvas->AbsPixeltoY(y)<0.475){
          mypad=Pad[17];
        }
        else{
          mypad=Pad[15];
        }
      }
      else{
        if(canvas->AbsPixeltoY(y)<0.475){
          mypad=Pad[18];
        }
        else{
          mypad=Pad[16];
        }
      }

    }
    else return;//dummy to avoid the unused warning

    double xeta=mypad->PadtoX(mypad->AbsPixeltoX(x));
    double yphi=mypad->PadtoY(mypad->AbsPixeltoY(y));
    int ieta=(int) xeta;
    int iphi=(int) yphi;
    if(ieta<0||ieta>96) return;
    if(iphi<0||iphi>256) return;
    int iphi_begin=(iphi/8)*8;
    int ieta_begin=(ieta/8)*8;
    if(!gROOT->FindObject("CemcPopup")) MakeCanvas("CemcPopup");
    PopUpCanvas->SetEditable(true);
    PopUpCanvas->Clear("D");
    for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
        if(!gROOT->FindObject(Form("Combinedh2_waveform_phi%d_eta%d",j+iphi_begin,i+ieta_begin))){
          return ;
          PopUpCanvas->Update();
          PopUpCanvas->Show();
          PopUpCanvas->SetEditable(false);
        }
        else{
          summedProfile[i][j]=(TProfile*)gROOT->FindObject(Form("Combinedh2_waveform_phi%d_eta%d",j+iphi_begin,i+ieta_begin));
        }
        if(PopUpPad[i][j]){
          PopUpPad[i][j]->cd();
        }
        else{
          std::cout<<"no valid popuppad"<<std::endl;
        }
        if(summedProfile[i][j]){
          summedProfile[i][j]->SetMaximum(pow(2,14));
          summedProfile[i][j]->DrawCopy();
          summedProfile[i][j]->SetMaximum(-1111);
          PopUpPad[i][j]->Update();
          PopUpPad[i][j]->Paint();
        }
        else{
          DrawDeadServer(PopUpTransparent);
          PopUpCanvas->SetEditable(false);
          return;
        }
      }
    }
    PopUpCanvas->Update();
    PopUpCanvas->Show();
    PopUpCanvas->SetEditable(false);
  }
}

***********************/
