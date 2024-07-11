#include "InttMon.h"

#include <onlmon/OnlMonServer.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <TH1.h>
#include <TH2.h>

#include <iostream>
#include <limits>

InttMon::InttMon(const std::string &name)
  : OnlMon(name)
{
  //  plist = new Packet *[1];
  return;
}

InttMon::~InttMon()
{
  //  delete[] plist;
}

int InttMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();

  // histograms
  // GetBinContent(1): # of rcdaq events, GetBinContent(2): # of unique BCOs, GetBinContent(3): BCO order error, GetBinContent(4): Time since last BCO
  EvtHist = new TH1I("InttEvtHist", "InttEvtHist", 4, 0.0, 1.0);
  // 26*14
  HitHist = new TH1I("InttHitHist", "InttHitHist", (NFEES * NCHIPS), 0.0, 1.0);
  // 128*14
  BcoHist = new TH2I("InttBcoHist", "InttBcoHist", 2, 0.0, 1.0, (NFEES * NBCOS), 0.0, 1.0);
  // Decoded BCOs as function of real time, implemented as ring buffer
  LogHist = new TH1I("InttLogHist", "InttLogHist", m_LOG_DURATION / m_LOG_INTERVAL, 0.0, m_LOG_DURATION);

  se->registerHisto(this, EvtHist);
  se->registerHisto(this, HitHist);
  se->registerHisto(this, BcoHist);
  se->registerHisto(this, LogHist);
  //...

  return 0;
}

int InttMon::BeginRun(const int /* run_num */)
{
  EvtHist->Reset();
  HitHist->Reset();
  BcoHist->Reset();
  LogHist->Reset();

  m_unique_bcos.clear();
  m_unique_bco_count = 0;
  m_most_recent_bco = std::numeric_limits<unsigned long long>::max();
  m_last_flushed_bco = std::numeric_limits<unsigned long long>::max();

  m_log_bin = 0;
  m_logged_bcos = 0;

  m_prev_time = std::chrono::system_clock::now();

  return 0;
}

int InttMon::process_event(Event *evt)
{
  for (int pid = 3001; pid < 3009; ++pid)
  {
    Packet *pkt = evt->getPacket(pid);
    if (!pkt)
    {
      continue;
    }

	// hits
    for (int n = 0; n < pkt->iValue(0, "NR_HITS"); ++n)
    {
      int fee = pkt->iValue(n, "FEE");
      int chp = (pkt->iValue(n, "CHIP_ID") + 25) % 26;
	  int fphx_bco = pkt->iValue(n, "FPHX_BCO");
      int bco_diff = ((0x7f & pkt->lValue(n, "BCO")) - fphx_bco + 128) % 128;
      HitHist->AddBinContent(fee * NCHIPS + chp + 1);  // +1 to start at bin 1
      BcoHist->AddBinContent(BcoHist->GetBin(1, fee * NBCOS + bco_diff + 1));   // +1 to start at bin 1
      BcoHist->AddBinContent(BcoHist->GetBin(2, fee * NBCOS + fphx_bco + 1));   // +1 to start at bin 1
    }

	// bcos
	for (int n = 0; n < pkt->iValue(0, "NR_BCOS"); ++n)
	{
      unsigned long long bco_full = pkt->lValue(n, "BCOLIST");

  	  // more recent that the variable we track it with, or variable we track it with hasn't been set to a "real" value yet
  	  if(m_bco_less(m_most_recent_bco, bco_full) || (m_most_recent_bco == std::numeric_limits<unsigned long long>::max()))
  	  {
          m_most_recent_bco = bco_full;
  	  }

  	  // make sure it's not in the range of bcos we've "flushed" into our counter (and that the variable we track it with has been set to a "real" value)
  	  // This check should be moot--the fees have their BCOs in order--but I leave it in anyways b/c it is not expensive and confirms this assumption
  	  if(!m_bco_less(m_last_flushed_bco, bco_full) && (m_last_flushed_bco != std::numeric_limits<unsigned long long>::max()))
  	  {
          EvtHist->SetBinContent(3, 1);
		  continue;
  	  }

  	  m_unique_bcos.insert(bco_full);
      m_last_decode_time = std::chrono::system_clock::now();
	}

    delete pkt;
  }

  // Go through our list of unique BCOs and "flush" them into a counter
  std::set<unsigned long long, bco_comparator_s>::const_iterator bco_itr = m_unique_bcos.begin();
  for(bco_itr = m_unique_bcos.begin(); bco_itr != m_unique_bcos.end(); ++bco_itr)
  {
    if((m_most_recent_bco == std::numeric_limits<unsigned long long>::max()) || m_bco_less(m_most_recent_bco - m_MAX_BCO_DIFF, *bco_itr))
    {
      break;
    }

    ++m_unique_bco_count;
    m_last_flushed_bco = *bco_itr;
  }
  m_unique_bcos.erase(m_unique_bcos.begin(), bco_itr);

  EvtHist->AddBinContent(1);
  EvtHist->SetBinContent(2, m_unique_bco_count + m_unique_bcos.size());

  // I'm being pedantic with this block since I don't use chrono that much
  auto now = std::chrono::system_clock::now();
  std::chrono::duration<double> duration = now - m_last_decode_time; // Time since we actually decoded a BCO
  double seconds = duration.count();
  EvtHist->SetBinContent(4, seconds);

  duration = now - m_prev_time; // Time since we updated the decoding rate histogram--we update this every m_LOG_INTERVAL even if we don't decode BCOs
  seconds = duration.count();

  // See if we should increment m_log_bin
  while(m_LOG_INTERVAL < seconds)
  {
    m_prev_time = now;
    int N = LogHist->GetNbinsX();

	if(m_log_bin == N - 1)
	{
	  LogHist->AddBinContent(N + 1); // flag that content was wrapped
	}

	m_log_bin = (m_log_bin + 1) % N;
    m_logged_bcos = m_unique_bco_count + m_unique_bcos.size();

	LogHist->SetBinContent(m_log_bin, 0);
	LogHist->SetBinContent(N, m_log_bin);

	seconds -= m_LOG_INTERVAL;
  }

  LogHist->SetBinContent(m_log_bin, m_unique_bco_count + m_unique_bcos.size() - m_logged_bcos);

  return 0;
}

