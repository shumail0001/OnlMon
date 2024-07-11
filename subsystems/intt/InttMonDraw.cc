#include "InttMonDraw.h"

#include <TPolyLine.h>

InttMonDraw::InttMonDraw(const std::string& name)
  : OnlMonDraw(name)
{
}

InttMonDraw::~InttMonDraw()
{
  delete m_style;

  for (auto& i : m_hist_felixbcofphxbco)
  {
    for (auto& j : i)
    {
      delete j;
    }
  }

  for (auto& i : m_hist_hitmap)
  {
    delete i;
  }

  for (auto& m_hist_hitrate : m_hist_hitrates)
  {
    delete m_hist_hitrate;
  }
}

int InttMonDraw::Init()
{
  m_style = new TStyle("INTT_Style", "INTT_Style");
  m_style->SetOptStat(0);

  Int_t palette[3] = {kBlue, kGreen, kRed};
  m_style->SetPalette(3, palette);
  // m_style->SetNumberContours(3);

  return 0;
}

int InttMonDraw::Draw(const std::string& what)
{
  int idraw = 0;
  int iret = 0;

  if (what == "ALL" || what == "SERVERSTATS")
  {
    iret += DrawServerStats();
    ++idraw;
  }

  if (what == "ALL" || what == "chip_hitmap")
  {
    iret += Draw_HitMap();
    ++idraw;
  }

  if (what == "ALL" || what == "bco_diff")
  {
    iret += Draw_FelixBcoFphxBco();
    ++idraw;
  }

  if (what == "ALL" || what == "fphx_bco")
  {
    iret += Draw_JustFphxBco();
    ++idraw;
  }

  if (what == "ALL" || what == "zoomed_fphx_bco")
  {
    iret += Draw_ZoomedFphxBco();
    ++idraw;
  }

  // if (what == "ALL" || what == "peaks")
  // {
  //   iret += Draw_Peaks(icnvs);
  //   ++idraw;
  // }

  if (what == "ALL" || what == "hitrates")
  {
    iret += Draw_HitRates();
    ++idraw;
  }

  if (what == "ALL" || what == "history")
  {
    iret += Draw_History();
    ++idraw;
  }

  if (!idraw)
  {
    std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
              << "\tUnimplemented drawing option \"" << what << "\"" << std::endl;
  }
  if (std::fabs(iret) != idraw)  // at least one succeeded
  {
    return 0;
  }

  return iret;
}

int InttMonDraw::MakeHtml(const std::string& what)
{
  int iret = Draw(what);
  if (iret)
  {
    return iret;
  }

  OnlMonClient* cl = OnlMonClient::instance();
  int icnt = 0;
  for (TCanvas* canvas : TC)
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

  return 0;
}

int InttMonDraw::SavePlot(std::string const& what, std::string const& type)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int iret = Draw(what);
  if (iret)  // on error no png files please
  {
    return iret;
  }
  int icnt = 0;
  for (TCanvas* canvas : TC)
  {
    if (canvas == nullptr)
    {
      continue;
    }
    ++icnt;
    std::string filename = ThisName + "_" + std::to_string(icnt) + "_" + std::to_string(cl->RunNumber()) + "." + type;
    cl->CanvasToPng(canvas, filename);
  }
  return 0;
}

