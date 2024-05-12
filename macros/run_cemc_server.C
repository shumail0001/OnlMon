#include "ServerFuncs.C"

#include <onlmon/cemc/CemcMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlcemcmon_server.so)

// void run_cemc_server(const char *prdffile = "/sphenix/data/data02/sphenix/t1044/rcdaq-00000221-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphnxpro/commissioning/jsh/emcal/eventcombiner/junk-00035646-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/direct/sphenix+lustre01/sphnxpro/commissioning/aligned_prdf/beam-00023727-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphnxpro/commissioning/jsh/emcal/eventcombiner/calib-36291-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/emcal/beam/beam_seb11-00041017-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/emcal/led/led_seb15-00041680-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/user/vandrieux/OnlMon/combined/beam_40796.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/emcal/beam/beam_seb12-00040796-0000.prdf")
void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/user/vandrieux/OnlMon/combined/beam_40796.prdf")

//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/phenix/u/ahodges/sUser/macros_git/analysis/forJohn/macro/prdf/36565_35nsLED/combinedLED-00036565-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/phenix/u/ahodges/sUser/macros_git/analysis/forJohn/macro/prdf/36567_40nsLED/combinedLED-00036567-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/phenix/u/ahodges/sUser/macros_git/analysis/forJohn/macro/prdf/36568_45nsLED/combinedLED-00036568-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/phenix/u/ahodges/sUser/macros_git/analysis/forJohn/macro/prdf/36569_50nsLED/combinedLED-00036569-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/phenix/u/ahodges/sUser/macros_git/analysis/forJohn/macro/prdf/36570_55nsLED/combinedLED-00036570-0000.prdf")
//void run_cemc_server(const std::string &name = "CEMCMON", unsigned int serverid = 0, const std::string &prdffile = "/phenix/u/ahodges/sUser/macros_git/analysis/forJohn/macro/prdf/36571_60nsLED/combinedLED-00036571-0000.prdf")

{
  OnlMon *m = new CemcMon(name);                    // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  gStyle->SetOptStat(0);
  // cemc_runningmean->SetMinimum(0);
  return;
}
