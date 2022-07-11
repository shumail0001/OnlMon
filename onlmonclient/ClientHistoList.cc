#include "ClientHistoList.h"
#include <TH1.h>

using namespace std;

ClientHistoList::ClientHistoList():
  histo(NULL),
  serverport(0),
  serverhost("UNKNOWN"),
  subsystem("UNKNOWN")
{}

ClientHistoList::ClientHistoList(const string &subsys):
  histo(NULL),
  serverport(0),
  serverhost("UNKNOWN"),
  subsystem(subsys)

{}

ClientHistoList::~ClientHistoList()
{
  delete histo;
}

TH1 *
ClientHistoList::Histo() const
{
  return histo;
}

void 
ClientHistoList::Histo(TH1 *Histo)
{
  histo = Histo;
  return ;
}

const string 
ClientHistoList::ServerHost() const
{
  return serverhost;
}

void 
ClientHistoList::ServerHost(const string &ServerHost)
{
  serverhost = ServerHost;
  return ;
}

const string 
ClientHistoList::SubSystem() const
{
  return subsystem;
}

void 
ClientHistoList::SubSystem(const string &SubSystem)
{
  subsystem = SubSystem;
  return ;
}

void 
ClientHistoList::identify(ostream &os) const
{
  os << "Histo" << histo->GetName()
     << ", subsystem: " << subsystem 
     << ", host: " << serverhost 
     << ", port: " << serverport << endl;
}

void 
ClientHistoList::ServerPort(const int port)
{
  serverport = port;
  return;
}

int 
ClientHistoList::ServerPort() const
{
  return serverport;
}