int InttMonDraw::MakeCanvas(const std::string& name)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  m_cnvs_width = xsize;
  m_cnvs_height = ysize;
  if (name == "InttMonServerStats")
  {
    TC[k_server_stats] = new TCanvas(name.c_str(), "InttMon Server Stats", xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    transparent[k_server_stats] = new TPad(Form("transparent%d", k_server_stats), "this does not show", 0, 0, 1, 1);
    transparent[k_server_stats]->SetFillColor(kGray);
    transparent[k_server_stats]->Draw();
    TC[k_server_stats]->SetEditable(false);
    TC[k_server_stats]->SetTopMargin(0.05);
    TC[k_server_stats]->SetBottomMargin(0.05);
  }
  if (name == "InttFelixBcoFphxBco")
  {
    TC[k_felixbcofphxbco] = new TCanvas(name.c_str(), "Felix Fphx Bco", m_cnvs_width, m_cnvs_height);
    gSystem->ProcessEvents();
    transparent[k_felixbcofphxbco] = new TPad(Form("transparent%d", k_felixbcofphxbco), "this does not show", 0, 0, 1, 1);
    transparent[k_felixbcofphxbco]->SetFillStyle(4000);  // Transparent
    transparent[k_felixbcofphxbco]->Draw();
    MakeDispPad(k_felixbcofphxbco, 0.15);
    TC[k_felixbcofphxbco]->SetEditable(false);
    TC[k_felixbcofphxbco]->SetTopMargin(0.05);
    TC[k_felixbcofphxbco]->SetBottomMargin(0.05);
  }
  if (name == "InttJustFphxBco")
  {
    TC[k_justfphxbco] = new TCanvas(name.c_str(), "Just Fphx Bco", m_cnvs_width, m_cnvs_height);
    gSystem->ProcessEvents();
    transparent[k_justfphxbco] = new TPad(Form("transparent%d", k_justfphxbco), "this does not show", 0, 0, 1, 1);
    transparent[k_justfphxbco]->SetFillStyle(4000);  // Transparent
    transparent[k_justfphxbco]->Draw();
    MakeDispPad(k_justfphxbco, 0.15);
    TC[k_justfphxbco]->SetEditable(false);
    TC[k_justfphxbco]->SetTopMargin(0.05);
    TC[k_justfphxbco]->SetBottomMargin(0.05);
  }
  if (name == "InttZoomedFphxBco")
  {
    TC[k_zoomedfphxbco] = new TCanvas(name.c_str(), "Zoomed Fphx Bco", m_cnvs_width, m_cnvs_height);
    gSystem->ProcessEvents();
    transparent[k_zoomedfphxbco] = new TPad(Form("transparent%d", k_zoomedfphxbco), "this does not show", 0, 0, 1, 1);
    transparent[k_zoomedfphxbco]->SetFillStyle(4000);  // Transparent
    transparent[k_zoomedfphxbco]->Draw();
    MakeDispPad(k_zoomedfphxbco, 0.15);
    TC[k_zoomedfphxbco]->SetEditable(false);
    TC[k_zoomedfphxbco]->SetTopMargin(0.05);
    TC[k_zoomedfphxbco]->SetBottomMargin(0.05);
  }
  if (name == "InttHitMap")
  {
    TC[k_hitmap] = new TCanvas(name.c_str(), "Intt Hit Map", m_cnvs_width, m_cnvs_height);
    gSystem->ProcessEvents();
    transparent[k_hitmap] = new TPad(Form("transparent%d", k_hitmap), "this does not show", 0, 0, 1, 1);
    transparent[k_hitmap]->SetFillStyle(4000);  // Transparent
    transparent[k_hitmap]->Draw();
    MakeDispPad(k_hitmap, 0.1);
    TC[k_hitmap]->SetEditable(false);
    TC[k_hitmap]->SetTopMargin(0.05);
    TC[k_hitmap]->SetBottomMargin(0.05);
  }
  if (name == "InttHitRates")
  {
    TC[k_hitrates] = new TCanvas(name.c_str(), "Intt Hit Rates", m_cnvs_width, m_cnvs_height);
    gSystem->ProcessEvents();
    transparent[k_hitrates] = new TPad(Form("transparent%d", k_hitrates), "this does not show", 0, 0, 1, 1);
    transparent[k_hitrates]->SetFillStyle(4000);  // Transparent
    transparent[k_hitrates]->Draw();
    MakeDispPad(k_hitrates);
    TC[k_hitrates]->SetEditable(false);
    TC[k_hitrates]->SetTopMargin(0.05);
    TC[k_hitrates]->SetBottomMargin(0.05);
  }
  if (name == "InttHistory")
  {
    TC[k_history] = new TCanvas(name.c_str(), "Intt History", m_cnvs_width, m_cnvs_height);
    gSystem->ProcessEvents();
    transparent[k_history] = new TPad(Form("transparent%d", k_history), "this does not show", 0, 0, 1, 1);
    transparent[k_history]->SetFillStyle(4000);  // Transparent
    transparent[k_history]->Draw();
    MakeDispPad(k_history, 0.15);
    TC[k_history]->SetEditable(false);
    TC[k_history]->SetTopMargin(0.05);
    TC[k_history]->SetBottomMargin(0.05);
  }
  return 0;
}

int InttMonDraw::DrawServerStats()
{
  OnlMonClient* cl = OnlMonClient::instance();
  if (!gROOT->FindObject("InttMonServerStats"))
  {
    MakeCanvas("InttMonServerStats");
  }
  TC[0]->Clear("D");
  TC[0]->SetEditable(true);
  transparent[0]->cd();
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
  for (const auto& server : m_ServerSet)
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
      txt << "Server " << server
          << ", run number " << std::get<1>(servermapiter->second)
          << ", event count: " << std::get<2>(servermapiter->second)
          << ", current time " << ctime(&(std::get<3>(servermapiter->second)));
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
  TC[0]->Update();
  TC[0]->Show();
  TC[0]->SetEditable(false);

  return 0;
}

