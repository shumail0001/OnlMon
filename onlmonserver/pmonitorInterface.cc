/*
  This is the interface from the monitoring to pmonitor
*/

#include "pmonitorInterface.h"
#include "OnlMonServer.h"

#include "HistoBinDefs.h"
#include "PortNumber.h"

#include <phool/phool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <Event.h>
#include <pmonitor.h>
#pragma GCC diagnostic pop

#include <EventTypes.h>
#include <msg_control.h>
#include <msg_profile.h>

#include <MessageTypes.h>  // for kMESS_OBJECT, kMESS_STRING
#include <TClass.h>
#include <TH1.h>
#include <TInetAddress.h>  // for TInetAddress
#include <TMessage.h>
#include <TROOT.h>
#include <TServerSocket.h>
#include <TSocket.h>
#include <TSystem.h>
#include <TThread.h>

#include <pthread.h>
#include <cstdio>   // for printf, NULL
#include <cstdlib>  // for exit
#include <cstring>  // for strcmp
#include <sstream>
#include <string>

//#define ROOTTHREAD

#ifndef ROOTTHREAD
#define SERVER
#endif

#ifdef SERVER
static void *server(void *);
int ServerThread = 0;
#endif

#ifdef ROOTTHREAD
static void *server(void *);
static TThread *ServerThread = nullptr;
#endif

pthread_mutex_t mutex;
TH1 *FrameWorkVars = 0;

//*********************************************************************

int pinit()
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  Onlmonserver->GetMutex(mutex);
  for (int i = 0; i < kMAXSIGNALS; i++)
  {
    gSystem->IgnoreSignal((ESignals) i);
  }
  pthread_mutex_lock(&mutex);
#if defined(SERVER) || defined(ROOTTHREAD)

  pthread_t ThreadId = 0;
  if (!ServerThread)
  {
    //std::cout << "creating server thread" << std::endl;
#ifdef SERVER

    ServerThread = pthread_create(&ThreadId, nullptr, server, (void *) nullptr);
    Onlmonserver->SetThreadId(ThreadId);
#endif
#ifdef ROOTTHREAD

    ServerThread = new TThread(server, (void *) 0);
    ServerThread->Run();
#endif
  }
#endif
  // for the timestamp we need doubles
  FrameWorkVars = new TH1D("FrameWorkVars", "FrameWorkVars", NFRAMEWORKBINS, 0., NFRAMEWORKBINS);
  Onlmonserver->registerCommonHisto(FrameWorkVars);
  pthread_mutex_unlock(&mutex);

  return 0;
}

