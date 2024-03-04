#include "ServerFuncs.C"

#include <onlmon/zdc/ZdcMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlzdcmon_server.so)

void run_zdc_server(const std::string &name = "ZDCMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/beam/beam_seb14-00023676-0000.prdf")
{
  cout << "first comment" << enld;
  OnlMon *m = new ZdcMon(name);                     // create subsystem Monitor object
  cout << "second comment" << enld;
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  cout << "third comment" << enld;
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  cout << "fourth comment" << enld;
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  cout << "fifth comment" << enld;
  start_server(prdffile);
  cout << "sixth comment" << enld;
  return;
}
