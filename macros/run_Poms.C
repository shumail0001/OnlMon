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
  bbc->AddAction("bbcDraw(\"FIRST\")", "BBC Vertex Monitor");
  bbc->AddAction("bbcDraw(\"SECOND\")", "BBC Timing Monitor");
  bbc->AddAction(new SubSystemActionDrawPS(bbc));
  pmf->RegisterSubSystem(bbc);

  SubSystem *cemc = new SubSystem("CEMC", "cemc");
  cemc->AddAction("cemcDraw(\"FIRST\")", "Towers");
  cemc->AddAction("cemcDraw(\"SECOND\")", "Packet Health");
  cemc->AddAction("cemcDraw(\"THIRD\")", "Wave Forms");
  cemc->AddAction("cemcDraw(\"FOURTH\")", "Wave vs Fast Fitting");
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
   // epd->AddAction("epdDraw(\"THIRD\")", "EPD 3rd Monitor");
   // epd->AddAction(new SubSystemActionDrawPS(epd));
   // pmf->RegisterSubSystem(epd);

  SubSystem *hcal = new SubSystem("HCAL", "hcal");
  hcal->AddAction("hcalDraw(\"FIRST\")", "Towers");
  hcal->AddAction("hcalDraw(\"SECOND\")", "Sector Average");
  hcal->AddAction("hcalDraw(\"THIRD\")", "Wave Form");
  hcal->AddAction(new SubSystemActionDrawPS(hcal));
  pmf->RegisterSubSystem(hcal);

  SubSystem *intt = new SubSystem("INTT", "intt");
  intt->AddAction("inttDraw(\"chip_hitmap\")", "INTT Chip Hitmap");
  intt->AddAction("inttDraw(\"ladder_hitmap\")", "INTT ladder Hitmap");
  intt->AddAction(new SubSystemActionDrawPS(intt));
  pmf->RegisterSubSystem(intt);

  SubSystem *ll1 = new SubSystem("LL1", "ll1");
  ll1->AddAction("ll1Draw(\"FIRST\")", "Hits");
  ll1->AddAction("ll1Draw(\"SECOND\")", "Hits Correlation");
  ll1->AddAction("ll1Draw(\"THIRD\")", "Hits Lineup");
  ll1->AddAction(new SubSystemActionDrawPS(ll1));
  pmf->RegisterSubSystem(ll1);
  
  SubSystem *mvtx = new SubSystem("MVTX", "mvtx");
  mvtx->AddAction("mvtxDraw(\"GENERAL\")", "General Monitor");
  mvtx->AddAction("mvtxDraw(\"FEE\")", "FEE");
  mvtx->AddAction("mvtxDraw(\"FHR\")", "FHR");
  mvtx->AddAction("mvtxDraw(\"OCC\")", "OCC");
  mvtx->AddAction(new SubSystemActionDrawPS(mvtx));
  pmf->RegisterSubSystem(mvtx);

  SubSystem *tpc = new SubSystem("TPC", "tpc");
  tpc->AddAction("tpcDraw(\"TPCMODULE\")", "TPC SUM[ADC]");
  tpc->AddAction("tpcDraw(\"TPCSAMPLESIZE\")", "TPC Sample Size");
  tpc->AddAction("tpcDraw(\"TPCCHECKSUMERROR\")", "TPC Checksum Error Prob.");
  tpc->AddAction("tpcDraw(\"TPCADCVSSAMPLE\")", "TPC ADC vs. Sample");
  tpc->AddAction("tpcDraw(\"TPCMAXADCMODULE\")", "TPC MAX ADC");
  tpc->AddAction("tpcDraw(\"TPCRAWADC1D\")", "TPC RAW ADC 1D");
  tpc->AddAction("tpcDraw(\"TPCMAXADC1D\")", "TPC (WindowMAX-Pedestal) ADC 1D");
  
  tpc->AddAction(new SubSystemActionDrawPS(tpc));
  pmf->RegisterSubSystem(tpc);

  SubSystem *tpot = new SubSystem("TPOT", "tpot");
  tpot->AddAction("tpotDraw(\"TPOT_detector_occupancy\")", "Detector Occupancy");
  tpot->AddAction("tpotDraw(\"TPOT_resist_occupancy\")", "Resist Occupnacy");
  tpot->AddAction("tpotDraw(\"TPOT_adc_vs_sample\")", "ADC Sample");
  tpot->AddAction("tpotDraw(\"TPOT_hit_charge\")", "Hit Charge");
  tpot->AddAction("tpotDraw(\"TPOT_hit_multiplicity\")", "Hit Multiplicity");
  tpot->AddAction("tpotDraw(\"TPOT_hit_vs_channel\")", "Hit Channel");
  tpot->AddAction(new SubSystemActionDrawPS(tpot));
  pmf->RegisterSubSystem(tpot);

  pmf->Draw();
}
