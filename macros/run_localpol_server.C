#include "ServerFuncs.C"

#include <onlmon/localpol/LocalPolMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonllocalpolmon_server.so)

//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/beam/beam_seb14-00024787-0000.prdf")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/junk/junk_seb14-00033119-0000.prdf")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/GL1/beam/GL1_beam_gl1daq-00024787-0000.prdf")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/junk/junk_seb14-00029121-0000.prdf")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/GL1/junk/GL1_junk_gl1daq-00029121-0000.evt")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/ZDC/junk/junk_seb14-00033063-0000.prdf")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/GL1/cosmics/GL1_cosmics_gl1daq-00034390-0000.prdf")
//void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/user/vandrieux/OnlMon/combined/beam_zdc_gl1_41352.prdf")
void run_localpol_server(const std::string &name = "LOCALPOLMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/lustre01/sphnxpro/commissioning/GL1/beam/GL1_beam_gl1daq-00041352-0000.evt")
// /sphenix/lustre01/sphnxpro/commissioning/ZDC/beam/beam_seb20-00041352-0000.prdf
{
  OnlMon *m = new LocalPolMon(name);                     // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
                                                //  m->AddTrigger("PPG(Laser)");  // high efficiency triggers selection at et pool
                                                //  m->AddTrigger("ONLMONBBCLL1"); // generic bbcll1 minbias trigger (defined in ServerFuncs.C)
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);                       // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}