//*********************************************************************
int process_event(Event *evt)
{
  static time_t savetmpticks = 0x7FFFFFFF;
  static time_t borticks = 0;
  static time_t eorticks = 0;
  static int eventcnt = 0;
  static int lowrunwarning = 1;
  static int lowrunevents = 0;
  static unsigned int oldetmask = 0xFFFFFFFF;

  OnlMonServer *se = OnlMonServer::instance();
  time_t tmpticks = evt->getTime();
  // first test if a new run has started and call BOR/EOR methods of monitors
  if (se->RunNumber() == -1)
  {
    lowrunevents = 0;
    savetmpticks = 0x7FFFFFFF;
    FrameWorkVars->SetBinContent(LOWRUNEVENTBIN, (Stat_t) lowrunevents);

    eventcnt = 0;
    int newrun = evt->getRunNumber();
    pthread_mutex_lock(&mutex);
    FrameWorkVars->SetBinContent(RUNNUMBERBIN, (Stat_t) newrun);
    se->BadEvents(0);
    se->EventNumber(evt->getEvtSequence());
    se->RunNumber(newrun);
    // set ticks to the current event, so the begin run has the
    // up to date time stamp, while the end run has the time stamp from
    // the last event of the previous run
    se->CurrentTicks(tmpticks);
    se->BeginRun(newrun);
    // set trigger mask in et pool frontend
    unsigned int scaledtrigmask = se->ScaledTrigMask();
    if (scaledtrigmask != oldetmask)  // only bother the et pool if the mask has changed
    {
      if (scaledtrigmask != 0xFFFFFFFF)
      {
        //	      petsetmask(scaledtrigmask);
      }
      else
      {
        // important is only the third parameter to disable et pool selection
        //	      petsetmask(0, 0, 1);
      }
      oldetmask = scaledtrigmask;
    }
    borticks = se->BorTicks();
    FrameWorkVars->SetBinContent(BORTIMEBIN, (Stat_t) borticks);
    pthread_mutex_unlock(&mutex);
    eorticks = borticks;
  }
  if (se->RunNumber() > evt->getRunNumber())
  {
    lowrunevents++;
    FrameWorkVars->SetBinContent(LOWRUNEVENTBIN, (Stat_t) lowrunevents);
    if (lowrunwarning)
    {
      printf("event run number %d smaller than current run number %d discarding event\n", evt->getRunNumber(), se->RunNumber());
      printf("If you are testing open a file with a higher run number\n");
      printf("You will get this warning only once per run\n");
      lowrunwarning = 0;
    }
    return 0;
  }
  /*
  if ((evt->getRunNumber() > 500000 || evt->getRunNumber() < 60000)  && evt->getRunNumber() != 1 && evt->getRunNumber() != 0xFEE2DCB)
    {
      std::ostringstream msg;
      msg << PHWHERE << " huge or tiny event run number "
	  << evt->getRunNumber()
	  << " discarding event" ;
      send_message(MSG_SEV_WARNING, msg.str());
      se->AddBadEvent();
      return 0;
    }
*/
  if (evt->getEvtLength() <= 0 || evt->getEvtLength() > 2500000)
  {
    std::ostringstream msg;
    msg << PHWHERE << "Discarding event with length "
        << evt->getEvtLength();
    send_message(MSG_SEV_WARNING, msg.str());
    se->AddBadEvent();
    return 0;
  }

  int oldrun;
  if ((oldrun = se->RunNumber()) != evt->getRunNumber())
  {
    // ROOT crashes when one thread updates histos while they are
    // being saved, need mutex protection here
    pthread_mutex_lock(&mutex);
    FrameWorkVars->SetBinContent(EORTIMEBIN, (Stat_t) eorticks);  // set EOR time
    se->EndRun(oldrun);
    se->WriteHistoFile();
    se->Reset();  // reset all monitors
    int newrun = evt->getRunNumber();
    FrameWorkVars->SetBinContent(RUNNUMBERBIN, (Stat_t) newrun);
    eorticks = tmpticks;  // initialize eorticks
    se->BadEvents(0);
    se->RunNumber(newrun);
    se->EventNumber(evt->getEvtSequence());
    // set ticks to the current event, so the begin run has the
    // up to date time stamp, while the end run has the time stamp from
    // the last event of the previous run
    se->CurrentTicks(tmpticks);
    se->BeginRun(newrun);
    borticks = se->BorTicks();
    FrameWorkVars->SetBinContent(BORTIMEBIN, (Stat_t) borticks);
    unsigned int scaledtrigmask = se->ScaledTrigMask();
    if (scaledtrigmask != oldetmask)  // only bother the et pool if the mask has changed
    {
      if (scaledtrigmask != 0xFFFFFFFF)
      {
        //	      petsetmask(scaledtrigmask);
      }
      else
      {
        // important is only the third parameter to disable et pool selection
        //	      petsetmask(0, 0, 1);
      }
      oldetmask = scaledtrigmask;
    }
    eventcnt = 0;
    lowrunwarning = 1;  // so we only get one low runnumber warning in logfile
    lowrunevents = 0;   // clear the low run event counter
    FrameWorkVars->SetBinContent(LOWRUNEVENTBIN, (Stat_t) lowrunevents);
    pthread_mutex_unlock(&mutex);
    savetmpticks = 0x7FFFFFFF;
  }

  se->CurrentTicks(tmpticks);
  // check if we get an event which was earlier than the BOR timestamp
  // save earliest time stamp and number of events with earlier timestamps
  if (tmpticks < borticks)
  {
    pthread_mutex_lock(&mutex);
    FrameWorkVars->AddBinContent(EARLYEVENTNUMBIN);
    if (tmpticks < savetmpticks)
    {
      savetmpticks = tmpticks;
      FrameWorkVars->SetBinContent(EARLYEVENTTIMEBIN, (Stat_t) tmpticks);
    }
    pthread_mutex_unlock(&mutex);
  }
  if (eorticks < se->CurrentTicks())
  {
    eorticks = se->CurrentTicks();
  }
  if (se->ScaledTrigMask() != 0xFFFFFFFF && evt->getEvtType() == DATAEVENT && !se->isStandaloneRun())
  {
    if (!(evt->getTagWord(0) & se->ScaledTrigMask()))
    {
      //	  	  std::cout << "discarding 0x" << hex << evt->getTagWord(0) << dec << std::endl;
      return 0;
    }
    //       else
    // 	{
    // 	  std::cout << "accepting 0x" << hex << evt->getTagWord(0) << dec << std::endl;
    // 	}
  }
  if (evt->getErrorCode())
  {
    std::ostringstream msg;
    msg << PHWHERE << " Event with error code: "
        << evt->getErrorCode()
        << " discarding event " << evt->getEvtSequence();
    send_message(MSG_SEV_WARNING, msg.str());
    se->AddBadEvent();
    return 0;
  }

  Packet *p = evt->getPacket(14001);
  if (p)
  {
    se->Trigger(p->iValue(0, 1), 0);
    se->Trigger(p->iValue(0, 2), 1);
    se->Trigger(p->iValue(0, 3), 2);
    //      std::cout << "Accepting 0x" << hex <<  p->iValue(0, SCALEDTRIG) << std::endl;
    //       unsigned int livetrigword = p->iValue(0, LIVETRIG) & 0x00FFFFFF;
    //       for (int i = 24; i < 32; i++)
    // 	{
    // 	  if (!(se->OnlTrig()->get_lvl1_trig_bitmask_bybit(i)))
    // 	    {
    // 	      livetrigword |= ((0x1 << i) & se->Trigger((unsigned short) 0));
    // 	    }
    // 	}
    //       se->Trigger(livetrigword,1);
    delete p;
  }
  else
  {
    unsigned int inittrig = 0;
    se->Trigger(inittrig, 2);
  }
  eventcnt++;
  if (se->Verbosity() > 1)
  {
    printf(" # Events: %d Trigger: 0x%x\n", eventcnt, se->Trigger(2));
  }
  pthread_mutex_lock(&mutex);
  FrameWorkVars->SetBinContent(CURRENTTIMEBIN, (Stat_t) se->CurrentTicks());
  se->EventNumber(evt->getEvtSequence());
  se->process_event(evt);
  pthread_mutex_unlock(&mutex);
  return 0;
}