int InttMonDraw::MakeDispPad(int icnvs, double lgnd_frac)
{
  std::string name = Form("intt_disp_pad_%d", icnvs);
  m_disp_pad[icnvs] = new TPad(
      name.c_str(), name.c_str(),  //
      0.0, 1.0 - m_disp_frac,      // Southwest x, y
      1.0, 1.0                     // Northeast x, y
  );
  TC[icnvs]->cd();
  m_disp_pad[icnvs]->SetFillStyle(4000);  // Transparent
  m_disp_pad[icnvs]->Range(0.0, 0.0, 1.0, 1.0);
  m_disp_pad[icnvs]->Draw();

  // Some methods do not need a legend, test this variable
  if (std::isfinite(lgnd_frac))
  {
    name = Form("intt_lgnd_pad_%d", icnvs);
    m_lgnd_pad[icnvs] = new TPad(
        name.c_str(), name.c_str(),  //
        1.0 - lgnd_frac, 0.0,        // Southwest x, y
        1.0, 1.0 - m_disp_frac       // Northeast x, y
    );
    TC[icnvs]->cd();
    m_lgnd_pad[icnvs]->SetFillStyle(4000);  // Transparent
    m_lgnd_pad[icnvs]->Range(0.0, 0.0, 1.0, 1.0);
    m_lgnd_pad[icnvs]->Draw();
  }
  else
  {
    lgnd_frac = 0.;
  }

  for (int i = 0; i < 8; ++i)
  {
    name = Form("intt_hist_pad_%d_%01d", icnvs, i);
    m_hist_pad[icnvs][i] = new TPad(
        name.c_str(), name.c_str(),                                                          //
        (i % 4 + 0.0) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac),  // Southwest x, y
        (i % 4 + 1.0) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)   // Northeast x, y
    );
    TC[icnvs]->cd();
    m_hist_pad[icnvs][i]->SetFillStyle(4000);  // Transparent
    m_hist_pad[icnvs][i]->Range(0.0, 0.0, 1.0, 1.0);
    m_hist_pad[icnvs][i]->Draw();

    name = Form("intt_left_hist_pad_%d_%01d", icnvs, i);
    m_left_hist_pad[icnvs][i] = new TPad(
        name.c_str(), name.c_str(),                                                          //
        (i % 4 + 0.0) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac),  // Southwest x, y
        (i % 4 + 0.5) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)   // Northeast x, y
    );
    TC[icnvs]->cd();
    m_left_hist_pad[icnvs][i]->SetFillStyle(4000);  // Transparent
    m_left_hist_pad[icnvs][i]->SetLeftMargin(0.15);
    m_left_hist_pad[icnvs][i]->SetRightMargin(0.01);
    m_left_hist_pad[icnvs][i]->Range(0.0, 0.0, 1.0, 1.0);
    m_left_hist_pad[icnvs][i]->Draw();

    name = Form("intt_right_hist_pad_%d_%01d", icnvs, i);
    m_right_hist_pad[icnvs][i] = new TPad(
        name.c_str(), name.c_str(),                                                          //
        (i % 4 + 0.5) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac),  // Southwest x, y
        (i % 4 + 1.0) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)   // Northeast x, y
    );
    TC[icnvs]->cd();
    m_right_hist_pad[icnvs][i]->SetFillStyle(4000);  // Transparent
    m_right_hist_pad[icnvs][i]->SetLeftMargin(0.01);
    m_right_hist_pad[icnvs][i]->Range(0.0, 0.0, 1.0, 1.0);
    m_right_hist_pad[icnvs][i]->Draw();

    name = Form("intt_transparent_pad_%d_%01d", icnvs, i);
    m_transparent_pad[icnvs][i] = new TPad(
        name.c_str(), name.c_str(),                                                          //
        (i % 4 + 0.0) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 0.0) / 2.0 * (1.0 - m_disp_frac),  // Southwest x, y
        (i % 4 + 1.0) / 4.0 * (1.0 - lgnd_frac), (i / 4 + 1.0) / 2.0 * (1.0 - m_disp_frac)   // Northeast x, y
    );
    TC[icnvs]->cd();
    m_transparent_pad[icnvs][i]->SetFillStyle(4000);  // Transparent
    m_transparent_pad[icnvs][i]->Range(0.0, 0.0, 1.0, 1.0);
    m_transparent_pad[icnvs][i]->Draw();
  }

  name = Form("intt_single_hist_pad_%d", icnvs);
  m_single_hist_pad[icnvs] = new TPad(
      name.c_str(), name.c_str(),        //
      0.0,             0.0,              // Southwest x, y
      1.0 - lgnd_frac, 1.0 - m_disp_frac // Southwest x, y

  );
  TC[icnvs]->cd();
  m_single_hist_pad[icnvs]->SetFillStyle(4000);  // Transparent
  m_single_hist_pad[icnvs]->Range(0.0, 0.0, 1.0, 1.0);
  m_single_hist_pad[icnvs]->Draw();

  name = Form("intt_single_transparent_pad_%d", icnvs);
  m_single_transparent_pad[icnvs] = new TPad(
      name.c_str(), name.c_str(),        //
      0.0,             0.0,              // Southwest x, y
      1.0 - lgnd_frac, 1.0 - m_disp_frac // Southwest x, y

  );
  TC[icnvs]->cd();
  m_single_transparent_pad[icnvs]->SetFillStyle(4000);  // Transparent
  m_single_transparent_pad[icnvs]->Range(0.0, 0.0, 1.0, 1.0);
  m_single_transparent_pad[icnvs]->Draw();

  return 0;
}

int InttMonDraw::DrawDispPad_Generic(int icnvs, const std::string& title)
{
  m_disp_pad[icnvs]->Clear();
  m_disp_pad[icnvs]->cd();

  OnlMonClient* cl = OnlMonClient::instance();

  // Get the "event" count from the first server we can
  TH1* evt_hist{nullptr};
  for (int i = 0; i < 8; ++i)
  {
    if ((evt_hist = cl->getHisto(Form("INTTMON_%d", i), "InttEvtHist")))
    {
      break;
    }
  }

  // If we can't find any, there is a problem
  if (!evt_hist)
  {
    DrawDeadServer(transparent[icnvs]);
    TC[icnvs]->SetEditable(false);
    if (isHtml())
    {
      delete TC[icnvs];
      TC[icnvs] = nullptr;
    }
    return -1;
  }

  // Title
  TText title_text;
  title_text.SetTextAlign(22);
  title_text.SetTextSize(m_disp_text_size);
  title_text.DrawText(0.5, 0.75, title.c_str());

  // Display text
  std::time_t evttime = cl->EventTime("CURRENT");  // BOR, CURRENT, or EOR
  std::string text = "Run " + std::to_string(cl->RunNumber()) + ", Events: " + std::to_string((int) evt_hist->GetBinContent(1)) + ", " + ctime(&evttime);
  TText disp_text;
  disp_text.SetTextAlign(22);
  disp_text.SetTextSize(m_disp_text_size);
  disp_text.DrawText(0.5, 0.5, text.c_str());

  // Disclaimer if not enough events
  if (m_min_events > evt_hist->GetBinContent(1))
  {
    TText warn_text;
    warn_text.SetTextAlign(22);
    warn_text.SetTextSize(m_warn_text_size);
    warn_text.SetTextColor(kRed);
    warn_text.DrawText(0.5, 0.25, Form("Not statistically significant (fewer than  %0.E events)", m_min_events));
  }
  return 0;
}

