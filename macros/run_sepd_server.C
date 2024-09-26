#include "ServerFuncs.C"

#include <onlmon/sepd/SepdMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlsepdmon_server.so)

// --- long used 40853 for testing
// --- see Rosi's collab meeting presentation https://indico.bnl.gov/event/23072/contributions/91050/attachments/54841/93832/RReed_sEPDCollaboration_052024.pdf
// --- 43840 used in Rosi's wheel plot
// --- 44133 used in Rosi's ADC plot
//void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/beam/beam_seb20-00040853-0000.prdf")
void run_sepd_server(const std::string &name = "SEPDMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/physics/ZDC/physics/physics_seb20-00044133-0000.prdf")
{
  OnlMon *m = new SepdMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}
