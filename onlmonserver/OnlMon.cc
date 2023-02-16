#include "OnlMon.h"
#include "OnlMonServer.h"

#include <Event/msg_profile.h>

#include <TH1.h>

#include <cstdio>  // for printf
#include <iostream>
#include <sstream>

class Event;

OnlMon::OnlMon(const std::string &name)
  : OnlMonBase(name)
{
  if (name.find('_') != std::string::npos)
  {
    std::cout << "No underscore (_) in online monitoring server name " << name << " allowed" << std::endl;
    exit(1);
  }
  livetrigmask = 0;
  status = OnlMon::ACTIVE;
  return;
}

int OnlMon::process_event_common(Event *evt)
{
  if (livetrigmask)
  {
    OnlMonServer *se = OnlMonServer::instance();
    if (!(se->Trigger(1) & livetrigmask))
    {
      return 0;
    }
    else
    {
      if (verbosity > 1)
      {
        printf("event accepted, live trig: %08x, mask %08x\n", se->Trigger(1), livetrigmask);
      }
    }
  }
  int iret = process_event(evt);
  return iret;
}

int OnlMon::process_event(Event * /*evt*/)
{
  std::cout << "process_event(Event *evt) not implemented by daughter class" << std::endl;
  return -1;
}

int OnlMon::Reset()
{
  //  cout << "Reset() not implemented by daughter class" << endl;
  return -1;
}

void OnlMon::identify(std::ostream &out) const
{
  out << "identify() not implemented by daughter class" << std::endl;
  return;
}

void OnlMon::AddTrigger(const std::string &name)
{
  if (TriggerList.find(name) != TriggerList.end())
  {
    std::ostringstream msg;
    msg << ThisName << ": Trigger " << name << " already in trigger list";
    OnlMonServer *se = OnlMonServer::instance();
    se->send_message(this, MSG_SOURCE_MONITOR, MSG_SEV_WARNING, msg.str(), 1);
    return;
  }
  TriggerList.insert(name);
  return;
}

void OnlMon::AddLiveTrigger(const std::string &name)
{
  if (LiveTriggerList.find(name) != LiveTriggerList.end())
  {
    std::ostringstream msg;
    msg << ThisName << ": Trigger " << name << " already in live trigger filter list";
    OnlMonServer *se = OnlMonServer::instance();
    se->send_message(this, MSG_SOURCE_MONITOR, MSG_SEV_WARNING, msg.str(), 1);
    return;
  }
  LiveTriggerList.insert(name);
  return;
}

int OnlMon::InitCommon(OnlMonServer *  se)
{
//  m_LocalFrameWorkVars = static_cast<TH1 *>(se->getCommonHisto("FrameWorkVars")->Clone());
  se->registerHisto(this,se->getCommonHisto("FrameWorkVars"));
  return 0;
}

int OnlMon::BeginRunCommon(const int /* runno */, OnlMonServer *se)
{
  livetrigmask = 0;
  if (se->isCosmicRun())  // no trigger selection for cosmic runs
  {
    return 0;
  }
  if (!TriggerList.empty() || !LiveTriggerList.empty())
  {
    unsigned int newmask = 0;
    std::string RunType = se->GetRunType();
    if (RunType == "PHYSICS" || RunType == "ZEROFIELD" || RunType == "CALIBRATION")
    {
      std::set<std::string>::const_iterator iter;
      for (iter = TriggerList.begin(); iter != TriggerList.end(); ++iter)
      {
        if (verbosity > 0)
        {
          std::cout << "Adding trigger " << *iter << " to selection" << std::endl;
        }
        printf("Adding trigger %s to selection\n", (*iter).c_str());
        newmask = se->AddToTriggerMask(*iter);
      }

      for (iter = LiveTriggerList.begin(); iter != LiveTriggerList.end(); ++iter)
      {
        if (verbosity > 0)
        {
          std::cout << "Adding trigger " << *iter << " to live trigger selection" << std::endl;
        }
        printf("Adding trigger %s to live trigger selection\n", (*iter).c_str());
        livetrigmask |= se->getLevel1Bit(*iter);
      }
    }
    printf("scaled trigger mask %08x, live trigger mask %08x\n", newmask, livetrigmask);
  }
  SetStatus(OnlMon::OK);
  return 0;
}

void OnlMon::SetStatus(const int newstatus)
{
  if (newstatus != status)
  {
    OnlMonServer *se = OnlMonServer::instance();
    status = newstatus;
    se->SetSubsystemRunStatus(this, status);
  }
  return;
}

void OnlMon::SetServerId(unsigned int i)
{
  if (Name().find('_') != std::string::npos)
  {
    std::cout << "Server Id was already set " << Name() << std::endl;
    return;
  }
  m_ServerId = i;
  Name(Name() + '_' + std::to_string(i));
  return;
}