//====== FelixBcoFphxBco ======//

int InttMonDraw::Draw_FelixBcoFphxBco()
{
  if (!gROOT->FindObject("InttFelixBcoFphxBco"))
  {
    MakeCanvas("InttFelixBcoFphxBco");
  }

  TC[k_felixbcofphxbco]->SetEditable(true);
  m_style->cd();
  if(DrawDispPad_Generic(k_felixbcofphxbco, TC[k_felixbcofphxbco]->GetTitle()) == -1)
  {
    return -1;
  }

  // Draw Legend
  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad[k_felixbcofphxbco]->Clear();
  m_lgnd_pad[k_felixbcofphxbco]->cd();

  double x0, y0, x[4], y[4];
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, Form("FChn %2d", fee));

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box;
    box.SetFillColor(GetFeeColor(fee));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  int iret = 1;
  for (int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_FelixBcoFphxBco(i, k_felixbcofphxbco) && iret;
  }

  TC[k_felixbcofphxbco]->Update();
  TC[k_felixbcofphxbco]->Show();
  TC[k_felixbcofphxbco]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHistPad_FelixBcoFphxBco(
    int i, int icnvs)
{
  for (int fee = 0; fee < 14; ++fee)
  {
    std::string name = Form("intt_hist_%d_%02d_%02d", icnvs, i, fee);
    if (!m_hist_felixbcofphxbco[i][fee])
    {
      TC[icnvs]->cd();
      m_hist_felixbcofphxbco[i][fee] = new TH1D(
          name.c_str(), name.c_str(),  //
          128, 0, 128                  //
      );
      m_hist_felixbcofphxbco[i][fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
      m_hist_felixbcofphxbco[i][fee]->GetXaxis()->SetNdivisions(16);  //, true);
      m_hist_felixbcofphxbco[i][fee]->SetLineColor(GetFeeColor(fee));
      m_hist_felixbcofphxbco[i][fee]->SetFillStyle(4000);  // Transparent
    }
    m_hist_pad[k_felixbcofphxbco][i]->SetLogy();
    m_hist_pad[k_felixbcofphxbco][i]->cd();

    m_hist_felixbcofphxbco[i][fee]->Reset();
    m_hist_felixbcofphxbco[i][fee]->Draw("same");
  }

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  TH1* bco_hist = cl->getHisto(Form("INTTMON_%d", i), "InttBcoHist");
  m_transparent_pad[k_felixbcofphxbco][i]->Clear();
  if (!bco_hist)
  {
    m_transparent_pad[k_felixbcofphxbco][i]->cd();
    TText dead_text;
    dead_text.SetTextColor(kBlue);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.1);
    dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.5, "Dead Server");
    return 1;
  }

  // Fill
  int max = 0;
  for (int fee = 0; fee < NFEES; ++fee)
  {
    for (int bco = 0; bco < NBCOS; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(1, fee * NBCOS + bco + 1));
      if (bincont > max)
      {
        max = bincont;
      }
      m_hist_felixbcofphxbco[i][fee]->SetBinContent(bco + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin
    }
  }

  // Noramlize ranges
  for (int fee = 0; fee < NFEES; ++fee)
  {
    m_hist_felixbcofphxbco[i][fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
  }

  return 0;
}

//====== JustFphxBco ======//

int InttMonDraw::Draw_JustFphxBco()
{
  if (!gROOT->FindObject("InttJustFphxBco"))
  {
    MakeCanvas("InttJustFphxBco");
  }

  TC[k_justfphxbco]->SetEditable(true);
  m_style->cd();
  if(DrawDispPad_Generic(k_justfphxbco, TC[k_justfphxbco]->GetTitle()) == -1)
  {
    return -1;
  }

  // Draw Legend
  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad[k_justfphxbco]->Clear();
  m_lgnd_pad[k_justfphxbco]->cd();

  double x0, y0, x[4], y[4];
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, Form("FChn %2d", fee));

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box;
    box.SetFillColor(GetFeeColor(fee));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  int iret = 1;
  for (int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_JustFphxBco(i, k_justfphxbco) && iret;
  }

  TC[k_justfphxbco]->Update();
  TC[k_justfphxbco]->Show();
  TC[k_justfphxbco]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHistPad_JustFphxBco(
    int i, int icnvs)
{
  for (int fee = 0; fee < 14; ++fee)
  {
    std::string name = Form("intt_hist_%d_%02d_%02d", icnvs, i, fee);
    if (!m_hist_justfphxbco[i][fee])
    {
      TC[icnvs]->cd();
      m_hist_justfphxbco[i][fee] = new TH1D(
          name.c_str(), name.c_str(),  //
          128, 0, 128                  //
      );
      m_hist_justfphxbco[i][fee]->SetTitle(Form("intt%01d;FPHX BCO;Counts (Hits)", i));
      m_hist_justfphxbco[i][fee]->GetXaxis()->SetNdivisions(16);  //, true);
      m_hist_justfphxbco[i][fee]->SetLineColor(GetFeeColor(fee));
      m_hist_justfphxbco[i][fee]->SetFillStyle(4000);  // Transparent
    }
    m_hist_pad[k_justfphxbco][i]->SetLogy();
    m_hist_pad[k_justfphxbco][i]->cd();

    m_hist_justfphxbco[i][fee]->Reset();
    m_hist_justfphxbco[i][fee]->Draw("same");
  }

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  TH1* bco_hist = cl->getHisto(Form("INTTMON_%d", i), "InttBcoHist");
  m_transparent_pad[k_justfphxbco][i]->Clear();
  if (!bco_hist)
  {
    m_transparent_pad[k_justfphxbco][i]->cd();
    TText dead_text;
    dead_text.SetTextColor(kBlue);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.1);
    dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.5, "Dead Server");
    return 1;
  }

  // Fill
  int max = 0;
  for (int fee = 0; fee < NFEES; ++fee)
  {
    for (int bco = 0; bco < NBCOS; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(2, fee * NBCOS + bco + 1));
      if (bincont > max)
      {
        max = bincont;
      }
      m_hist_justfphxbco[i][fee]->SetBinContent(bco + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin
    }
  }

  // Noramlize ranges
  for (int fee = 0; fee < NFEES; ++fee)
  {
    m_hist_justfphxbco[i][fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
  }

  return 0;
}

