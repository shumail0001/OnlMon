#ifndef INTT_MON_H
#define INTT_MON_H

#include <onlmon/OnlMon.h>

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

  Packet** plist{nullptr};
  TH1* EvtHist{nullptr};
  TH1* HitHist{nullptr};
  TH1* BcoHist{nullptr};

  struct bco_comparator_s
  {
	  unsigned long long static const MAX = (unsigned long long){1} << 40;
	  bool operator()(unsigned long long const&, unsigned long long const&) const;
  } const m_bco_less{};
  std::set<unsigned long long, bco_comparator_s> m_unique_bcos;
  unsigned long long m_most_recent_bco = {};
  unsigned long long m_last_flushed_bco = {};

  int m_max_size = 1000;
  int m_unique_bco_count = {};
};

#endif
