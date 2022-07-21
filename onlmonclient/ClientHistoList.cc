#include "ClientHistoList.h"

#include <TH1.h>

ClientHistoList::ClientHistoList()
  : histo(nullptr)
  , serverport(0)
  , serverhost("UNKNOWN")
  , subsystem("UNKNOWN")
{
}

ClientHistoList::ClientHistoList(const std::string &subsys)
  : histo(nullptr)
  , serverport(0)
  , serverhost("UNKNOWN")
  , subsystem(subsys)

{
}

ClientHistoList::~ClientHistoList()
{
  delete histo;
}

TH1 *ClientHistoList::Histo() const
{
  return histo;
}

void ClientHistoList::Histo(TH1 *Histo)
{
  histo = Histo;
  return;
}

const std::string
ClientHistoList::ServerHost() const
{
  return serverhost;
}

void ClientHistoList::ServerHost(const std::string &ServerHost)
{
  serverhost = ServerHost;
  return;
}

const std::string
ClientHistoList::SubSystem() const
{
  return subsystem;
}

void ClientHistoList::SubSystem(const std::string &SubSystem)
{
  subsystem = SubSystem;
  return;
}

void ClientHistoList::identify(std::ostream &os) const
{
  os << "Histo" << histo->GetName()
     << ", subsystem: " << subsystem
     << ", host: " << serverhost
     << ", port: " << serverport << std::endl;
}

void ClientHistoList::ServerPort(const int port)
{
  serverport = port;
  return;
}

int ClientHistoList::ServerPort() const
{
  return serverport;
}
