#include "MessageSystem.h"

#include "OnlMonBase.h"  // for OnlMonBase
#include "OnlMonServer.h"

#include <Event/msg_control.h>
#include <Event/msg_profile.h>  // for MSG_SEV_DEFAULT, MSG_SEV_INFORMATIONAL

#include <iostream>  // for endl, ostream, basic_ostream, cout, std

MessageSystem::MessageSystem(const std::string &name)
  : OnlMonBase(name)
{
  Message = new msg_control(MSG_TYPE_MONITORING, MSG_SOURCE_DEFAULT, MSG_SEV_DEFAULT, name.c_str());
  return;
}

MessageSystem::~MessageSystem()
{
  delete Message;
  msgcounter.clear();
  return;
}

int MessageSystem::send_message(const int msgsource, const int severity, const std::string &err_message, const int msgtype)
{
  Message->set_source(msgsource);
  Message->set_severity(severity);
  std::map<int, std::pair<int, int> >::iterator iter;
tryagain:
  iter = msgcounter.find(msgtype);
  if (iter == msgcounter.end())
  {
    std::pair<int, int> newpair(-10, 2);
    msgcounter[msgtype] = newpair;
    goto tryagain;
  }
  (iter->second).first++;
  if ((iter->second).first > 0)
  {
    if ((iter->second).first == (iter->second).second)
    {
      std::cout << *Message << err_message << std::endl;
      (iter->second).first = 0;
      (iter->second).second *= 2;
    }
    //       else
    // 	{
    // 	  cout << "Not sending " << (iter->second).first
    // 	       << ", 2nd: " << (iter->second).second
    // 	       << ", %: " << (iter->second).first%(iter->second).second << endl;
    // 	}
  }
  else
  {
    std::cout << *Message << err_message << std::endl;
  }
  if (severity > MSG_SEV_INFORMATIONAL)
  {
    OnlMonServer *se = OnlMonServer::instance();
    se->WriteLogFile(ThisName, err_message);
  }
  return 0;
}

int MessageSystem::Reset()
{
  msgcounter.clear();
  return 0;
}
