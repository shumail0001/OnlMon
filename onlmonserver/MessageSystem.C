#include "MessageSystem.h"
#include "OnlMonServer.h"

#include "msg_control.h"

using namespace std;

MessageSystem::MessageSystem(const string &name): OnlMonBase(name)
{
  Message = new msg_control(MSG_TYPE_MONITORING, MSG_SOURCE_DEFAULT, MSG_SEV_DEFAULT, name.c_str());
  return ;
}

MessageSystem::~MessageSystem()
{
  delete Message;
  msgcounter.clear();
  return ;
}

int
MessageSystem::send_message(const int msgsource, const int severity, const string &err_message, const int msgtype)
{
  Message->set_source(msgsource);
  Message->set_severity(severity);
  map<int, pair<int, int> >::iterator iter;
 tryagain:
  iter = msgcounter.find(msgtype);
  if (iter == msgcounter.end())
    {
      pair<int, int> newpair( -10, 2);
      msgcounter[msgtype] = newpair;
      goto tryagain;
    }
  (iter->second).first++;
  if ((iter->second).first > 0)
    {
      if ((iter->second).first == (iter->second).second)
        {
          cout << *Message << err_message << endl;
          (iter->second).first=0;
          (iter->second).second*=2;
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
      cout << *Message << err_message << endl;
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
