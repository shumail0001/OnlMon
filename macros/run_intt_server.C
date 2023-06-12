#include "ServerFuncs.C"

#include <onlmon/intt/InttMonConstants.h>
#include <onlmon/intt/InttMon.h>
#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlinttmon_server.so)

//std::string _prdf_file = "/sphenix/u/jbertaux/evt_files/intt_intt0-00000025-0000.evt";
//std::string _prdf_file = "/sphenix/u/jbertaux/evt_files/intt_intt2-00000023-0000.evt";
std::string _prdf_file = "/sphenix/lustre01/sphnxpro/commissioning/INTT/beam/beam_intt0-00009328-0000.evt";


void run_intt_server(const std::string &name = "INTTMON", unsigned int serverid = 0, const std::string &prdffile = _prdf_file)
{
//  //for debugging
//  InttMon *m = new InttMon(name);
//  m->MiscDebug();

  std::cout << "name:\t" << name << std::endl;
  std::cout << "Server ID:\t" << serverid << std::endl;
  std::cout << "Using .evt file:" << std::endl;
  std::cout << "\t" << prdffile << std::endl;

  OnlMon *m = new InttMon(name);      // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
  //m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

  //**********************************************************
  //
  // just a comment: This is not how this will be run
  //
  //**********************************************************
  //m->Init(); this must already be called somewhere above
  //m->BeginRun(1);

  //pidentify(0);
  prun(300);

  //for debugging
  //static_cast<InttMon*>(m)->RandomEvent(serverid);

  return;
}
