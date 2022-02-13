#ifndef __MESSAGESYSTEM_H__
#define __MESSAGESYSTEM_H__

#include <OnlMonBase.h>

#include <msg_profile.h>

#include <map>
#include <string>

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
