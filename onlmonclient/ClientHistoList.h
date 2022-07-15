#ifndef CLIENTHISTOLIST_H__
#define CLIENTHISTOLIST_H__

#include <iostream>
#include <string>

class TH1;

class ClientHistoList
{
 public:
  ClientHistoList();
  explicit ClientHistoList(const std::string &subsys);

  virtual ~ClientHistoList();

  TH1 *Histo() const;
  void Histo(TH1 *Histo);
  const std::string ServerHost() const;
  void ServerHost(const std::string &ServerHost);
  const std::string SubSystem() const;
  void SubSystem(const std::string &SubSystem);
  void ServerPort(const int port);
  int ServerPort() const;
  void identify(std::ostream &os = std::cout) const;

 protected:
  TH1* histo;
  int serverport;
  std::string serverhost;
  std::string subsystem;

};

#endif /* __CLIENTHISTOLIST_H__ */

