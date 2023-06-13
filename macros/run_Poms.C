// Phenix Online Monitoring System
//
// Macro loader for starting Online Monitoring GUI Framework
//
// Author:  Cody McCain (mcm99c@acu.edu)
// Date:    January 2, 2003
//
// See run_Poms.C.README

#include <onlmon/Poms.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libpoms.so)

void StartPoms()
{
  PomsMainFrame *pmf;
  pmf = PomsMainFrame::Instance();

  // Register SubSystems
//  pmf->RegisterSubSystem("EXAMPLE", "example");

  SubSystem *bbc = new SubSystem("BBC", "bbc");
  bbc->AddAction("bbcDraw(\"FIRST\")", "BBC 1st Monitor");
  bbc->AddAction("bbcDraw(\"SECOND\")", "BBC 2nd Monitor");
  bbc->AddAction("bbcDraw(\"HISTORY\")", "BBC History Monitor");
  bbc->AddAction(new SubSystemActionDrawPS(bbc));
  pmf->RegisterSubSystem(bbc);

  // SubSystem *bbcll1 = new SubSystem("BBCLL1", "bbcll1");
  // bbcll1->AddAction("bbcll1Draw(\"FIRST\")", "BBCLL1 1st Monitor");
  // bbcll1->AddAction("bbcll1Draw(\"SECOND\")", "BBCLL1 2nd Monitor");
  // bbcll1->AddAction("bbcll1Draw(\"HISTORY\")", "BBCLL1 History Monitor");
  // bbcll1->AddAction(new SubSystemActionDrawPS(bbcll1));
  // pmf->RegisterSubSystem(bbcll1);

  SubSystem *cemc = new SubSystem("CEMC", "cemc");
  cemc->AddAction("cemcDraw(\"FIRST\")", "CEMC 1st Monitor");
  cemc->AddAction("cemcDraw(\"SECOND\")", "CEMC 2nd Monitor");
  cemc->AddAction("cemcDraw(\"HISTORY\")", "CEMC History Monitor");
  cemc->AddAction(new SubSystemActionDrawPS(cemc));
  pmf->RegisterSubSystem(cemc);

  // SubSystem *daq = new SubSystem("DAQ", "daq");
  // daq->AddAction("daqDraw(\"FIRST\")", "DAQ 1st Monitor");
  // daq->AddAction("daqDraw(\"SECOND\")", "DAQ 2nd Monitor");
  // daq->AddAction("daqDraw(\"HISTORY\")", "DAQ History Monitor");
  // daq->AddAction(new SubSystemActionDrawPS(daq));
  // pmf->RegisterSubSystem(daq);

  // SubSystem *epd = new SubSystem("EPD", "epd");
  // epd->AddAction("epdDraw(\"FIRST\")", "EPD 1st Monitor");
  // epd->AddAction("epdDraw(\"SECOND\")", "EPD 2nd Monitor");
  // epd->AddAction("epdDraw(\"HISTORY\")", "EPD History Monitor");
  // epd->AddAction(new SubSystemActionDrawPS(epd));
  // pmf->RegisterSubSystem(epd);

  SubSystem *hcal = new SubSystem("HCAL", "hcal");
  hcal->AddAction("hcalDraw(\"FIRST\")", "HCAL 1st Monitor");
  hcal->AddAction("hcalDraw(\"SECOND\")", "HCAL 2nd Monitor");
  hcal->AddAction("hcalDraw(\"HISTORY\")", "HCAL History Monitor");
  hcal->AddAction(new SubSystemActionDrawPS(hcal));
  pmf->RegisterSubSystem(hcal);

  SubSystem *intt = new SubSystem("INTT", "intt");
  intt->AddAction("inttDraw(\"chip_hitmap\")", "INTT Chip Hitmap");
  intt->AddAction("inttDraw(\"ladder_hitmap\")", "INTT ladder Hitmap");
  intt->AddAction(new SubSystemActionDrawPS(intt));
  pmf->RegisterSubSystem(intt);

  SubSystem *mvtx = new SubSystem("MVTX", "mvtx");
  mvtx->AddAction("mvtxDraw(\"MvtxMon_General\")", "General Monitor");
  mvtx->AddAction("mvtxDraw(\"MvtxMon_HitMap\")", "Hitmap");
  mvtx->AddAction("mvtxDraw(\"MvtxMon_FEE\")", "FEE");
  mvtx->AddAction("mvtxDraw(\"MvtxMon_FHR\")", "FHR");
  mvtx->AddAction("mvtxDraw(\"MvtxMon_OCC\")", "OCC");
  mvtx->AddAction(new SubSystemActionDrawPS(mvtx));
  pmf->RegisterSubSystem(mvtx);

  SubSystem *tpc = new SubSystem("TPC", "tpc");
  tpc->AddAction("tpcDraw(\"TPCMODULE\")", "TPC SUM[ADC]");
  tpc->AddAction("tpcDraw(\"TPCSAMPLESIZE\")", "TPC Sample Size");
  tpc->AddAction("tpcDraw(\"TPCCHECKSUMERROR\")", "TPC Checksum Error Prob.");
  tpc->AddAction("tpcDraw(\"TPCADCVSSAMPLE\")", "TPC ADC vs. Sample");
  tpc->AddAction("tpcDraw(\"TPCMAXADCMODULE\")", "TPC MAX ADC");
  tpc->AddAction("tpcDraw(\"SECOND\")", "TPC 2nd Monitor");
  tpc->AddAction("tpcDraw(\"HISTORY\")", "TPC History Monitor");
  tpc->AddAction(new SubSystemActionDrawPS(tpc));
  pmf->RegisterSubSystem(tpc);

  SubSystem *tpot = new SubSystem("TPOT", "tpot");
  tpot->AddAction("tpotDraw(\"FIRST\")", "TPOT 1st Monitor");
  tpot->AddAction("tpotDraw(\"SECOND\")", "TPOT 2nd Monitor");
  tpot->AddAction("tpotDraw(\"HISTORY\")", "TPOT History Monitor");
  tpot->AddAction(new SubSystemActionDrawPS(tpot));
  pmf->RegisterSubSystem(tpot);

  pmf->Draw();
}