//====== ZoomedFphxBco ======//

int InttMonDraw::Draw_ZoomedFphxBco()
{
  if (!gROOT->FindObject("InttZoomedFphxBco"))
  {
    MakeCanvas("InttZoomedFphxBco");
  }

  TC[k_zoomedfphxbco]->SetEditable(true);
  m_style->cd();
  if(DrawDispPad_Generic(k_zoomedfphxbco, TC[k_zoomedfphxbco]->GetTitle()) == -1)
  {
    return -1;
  }

  // Draw Legend
  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad[k_zoomedfphxbco]->Clear();
  m_lgnd_pad[k_zoomedfphxbco]->cd();

  double x0, y0, x[4], y[4];
  for (int fee = 0; fee < 14; ++fee)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * fee + 1.0) / (2.0 * 14);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, Form("FChn %2d", fee));

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box;
    box.SetFillColor(GetFeeColor(fee));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  int iret = 1;
  for (int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_ZoomedFphxBco(i, k_zoomedfphxbco) && iret;
  }

  TC[k_zoomedfphxbco]->Update();
  TC[k_zoomedfphxbco]->Show();
  TC[k_zoomedfphxbco]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHistPad_ZoomedFphxBco(
    int i, int icnvs)
{
  int num_fphx_bins = 20;
  for (int fee = 0; fee < NFEES; ++fee)
  {
    std::string name = Form("intt_left_hist_%d_%02d_%02d", icnvs, i, fee);
    if (!m_left_hist_zoomedfphxbco[i][fee])
    {
      TC[icnvs]->cd();
      m_left_hist_zoomedfphxbco[i][fee] = new TH1D(
          name.c_str(), name.c_str(),  //
          num_fphx_bins, 0, num_fphx_bins
      );
      m_left_hist_zoomedfphxbco[i][fee]->SetTitle(Form("intt%01d;FPHX BCO;Counts (Hits)", i));
      m_left_hist_zoomedfphxbco[i][fee]->SetTitleSize(0.04);
      m_left_hist_zoomedfphxbco[i][fee]->GetXaxis()->SetNdivisions(10);
      // m_left_hist_zoomedfphxbco[i][fee]->GetXaxis()->SetLabelSize(0);
      // m_left_hist_zoomedfphxbco[i][fee]->GetXaxis()->SetLabelOffset(999);
      m_left_hist_zoomedfphxbco[i][fee]->GetYaxis()->SetLabelSize(0.04);
      m_left_hist_zoomedfphxbco[i][fee]->SetLineColor(GetFeeColor(fee));
      m_left_hist_zoomedfphxbco[i][fee]->SetFillStyle(4000);  // Transparent
    }
    m_left_hist_pad[k_zoomedfphxbco][i]->SetLogy();
    m_left_hist_pad[k_zoomedfphxbco][i]->cd();
    m_left_hist_zoomedfphxbco[i][fee]->Reset();
    m_left_hist_zoomedfphxbco[i][fee]->Draw("same");
   
    name = Form("intt_right_hist_%d_%02d_%02d", icnvs, i, fee);
    if (!m_right_hist_zoomedfphxbco[i][fee])
    {
      TC[icnvs]->cd();
      m_right_hist_zoomedfphxbco[i][fee] = new TH1D(
          name.c_str(), name.c_str(),  //
          num_fphx_bins, 128 - num_fphx_bins, 128
      );
      m_right_hist_zoomedfphxbco[i][fee]->SetTitle(Form("intt%01d;FPHX BCO;Counts (Hits)", i));
      m_right_hist_zoomedfphxbco[i][fee]->SetTitleSize(0.0);
      m_right_hist_zoomedfphxbco[i][fee]->SetTitleOffset(999);
      m_right_hist_zoomedfphxbco[i][fee]->GetXaxis()->SetNdivisions(10);
      // m_right_hist_zoomedfphxbco[i][fee]->GetXaxis()->SetLabelSize(0);
      // m_right_hist_zoomedfphxbco[i][fee]->GetXaxis()->SetLabelOffset(999);
      m_right_hist_zoomedfphxbco[i][fee]->GetYaxis()->SetLabelSize(0);
      m_right_hist_zoomedfphxbco[i][fee]->GetYaxis()->SetLabelOffset(999);
      m_right_hist_zoomedfphxbco[i][fee]->SetLineColor(GetFeeColor(fee));
      m_right_hist_zoomedfphxbco[i][fee]->SetFillStyle(4000);  // Transparent
    } 
    m_right_hist_pad[k_zoomedfphxbco][i]->SetLogy();
    m_right_hist_pad[k_zoomedfphxbco][i]->cd();
    m_right_hist_zoomedfphxbco[i][fee]->Reset();
    m_right_hist_zoomedfphxbco[i][fee]->Draw("same");
  }

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  TH1* bco_hist = cl->getHisto(Form("INTTMON_%d", i), "InttBcoHist");
  m_transparent_pad[k_zoomedfphxbco][i]->Clear();
  if (!bco_hist)
  {
    m_transparent_pad[k_zoomedfphxbco][i]->cd();
    TText dead_text;
    dead_text.SetTextColor(kBlue);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.1);
    dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.5, "Dead Server");
    return 1;
  }

  // Fill
  int max = 0;
  for (int fee = 0; fee < NFEES; ++fee)
  {
    for (int bco = 0; bco < num_fphx_bins; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(2, fee * NBCOS + bco + 1));
      if (bincont > max)
      {
        max = bincont;
      }
      m_left_hist_zoomedfphxbco[i][fee]->SetBinContent(bco + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin
    }

    for (int bco = 128 - num_fphx_bins; bco < 128; ++bco)
    {
      int bincont = bco_hist->GetBinContent(bco_hist->GetBin(2, fee * NBCOS + bco + 1));
      if (bincont > max)
      {
        max = bincont;
      }
      m_right_hist_zoomedfphxbco[i][fee]->SetBinContent(bco - (128 - num_fphx_bins) + 1, bincont);  // + 1 is b/c the 0th bin is an underflow bin
    }
  }

  max *= 10;
  if(max == 0)
  {
    max = 10;
  }

  // Noramlize ranges
  for (int fee = 0; fee < NFEES; ++fee)
  {
    m_left_hist_zoomedfphxbco[i][fee]->GetYaxis()->SetRangeUser(1, max);
    m_right_hist_zoomedfphxbco[i][fee]->GetYaxis()->SetRangeUser(1, max);

    // TLines
    TLine line;
    line.SetLineWidth(2);
    line.SetLineColor(kBlack);
    line.SetLineStyle(2);
  
    m_left_hist_pad[k_zoomedfphxbco][i]->cd();
    line.DrawLine(5, 0, 5, max);
  
    m_right_hist_pad[k_zoomedfphxbco][i]->cd();
    line.DrawLine(116, 0, 116, max);
    line.DrawLine(120, 0, 120, max);
  }

  return 0;
}

