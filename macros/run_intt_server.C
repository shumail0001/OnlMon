#include <ServerFuncs.C>

#include <onlmon/intt/InttMonConstants.h>
#include <onlmon/intt/InttMon.h>
#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlinttmon_server.so)

void run_intt_server(const std::string &name = "INTTMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data02/sphenix/t1044/rcdaq-00000221-0000.prdf")
{
//  //for debugging
//  InttMon *m = new InttMon();
//  m->MiscDebug();

  OnlMon *m = new InttMon(name);      // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
//  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  se->registerMonitor(m);       // register subsystem Monitor with Framework
  start_server(prdffile);

//**********************************************************
//
// just a comment: This is not how this will be run
//
//**********************************************************
  //m->Init(); this must already be called somewhere above
  m->BeginRun(1);

  int N = 1;
  for(int n = 0; n < N; n++)
  {
    m->process_event(static_cast<Event*>(0x0));
  }

  return;
}
