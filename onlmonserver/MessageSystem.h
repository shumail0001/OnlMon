#ifndef ONLMONSERVER_MESSAGESYSTEM_H
#define ONLMONSERVER_MESSAGESYSTEM_H

#include <OnlMonBase.h>

#include <map>
#include <string>
#include <utility>       // for pair

class msg_control;

class MessageSystem: public OnlMonBase
{
 public:
  MessageSystem(const std::string &name);
  virtual ~MessageSystem();

  int send_message(const int msg_source, const int severity, const std::string &err_message, const int msgtype);

  int Reset();

 protected:

  msg_control *Message;
  std::map<int, std::pair<int,int> > msgcounter;

};

#endif