Color_t
InttMonDraw::GetFeeColor(
    int const& fee)
{
  switch (fee % 7)
  {
  case 0:
    return (fee / 7) ? kOrange : kBlack;
  case 1:
    return kRed + 3 * (fee / 7);
  case 2:
    return kViolet + 3 + 7 * (fee / 7);
  case 3:
    return kGreen + 3 * (fee / 7);
  case 4:
    return kCyan + 3 * (fee / 7);
  case 5:
    return kBlue + 3 * (fee / 7);
  case 6:
    return kMagenta + 3 * (fee / 7);
  }
  return kBlack;
}

//====== HitMap ======//

int InttMonDraw::Draw_HitMap()
{
  // Set member variables we use to what they should be at beginning of each call
  if (!gROOT->FindObject("InttHitMap"))
  {
    MakeCanvas("InttHitMap");
  }

  TC[k_hitmap]->SetEditable(true);
  m_style->cd();
  if(DrawDispPad_Generic(k_hitmap, TC[k_hitmap]->GetTitle()) == -1)
  {
    return -1;
  }

  // Legend Pad
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.03;
  double lgnd_text_size = 0.12;

  m_lgnd_pad[k_hitmap]->Clear();
  m_lgnd_pad[k_hitmap]->cd();

  int color;
  std::string label;
  double x0, y0, x[4], y[4];
  for (int c = 0; c < 3; ++c)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * c + 1.0) / (2.0 * 3);

    switch (c)
    {
    case 0:
      label = "Cold";
      color = kBlue;
      break;
    case 1:
      label = "Good";
      color = kGreen;
      break;
    case 2:
      label = "Hot";
      color = kRed;
      break;
    default:
      label = "Unknown";
      color = kOrange;
      break;
    }

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(
        x0 + 1.5 * lgnd_box_width, y0,  //
        label.c_str()                   //
    );

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int i = 0; i < 4; ++i)
    {
      x[i] *= 0.5 * lgnd_box_width;
      x[i] += x0;

      y[i] *= 0.5 * lgnd_box_height;
      y[i] += y0;
    }

    TPolyLine box;
    box.SetFillColor(color);
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  // Draw histograms
  int iret = 0;
  for (int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_HitMap(i, k_hitmap) && iret;
  }

  TC[k_hitmap]->Update();
  TC[k_hitmap]->Show();
  TC[k_hitmap]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHistPad_HitMap(int i, int icnvs)
{
  double lower = 0.015;
  double upper = 0.650;

  std::string name = Form("intt_hist_%d_%01d", icnvs, i);
  if (!m_hist_hitmap[i])
  {
    TC[icnvs]->cd();
    m_hist_hitmap[i] = new TH2D(
        name.c_str(), name.c_str(),
        26, -0.5, 25.5, //
        14, -0.5, 13.5  //
    );
    m_hist_hitmap[i]->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", i));

    m_hist_hitmap[i]->GetXaxis()->SetNdivisions(13, true);
    m_hist_hitmap[i]->GetYaxis()->SetNdivisions(14, true);
    m_hist_hitmap[i]->GetZaxis()->SetRangeUser(0, 3);
    m_hist_hitmap[i]->SetFillStyle(4000);  // Transparent

    Double_t levels[4] = {0, 1, 2, 3};
    m_hist_hitmap[i]->SetContour(4, levels);
  }
  m_hist_pad[k_hitmap][i]->SetGrid(1);
  m_hist_pad[k_hitmap][i]->cd();

  m_hist_hitmap[i]->Reset();
  m_hist_hitmap[i]->Draw("COL");  // "COLZ" for a legend, "COL" for no legend; no legend is preferrable here

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  TH1* evt_hist = cl->getHisto(Form("INTTMON_%d", i), "InttEvtHist");
  TH1* hit_hist = cl->getHisto(Form("INTTMON_%d", i), "InttHitHist");
  m_transparent_pad[k_hitmap][i]->Clear();
  if (!evt_hist || !hit_hist)
  {
    m_transparent_pad[k_hitmap][i]->cd();
    TText dead_text;
    dead_text.SetTextColor(kBlue);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.1);
    dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.5, "Dead Server");
    return 1;
  }

  // Fill
  for (int fee = 0; fee < NFEES; ++fee)
  {
    for (int chp = 0; chp < NCHIPS; ++chp)
    {
      double bincont = hit_hist->GetBinContent(fee * NCHIPS + chp + 1);
      if(!bincont)
      {
        continue;
        std::cout << "filled 0" << std::endl;
      }
      bincont /= evt_hist->GetBinContent(2); // Normalize by number of unique BCOs

      // Assign a value to this bin
      // that will give it the appropriate color
      // based on how it compares to the hot/cold thresholds
      if (bincont < lower)
      {
        bincont = 0.4;  // 0.4 Cold/Dead
      }
      else if (upper < bincont)
      {
        bincont = 3.0;  // 3.0 Hot
      }
      else
      {
        bincont = 1.7;  // 1.7 Good
      }

      m_hist_hitmap[i]->SetBinContent(chp + 1, fee + 1, bincont);  // +1 to start at first bin
    }
  }

  return 0;
}