int setup_server()
{
  return 0;
}

static void *server(void * /* arg */)
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  int MoniPort = MONIPORT;
  //  int thread_arg[5];
  pthread_mutex_lock(&mutex);
  TServerSocket *ss = nullptr;
  sleep(5);
  do
  {
    if (ss)
    {
      delete ss;
    }
    ss = new TServerSocket(MoniPort, kTRUE);
    // Accept a connection and return a full-duplex communication socket.
    Onlmonserver->PortNumber(MoniPort);
    MoniPort++;
    if ((MoniPort - MONIPORT) >= NUMMONIPORT)
    {
      std::ostringstream msg;
      msg << "Too many Online Monitors running on this machine, bailing out";
      send_message(MSG_SEV_FATAL, msg.str());

      exit(1);
    }
    if (!ss->IsValid())
    {
      printf("Ignore ROOT error about socket in use, I try another one\n");
    }
  } while (!ss->IsValid());  // from do{}while

  // root keeps a list of sockets and tries to close them when quitting.
  // this interferes with my own threading and makes valgrind crash
  // The solution is to remove the TServerSocket *ss from roots list of
  // sockets. Then it will leave this socket alone.
  int isock = gROOT->GetListOfSockets()->IndexOf(ss);
  gROOT->GetListOfSockets()->RemoveAt(isock);
  sleep(10);
  pthread_mutex_unlock(&mutex);
again:
  TSocket *s0 = ss->Accept();
  if (!s0)
  {
    std::cout << "Server socket " << MONIPORT
              << " in use, either go to a different node or" << std::endl
              << "change MONIPORT in server/PortNumber.h and recompile" << std::endl
              << "server and client" << std::endl;
    exit(1);
  }
  // mutex protected since writing of histo
  // to outgoing buffer and updating by other thread do not
  // go well together
  if (Onlmonserver->Verbosity() > 2)
  {
    TInetAddress adr = s0->GetInetAddress();
    std::cout << "got connection from " << std::endl;
    adr.Print();
  }
  //  std::cout << "try locking mutex" << std::endl;
  pthread_mutex_lock(&mutex);
  //std::cout << "got mutex" << std::endl;
  handleconnection(s0);
  //std::cout << "try releasing mutex" << std::endl;
  pthread_mutex_unlock(&mutex);
  //std::cout << "mutex released" << std::endl;
  delete s0;
  /*
    if (!aargh)
    {
    std::cout << "making thread" << std::endl;
    aargh = new TThread(handletest,(void *)0);
    aargh->Run();
    }
  */
  //std::cout << "closing socket" << std::endl;
  //s0->Close();
  goto again;
}

void handletest(void * /* arg */)
{
  //  std::cout << "threading" << std::endl;
  return;
}

