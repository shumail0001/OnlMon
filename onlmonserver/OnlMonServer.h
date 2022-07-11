#ifndef __ONLMONSERVER_H
#define __ONLMONSERVER_H

#include <OnlMonBase.h>

#include <ctime>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <pthread.h>

class Event;
class MessageSystem;
class OnlMon;
class OnlMonStatusDB;
class OnlMonTrigger;
class PHCompositeNode;
class TH1;


class OnlMonServer: public OnlMonBase
{
public:
  static OnlMonServer *instance();
  virtual ~OnlMonServer();

  void registerHisto(const std::string &monitorname, const std::string &hname, TH1 *h1d, const int replace = 0);
  void registerHisto(const OnlMon *monitor, TH1 *h1d);

  void registerCommonHisto(TH1 *h1d);
  TH1 *getHisto(const std::string &hname) const;
  TH1 *getHisto(const unsigned int ihisto) const;
  const std::string getHistoName(const unsigned int ihisto) const;
  unsigned int nHistos() const {return Histo.size();}
  unsigned int Trigger(const unsigned short int i = 2) {return trigger[i];}
  void Trigger(const unsigned int i, const unsigned short int iwhat) {trigger[iwhat] = i;}
  int Trigger(const std::string &trigname, const unsigned short int i = 2);
  int RunNumber() const {return runnumber;}
  void RunNumber(const int irun);
  int EventNumber() const {return eventnumber;}
  void EventNumber(const int iev) {eventnumber = iev;}
  int PortNumber() const {return portnumber;}
  void PortNumber(const int i) {portnumber = i;}
  void Print(const std::string &what = "ALL") const;

  void InitAll();

  void registerMonitor(OnlMon *Monitor);
  OnlMon *getMonitor(const std::string &name);
  void dumpHistos(const std::string &filename);
  int process_event(Event *);
  int Reset();
  int BeginRun(const int runno);
  int EndRun(const int runno);
  int WriteHistoFile();

  time_t CurrentTicks() const {return currentticks;}
  void CurrentTicks(const time_t ival) {currentticks = ival;}
  time_t BorTicks() const {return borticks;}
  void BorTicks(const time_t ival) {borticks = ival;}

  int BadEvents() const {return badevents;}
  void AddBadEvent() {badevents++;}
  void BadEvents(const int ibad) {badevents = ibad;}

  int WriteLogFile(const std::string &name, const std::string &msg) const;

  // interface to OnlMonTrigger class methods
  OnlMonTrigger *OnlTrig();
  void TrigMask(const std::string &name, const unsigned int bitmask);
  unsigned int getLevel1Bit(const std::string &name);
  unsigned int AddToTriggerMask(const std::string &name);
  unsigned int ScaledTrigMask() const {return scaledtrigmask;}
  unsigned int  AddScaledTrigMask(const unsigned int mask);
  int IsPacketActive(const unsigned int ipkt);
  // set status if something went wrong

  int SetSubsystemStatus(OnlMon *Monitor, const int status);
  int SetSubsystemRunStatus(OnlMon *Monitor, const int status);
  int LookAtMe(OnlMon *Monitor, const int level, const std::string &message);
  std::string GetRunType() const {return RunType;}
  std::string GetTriggerConfig() const {return TriggerConfig;}

  int send_message(const OnlMon *Monitor, const int msgsource, const int severity, const std::string &err_message, const int msgtype) const; 
  int DisconnectDB();
  void GetMutex(pthread_mutex_t &lock) {lock = mutex;}
  void SetThreadId(pthread_t &id) {serverthreadid = id;}

  int LoadActivePackets();
  int parse_granuleDef(std::set<std::string> &pcffilelist);
  void parse_pcffile(const std::string &lfn);
  void clearactivepackets() {activepackets.clear();}
  int LoadLL1Packets();
  int isStandaloneRun() const {return standalone;}
  int isCosmicRun() const {return cosmicrun;}
  PHCompositeNode *TopNode() {return topNode;}

private:
  OnlMonServer(const std::string &name= "OnlMonServer");
  int send_message(const int severity, const std::string &err_message, const int msgtype) const;
  int CacheRunDB(const int runno);
  void registerHisto(const std::string &hname, TH1 *h1d, const int replace = 0);

  static OnlMonServer *__instance;
  unsigned int trigger[3];
  int runnumber;
  int eventnumber;
  int portnumber;
  int badevents;
  time_t currentticks;
  time_t borticks;
  int activepacketsinit;
  unsigned int scaledtrigmask;
  int scaledtrigmask_used;
  int standalone;
  int cosmicrun;
  std::string TriggerConfig;
  std::string RunType;

  TH1 *serverrunning;
  OnlMonTrigger *onltrig;
  OnlMonStatusDB *statusDB;
  OnlMonStatusDB *RunStatusDB;
  std::map<const std::string, TH1*> Histo;
  std::vector<OnlMon*> MonitorList;
  std::set<unsigned int> activepackets;
  std::map<std::string, MessageSystem *> MsgSystem;
  std::map<std::string, std::set<std::string> > MonitorHistoSet;
  std::set<std::string> CommonHistoSet;
  pthread_mutex_t mutex;
  pthread_t serverthreadid;
  PHCompositeNode *topNode;
};

#endif /* __ONLMONSERVER_H */