//====== HitRates ======//

int InttMonDraw::Draw_HitRates()
{
  if (!gROOT->FindObject("InttHitRates"))
  {
    MakeCanvas("InttHitRates");
  }

  TC[k_hitrates]->SetEditable(true);
  m_style->cd();
  if(DrawDispPad_Generic(k_hitrates, TC[k_hitrates]->GetTitle()) == -1)
  {
    return -1;
  }

  int iret = 1;
  for (int i = 0; i < 8; ++i)
  {
    // If any subdraw succeeds, say the entire call succeeds
    iret = DrawHistPad_HitRates(i, k_hitrates) && iret;
  }

  TC[k_hitrates]->Update();
  TC[k_hitrates]->Show();
  TC[k_hitrates]->SetEditable(false);

  return iret;
}

int InttMonDraw::DrawHistPad_HitRates(
    int i, int icnvs)
{
  double lower = 0.00;
  double upper = 0.65;

  // Validate member histos
  std::string name = Form("intt_hitrate_hist_%d_%01d", icnvs, i);
  if (!m_hist_hitrates[i])
  {
    m_hist_hitrates[i] = new TH1D(
        name.c_str(), name.c_str(), //
        112, lower, upper           //
    );
    m_hist_hitrates[i]->SetTitle(Form("intt%01d;Hits/Event (overflow is shown in last bin);Entries (One Hitrate per Chip)", i));
    m_hist_hitrates[i]->GetXaxis()->SetNdivisions(8, true);
    m_hist_hitrates[i]->SetFillStyle(4000); // Transparent
  }
  m_hist_pad[k_hitrates][i]->cd();

  m_hist_hitrates[i]->Reset();
  m_hist_hitrates[i]->Draw();

  // Access client
  OnlMonClient* cl = OnlMonClient::instance();

  TH1* evt_hist = cl->getHisto(Form("INTTMON_%d", i), "InttEvtHist");
  TH1* hit_hist = cl->getHisto(Form("INTTMON_%d", i), "InttHitHist");
  m_transparent_pad[k_hitrates][i]->Clear();
  if (!evt_hist || !hit_hist)
  {
    m_transparent_pad[k_hitrates][i]->cd();
    TText dead_text;
    dead_text.SetTextColor(kBlue);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.1);
    dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.5, "Dead Server");
    return 1;
  }

  // Fill
  // double mean = 0.0, fraction = 0.0;
  for (int fee = 0; fee < NFEES; ++fee)
  {
    for (int chp = 0; chp < NCHIPS; ++chp)
    {
      double bincont = hit_hist->GetBinContent(fee * NCHIPS + chp + 1);
      bincont /= evt_hist->GetBinContent(2); // Normalize by number of events

      // mean += bincont;
      // if(bincont < upper)++fraction;

      // Manually catch overflows and put them in the last displayed bin
      if (upper <= bincont)
      {
        bincont = upper - m_hist_hitrates[i]->GetXaxis()->GetBinWidth(1);
      }

      m_hist_hitrates[i]->Fill(bincont);
    }
  }
  // mean /= (NFEES * NCHIPS);
  // fraction /= (NFEES * NCHIPS);
  // std::cout << "mean: " << mean << " fraction less than " << upper << ": " << fraction << std::endl;

  return 0;
}

//====== History ======//