int InttMon::Reset()
{
  	return 0;
}

int InttMon::MiscDebug()
{
  for (int fee = 0; fee < 14; ++fee)
  {
    for (int chp = 0; chp < 26; ++chp)
    {
      HitHist->SetBinContent(fee * NCHIPS + chp + 1, chp);
    }
  }

  for (int fee = 0; fee < 14; ++fee)
  {
    for (int bco = 0; bco < 128; ++bco)
    {
      BcoHist->SetBinContent(fee * NBCOS + bco + 1, fee);
    }
  }

  return 0;
}

bool InttMon::bco_comparator_s::operator()(unsigned long long const& lhs, unsigned long long const& rhs) const
{
  return (rhs - lhs + 2 * MAX) % MAX < (lhs - rhs + 2 *MAX) % MAX;
}

//             Ladder Structure                //
//=============================================//
//      U14     U1   Ladder_z  Type B  North   //
//      U15     U2      .        .       .     //
//      U16     U3      3        .       .     //
//      U17     U4      .        .       .     //
//      U18     U5   Ladder_z  Type B    .     //
//------------------------------------   .     //
//      U19     U6   Ladder_z  Type A    .     //
//      U20     U7      .        .       .     //
//      U21     U8      .        .       .     //
//      U22     U9      2        .       .     //
//      U23     U10     .        .       .     //
//      U24     U11     .        .       .     //
//      U25     U12     .        .       .     //
//      U26     U13  Ladder_z  Type A  North   //
//---------------------------------------------//
//      U13     U26  Ladder_z  Type A  South   //
//      U12     U25     .        .       .     //
//      U11     U24     .        .       .     //
//      U10     U23     .        .       .     //
//      U9      U22     0        .       .     //
//      U8      U21     .        .       .     //
//      U7      U20     .        .       .     //
//      U6      U19  Ladder_z  Type A    .     //
//------------------------------------   .     //
//      U5      U18  Ladder_z  Type B    .     //
//      U4      U17     .        .       .     //
//      U3      U16     1        .       .     //
//      U2      U15     .        .       .     //
//      U1      U14  Ladder_z  Type B  South   //
//=============================================//
