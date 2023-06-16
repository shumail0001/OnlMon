#include "ServerFuncs.C"

#include <onlmon/mvtx/MvtxMon.h>

#include <onlmon/OnlMonServer.h>

// cppcheck-suppress unknownMacro
R__LOAD_LIBRARY(libonlmvtxmon_server.so)


//void run_mvtx_server(const char *prdffile = "/sphenix/data/data01/mvtx/tb-1441-052019/longrun/longrun_00000872-0000.prdf")
//void run_mvtx_server(const char *prdffile = "/sphenix/data/data01/mvtx/test_00000096-0000.prdf")  "/sphenix/user/mjpeters/test_00000016-0000.prdf"
//void run_mvtx_server(const char *prdffile = "/sphenix/user/mjpeters/test_00000014-0000.prdf")
void run_mvtx_server(const std::string &name = "MVTXMON", unsigned int serverid = 0, const std::string &prdffile = "/sphenix/data/data01/mvtx/test_00000096-0000.prdf")
{
  OnlMon *m = new MvtxMon(name);  // create subsystem Monitor object
  m->SetMonitorServerId(serverid);
  OnlMonServer *se = OnlMonServer::instance();  // get pointer to Server Framework
  se->registerMonitor(m);        // register subsystem Monitor with Framework
  start_server(prdffile);
  return;
}