void handleconnection(void *arg)
{
  TSocket *s0 = (TSocket *) arg;

  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  /*
    int val;
    s0->GetOption(kSendBuffer, val);
    printf("sendbuffer size: %d\n", val);
    s0->GetOption(kRecvBuffer, val);
    printf("recvbuffer size: %d\n", val);
  */
  TMessage *mess = nullptr;
  TMessage outgoing(kMESS_OBJECT);
  while (1)
  {
    if (Onlmonserver->Verbosity() > 2)
    {
      std::cout << "Waiting for message" << std::endl;
    }
    s0->Recv(mess);
    if (!mess)
    {
      std::cout << "Broken Connection, closing socket" << std::endl;
      break;
    }
    if (mess->What() == kMESS_STRING)
    {
      char str[64];
      mess->ReadString(str, 64);
      delete mess;
      mess = 0;
      if (Onlmonserver->Verbosity() > 2)
      {
        std::cout << "received message" << str << std::endl;
      }
      if (!strcmp(str, "Finished"))
      {
        break;
      }
      else if (!strcmp(str, "WriteRootFile"))
      {
        Onlmonserver->WriteHistoFile();
        s0->Send("Finished");
        break;
      }
      else if (!strcmp(str, "Ack"))
      {
        continue;
      }
      else if (!strcmp(str, "HistoList"))
      {
        if (Onlmonserver->Verbosity() > 2)
        {
          std::cout << "number of histos: " << Onlmonserver->nHistos() << std::endl;
        }
        for (unsigned int i = 0; i < Onlmonserver->nHistos(); i++)
        {
          if (Onlmonserver->Verbosity() > 2)
          {
            std::cout << "HistoName: " << Onlmonserver->getHistoName(i) << std::endl;
          }
          s0->Send(Onlmonserver->getHistoName(i).c_str());
          int nbytes = s0->Recv(mess);
          delete mess;
          mess = 0;
          if (nbytes <= 0)
          {
            std::ostringstream msg;

            msg << "Problem receiving message: return code: " << nbytes;
            send_message(MSG_SEV_ERROR, msg.str());
          }
        }
        s0->Send("Finished");
      }
      else if (!strcmp(str, "ALL"))
      {
        if (Onlmonserver->Verbosity() > 2)
        {
          std::cout << "number of histos: " << Onlmonserver->nHistos() << std::endl;
        }
        for (unsigned int i = 0; i < Onlmonserver->nHistos(); i++)
        {
          TH1 *histo = Onlmonserver->getHisto(i);
          if (histo)
          {
            outgoing.Reset();
            outgoing.WriteObject(histo);
            s0->Send(outgoing);
            outgoing.Reset();
            s0->Recv(mess);
            delete mess;
            mess = 0;
          }
        }
        s0->Send("Finished");
      }
      else if (!strcmp(str, "LIST"))
      {
        s0->Send("go");
        while (1)
        {
          char strmess[200];
          s0->Recv(mess);
          if (!mess)
          {
            break;
          }
          if (mess->What() == kMESS_STRING)
          {
            mess->ReadString(strmess, 200);
            delete mess;
            mess = 0;
            if (!strcmp(strmess, "alldone"))
            {
              break;
            }
          }
          TH1 *histo = Onlmonserver->getHisto(strmess);
          if (histo)
          {
            outgoing.Reset();
            outgoing.WriteObject(histo);
            s0->Send(outgoing);
            outgoing.Reset();
          }
          else
          {
            s0->Send("unknown");
          }
          //		  delete mess;
        }
        s0->Send("Finished");
      }
      else
      {
        TH1 *histo = Onlmonserver->getHisto(str);
        if (histo)
        {
          //		  const char *hisname = histo->GetName();
          outgoing.Reset();
          outgoing.WriteObject(histo);
          s0->Send(outgoing);
          outgoing.Reset();
          s0->Recv(mess);
          delete mess;
          s0->Send("Finished");
        }
        else
        {
          s0->Send("UnknownHisto");
        }
      }
    }
    else if (mess->What() == kMESS_OBJECT)
    {
      printf("got object of class: %s\n", mess->GetClass()->GetName());
      delete mess;
    }
    else
    {
      printf("*** Unexpected message ***\n");
      delete mess;
    }
  }

  // Close the socket.
  s0->Close();
  return;
}

int send_message(const int severity, const std::string &msg)
{
  // check $ONLINE_MAIN/include/msg_profile.h for MSG defs
  // if you do not find your subsystem, do not initialize it and drop me a line
  msg_control *Message = new msg_control(MSG_TYPE_MONITORING,
                                         MSG_SOURCE_DAQMON,
                                         severity, "pmonitorInterface");
  std::cout << *Message << msg << std::endl;
  delete Message;
  return 0;
}
