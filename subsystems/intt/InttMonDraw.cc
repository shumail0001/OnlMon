#include "InttMonDraw.h"

#include <TPolyLine.h>

InttMonDraw::Options_t InttMonDraw::OPTIONS =
{
  //Chip-Channel
  {"chip_hitmap", (struct Option_s){
    .head = &InttMonDraw::GlobalChipLocalChannelHead,
    .global = &InttMonDraw::PrepGlobalChipHists_Hitmap,
    .local = &InttMonDraw::PrepLocalChannelHists_Hitmap}},

//  {"chip_nll", (struct Option_s){
//    .head = &InttMonDraw::GlobalChipLocalChannelHead,
//    .global = &InttMonDraw::PrepGlobalChipHists_NLL,
//    .local = &InttMonDraw::PrepLocalChannelHists_Hitmap}},
//
//  //Ladder-Chip
//  {"ladder_hitmap", (struct Option_s){
//    .head = &InttMonDraw::GlobalLadderLocalChipHead,
//    .global = &InttMonDraw::PrepGlobalLadderHists_Interface,
//    .local = &InttMonDraw::PrepLocalChipHists_Hitmap}},
//
//  //hits vs Evt (by packet)
//  {"hits_vs_evt", (struct Option_s){ // Does not follow same recursive structure; everything is done in the head function
//    .head = &InttMonDraw::DrawHitsVsEvt,
//    .global = nullptr,
//    .local = nullptr}},

  //Bco Diffs
  {"bco_diff", (struct Option_s){ // Does not follow same recursive structure; everything is done in the head function
    .head = &InttMonDraw::DrawFelixBcoFphxBco,
    .global = nullptr,
    .local = nullptr}},
};

InttMonDraw::InttMonDraw(const std::string& name)
  : OnlMonDraw(name)
{
  return;
}

InttMonDraw::~InttMonDraw()
{
  return;
}

//===    Inherited Functions    ===//
int InttMonDraw::Init()
{
  return 0;
}

int InttMonDraw::Draw(const std::string& what)
{
  bool found = false;

  if (what == "ALL" || what == "SERVERSTATS")
  {
    DrawServerStats();
    found = true;
  }

  bool b = false;
  std::string temp = "";
  for (char s : what)
  {
    temp += (char) std::tolower(s);
  }

  for (auto& itr : OPTIONS)
  {
    b = false;

    if (temp == "all")
    {
      b = true;
    }
    if (temp == itr.first)
    {
      b = true;
    }

    if (!b)
    {
      continue;
    }

    found = true;
    (*(itr.second.head))(itr.first);
  }

  if (!found)
  {
    std::cout << "Option \"" << what << "\" not found" << std::endl;
    std::cout << "Try \"all\" or one of the following (case insensitive)" << std::endl;
    for (auto& itr : OPTIONS)
    {
      std::cout << "\t" << itr.first << std::endl;

      return 1;
    }
  }

  return 0;
}

int InttMonDraw::MakeHtml(const std::string& what)
{
  OnlMonClient* cl = OnlMonClient::instance();
  if (Draw(what))
  {
    return 1;
  }
  TSeqCollection* canvases = gROOT->GetListOfCanvases();
  TCanvas* canv = nullptr;

  bool b = false;
  bool found = false;

  std::string temp = "";
  std::string name = "";
  for (char s : what)
  {
    temp += (char) std::tolower(s);
  }
  for (auto& itr : OPTIONS)
  {
    b = false;

    if (temp == "all")
    {
      b = true;
    }
    if (temp == itr.first)
    {
      b = true;
    }

    if (!b)
    {
      continue;
    }

    found = true;

    canv = nullptr;
    name = Form("Intt_%s", (itr.first).c_str());
    for (TIter t_itr = canvases->begin(); t_itr != canvases->end(); ++t_itr)
    {
      if (std::string((*t_itr)->GetName()).find(name) == std::string::npos)
      {
        continue;
      }

      canv = (TCanvas*) (*t_itr);
      break;
    }

    if (!canv)
    {
      continue;
    }

    std::string pngfl = cl->htmlRegisterPage(*this, canv->GetTitle(), itr.first, "png");
    cl->CanvasToPng(canv, pngfl);
  }

  if (!found)
  {
    std::cout << "Option \"" << what << "\" not found" << std::endl;
    std::cout << "Try \"all\" or one of the following (case insensitive)" << std::endl;
    for (auto& itr : OPTIONS)
    {
      std::cout << "\t" << itr.first << std::endl;
    }
  }

// this code must not be modified
  Draw("SERVERSTATS");

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

  return 0;
}

int InttMonDraw::SavePlot(std::string const& what, std::string const& type)
{
  OnlMonClient* cl = OnlMonClient::instance();
  if (Draw(what))
  {
    return 1;
  }

  TSeqCollection* canvases = gROOT->GetListOfCanvases();
  TCanvas* canvas = nullptr;
  std::string filename;

  bool b = false;
  bool found = false;

  std::string temp = "";
  std::string name = "";
  for (char s : what)
  {
    temp += (char) std::tolower(s);
  }
  for (auto& itr : OPTIONS)
  {
    b = false;

    if (temp == "all")
    {
      b = true;
    }
    if (temp == itr.first)
    {
      b = true;
    }

    if (!b)
    {
      continue;
    }

    found = true;

    canvas = nullptr;
    name = Form("Intt_%s", (itr.first).c_str());
    for (TIter t_itr = canvases->begin(); t_itr != canvases->end(); ++t_itr)
    {
      if (std::string((*t_itr)->GetName()).find(name) == std::string::npos)
      {
        continue;
      }

      canvas = (TCanvas*) (*t_itr);
      break;
    }

    if (!canvas)
    {
      continue;
    }

    filename = ThisName + "_" + itr.first + "_" + cl->RunNumber() + "." + type;
    cl->CanvasToPng(canvas, filename);
  }

  if (!found)
  {
    std::cout << "Option \"" << what << "\" not found" << std::endl;
    std::cout << "Try \"all\" or one of the following (case insensitive)" << std::endl;
    for (auto& itr : OPTIONS)
    {
      std::cout << "\t" << itr.first << std::endl;
    }
  }

  return 0;
}

int InttMonDraw::MakeCanvas(const std::string& name)
{
  OnlMonClient* cl = OnlMonClient::instance();
  int xsize = cl->GetDisplaySizeX();
  int ysize = cl->GetDisplaySizeY();
  if (name == "InttMonServerStats")
  {
    TC[0] = new TCanvas(name.c_str(), "InttMon Server Stats", xsize / 2, 0, xsize / 2, ysize);
    gSystem->ProcessEvents();
    transparent[0] = new TPad("transparent1", "this does not show", 0, 0, 1, 1);
    transparent[0]->SetFillColor(kGray);
    transparent[0]->Draw();
    TC[0]->SetEditable(false);
    TC[0]->SetTopMargin(0.05);
    TC[0]->SetBottomMargin(0.05);
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
        PrintRun.SetTextColor(kGray+2);
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