int InttMonDraw::Draw_History()
{
  if (!gROOT->FindObject("InttHistory"))
  {
    MakeCanvas("InttHistory");
  }

  TC[k_history]->SetEditable(true);
  m_style->cd();
  if(DrawDispPad_Generic(k_history, TC[k_history]->GetTitle()) == -1)
  {
    return -1;
  }

  // hist
  double max = 0.0;
  int num_dead = 0;
  m_single_transparent_pad[k_history]->Clear();
  for(int i = 0; i < 8; ++i)
  {
    // Access client
    OnlMonClient* cl = OnlMonClient::instance();

    // check to see if the most recent intervals are identically dead
    TH1* evt_hist = cl->getHisto(Form("INTTMON_%d", i), "InttEvtHist");
    if(!evt_hist)
    {
      m_single_transparent_pad[k_history]->cd();
      TText dead_text;
      dead_text.SetTextColor(kBlue);
      dead_text.SetTextAlign(22);
      dead_text.SetTextSize(0.1);
      dead_text.SetTextAngle(45);
      // dead_text.DrawText(0.5, 0.5, "Dead Server");
      ++num_dead;
      continue;
    }

    // Server has been running for 3 minutes without recieving decodable BCOs
    if(180 < evt_hist->GetBinContent(4))
    {
      ++num_dead;
    }

    TH1* log_hist = cl->getHisto(Form("INTTMON_%d", i), "InttLogHist");
    if(!log_hist)
    {
      m_single_transparent_pad[k_history]->cd();
      TText dead_text;
      dead_text.SetTextColor(kBlue);
      dead_text.SetTextAlign(22);
      dead_text.SetTextSize(0.1);
      dead_text.SetTextAngle(45);
      // dead_text.DrawText(0.5, 0.5, "Dead Server");
      ++num_dead;
      continue;
    }

    // Validate member histos
    int N = log_hist->GetNbinsX();
    double w = log_hist->GetXaxis()->GetBinWidth(0);
    std::string name = Form("intt_history_hist_%d", i);
    if (!m_hist_history[i])
    {
      m_hist_history[i] = new TH1D(
          name.c_str(), name.c_str(), //
          N, 0.0, w * N //
      );
      m_hist_history[i]->SetTitle(Form("Rate of BCO decoding;Most recent %.0lf seconds;Decoded BCOs / s", (double)(w * N)));
      m_hist_history[i]->SetFillStyle(4000); // Transparent
      m_hist_history[i]->SetLineColor(GetFeeColor(i)); // Transparent
    }
    m_single_hist_pad[k_history]->cd();

    m_hist_history[i]->Reset();
    m_hist_history[i]->Draw("Same");

    // Fill
    int buff_index = log_hist->GetBinContent(N);
    if(log_hist->GetBinContent(N + 1))
    {
      // The contents should be displayed as being wrapped
      // start from right edge and go backward in time
      for(int n = N; 0 < n; --n)
      {
        double rate = log_hist->GetBinContent(buff_index) / w;
        m_hist_history[i]->SetBinContent(n, rate);
        if(max < rate)
        {
          max = rate;
        }
        buff_index = (buff_index + N - 1) % N;
      }
    }
    else
    {
      // The contents should not be displayed as being wrapped
      // start from left edge and go forward in time
      for(int n = 0; n < buff_index; ++n)
      {
        double rate = log_hist->GetBinContent(n + 1) / w;
        m_hist_history[i]->SetBinContent(n + 1, rate);
        if(max < rate)
        {
          max = rate;
        }
      }
    }
  }

  if(2 < num_dead)
  {
    m_single_transparent_pad[k_history]->cd();
    TText dead_text;
    dead_text.SetTextColor(kRed);
    dead_text.SetTextAlign(22);
    dead_text.SetTextSize(0.1);
    // dead_text.SetTextAngle(45);
    dead_text.DrawText(0.5, 0.65, "Dead Felix Servers");
    dead_text.DrawText(0.5, 0.35, "Restart Run");
  }

  for(int i = 0; i < 8; ++i)
  {
    if(!m_hist_history[i])
    {
      continue;
    }
    m_hist_history[i]->GetYaxis()->SetRangeUser(0, 1.5 * max);
  }

  // Draw Legend
  double lgnd_text_size = 0.08;
  double lgnd_box_width = 0.16;
  double lgnd_box_height = 0.01;

  std::string name;

  m_lgnd_pad[k_history]->Clear();
  m_lgnd_pad[k_history]->cd();

  double x0, y0, x[4], y[4];
  for (int i = 0; i < 8; ++i)
  {
    x0 = 0.5 - lgnd_box_width;
    y0 = (2.0 * i + 1.0) / (2.0 * 8);

    TText lgnd_text;
    lgnd_text.SetTextAlign(12);
    lgnd_text.SetTextSize(lgnd_text_size);
    lgnd_text.SetTextColor(kBlack);
    lgnd_text.DrawText(x0 + 1.5 * lgnd_box_width, y0, Form("intt%01d", i));

    x[0] = -1, x[1] = +1, x[2] = +1, x[3] = -1;
    y[0] = -1, y[1] = -1, y[2] = +1, y[3] = +1;
    for (int j = 0; j < 4; ++j)
    {
      x[j] *= 0.5 * lgnd_box_width;
      x[j] += x0;

      y[j] *= 0.5 * lgnd_box_height;
      y[j] += y0;
    }

    TPolyLine box;
    box.SetFillColor(GetFeeColor(i));
    box.SetLineColor(kBlack);
    box.SetLineWidth(1);
    box.DrawPolyLine(4, x, y, "f");
  }

  TC[k_history]->Update();
  TC[k_history]->Show();
  TC[k_history]->SetEditable(false);

  return 0;
}


