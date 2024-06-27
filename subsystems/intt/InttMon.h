#ifndef INTT_MON_H
#define INTT_MON_H

#include <onlmon/OnlMon.h>

#include <chrono>
#include <set>
#include <string>

class Packet;
class TH1;

class InttMon : public OnlMon
{
 public:
  InttMon(const std::string& name);
  virtual ~InttMon();

  int Init();
  int BeginRun(const int);
  int process_event(Event*);
  int Reset();

  int MiscDebug();

 private:
  static constexpr int NCHIPS = 26;
  static constexpr int NFEES = 14;
  static constexpr int NBCOS = 128;

  int static const m_MAX_BCO_DIFF = 1000;

  int static const m_LOG_DURATION = 360; // seconds
  int static const m_LOG_INTERVAL = 2; // seconds

  Packet** plist{nullptr};
  TH1* EvtHist{nullptr};
  TH1* HitHist{nullptr};
  TH1* BcoHist{nullptr};
  TH1* LogHist{nullptr};

  struct bco_comparator_s
  {
	  unsigned long long static const MAX = (unsigned long long){1} << 40;
	  bool operator()(unsigned long long const&, unsigned long long const&) const;
  } const m_bco_less{};
  std::set<unsigned long long, bco_comparator_s> m_unique_bcos;
  unsigned long long m_most_recent_bco = 0;
  unsigned long long m_last_flushed_bco = 0; 

  int m_unique_bco_count = {};
  int m_log_bin = 0;
  int m_logged_bcos = 0;

  std::chrono::time_point<std::chrono::system_clock> m_start{};
};

#endif
