#include "PktSizeMon.h"
#include "PktSizeCommon.h"
#include "PktSizeDBodbc.h"

#include <onlmon/OnlMonServer.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/msg_profile.h>

#include <phool/phool.h>

#include <TH1.h>

#include <iostream>
#include <sstream>

const char *histoname = "pktsize_hist";
const int NUPDATE = 1000;

using namespace std;

PktSizeMon::PktSizeMon(const char *name): 
  OnlMon(name),
  nevnts(0),
  db(new PktSizeDBodbc(name))
{
  return ;
}

PktSizeMon::~PktSizeMon()
{
  Reset();
  delete db;
  return ;
}

// void
// PktSizeMon::Verbosity(const int i)
// {
//  if (db)
//     {
//        db->Verbosity(i);
//     }
//   OnlMon::Verbosity(i);
//   return ;
// }

int
PktSizeMon::Init()
{
  OnlMonServer *se = OnlMonServer::instance();
  sizehist = new TH1F(histoname, "dummy", 1, 0, 1);
  se->registerHisto(this, sizehist);
  PktSizeCommon::fillgranules(granulepacketlimits);
  PktSizeCommon::filldcmgroups(dcmgroups);
  PktSizeCommon::fillfibergroups(fibergroups);
  return 0;
}


int
PktSizeMon::Reset()
{
  packetsize.clear();

  OnlMonServer *se = OnlMonServer::instance();
  TH1 *newhist = new TH1F("pktsize_tmp", "packet size storage facility", 1, 0, 1);
  se->registerHisto(this->Name(), histoname, newhist, 1);
  sizehist = newhist;
  nevnts = 0;
  return 0;
}


int
PktSizeMon::process_event(Event *e)
{
  // only data events
  if (e->getEvtType() != DATAEVENT)
    {
      return 0;
    }
  int nw = e->getPacketList(plist, 10000);
  if (nw >= 10000)
    {
      ostringstream errmsg;
      errmsg << "Packet array too small, need at least " << nw << " entries";
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DAQMON, MSG_SEV_ERROR, errmsg.str(), 1);
      nw = 10000;
    }
  int packetid;
  unsigned int size;
  map<unsigned int, unsigned int>::iterator mapiter;
  for (int i = 0; i < nw;i++)
    {
      packetid = plist[i]->getIdentifier();
      size = (plist[i]->getLength());
      packetsize[packetid] += size;
      //      string dcmgrp = dcmgroups[packetid];
      //      dcmgroupsize[dcmgrp] += size;
//       mapiter = packetsize.find(packetid);
//       if (mapiter != packetsize.end())
//         {
//           mapiter->second += size;
//         }
//       else
//         {
//           packetsize[packetid] = size;
//         }
      delete plist[i];
    }
  nevnts++;
  if (nevnts % NUPDATE == 0)
    {
      if (verbosity > 0)
        {
          cout << "putting map into histos" << endl;
        }
      putmapinhisto();
    }
  return 0;
}

int
PktSizeMon::EndRun(const int runno)
{
  putmapinhisto();
  UpdateDB(runno);
  return 0;
}

int
PktSizeMon::putmapinhisto()
{
  OnlMonServer *se = OnlMonServer::instance();
  if (packetsize.size() != (unsigned int) sizehist->GetNbinsX())
    {
      TH1 *newhist = new TH1F("pktsize_tmp", "packet size storage facility", packetsize.size(), 0, packetsize.size());
      se->registerHisto(this->Name(), histoname, newhist, 1);
      sizehist = newhist;
    }
  map<unsigned int, unsigned int>::const_iterator mapiter;
  int nbin = 1;
  double aversize;
  sizehist->SetBinContent(0,nevnts); // fill number of evts into 0th bin
  for (mapiter = packetsize.begin(); mapiter != packetsize.end(); ++mapiter)
    {
      aversize = (double) (mapiter->second) / (double) (nevnts);
      sizehist->SetBinContent(nbin, aversize);
      sizehist->SetBinError(nbin, mapiter->first);
      nbin++;
    }
  return 0;
}

void
PktSizeMon::Print(const string &what)
{
  if (what == "ALL")
    {
      map<unsigned int, unsigned int>::const_iterator mapiter;
      for (mapiter = packetsize.begin(); mapiter != packetsize.end(); ++mapiter)
        {
          cout << "Packet " << mapiter->first
	       << ", SumBytes " << mapiter->second
	       << ", Average " << (float) (mapiter->second) / (float) (nevnts)
	       << endl;
	  if (dcmgroups.find(mapiter->first) == dcmgroups.end())
	    {
	      cout << "could not find dcm group for packet " << mapiter->first << endl;
	    }
	  else
	    {
          string dcmgrp = dcmgroups[mapiter->first];
	  dcmgroupsize[dcmgrp] += mapiter->second;
          string fibergrp = fibergroups[mapiter->first];
          fibergroupsize[fibergrp] += mapiter->second;
	    }
        }

      map<string,unsigned int>::const_iterator iter;
      for (iter = dcmgroupsize.begin(); iter != dcmgroupsize.end(); ++iter)
	{
	  cout << "DCM group " << iter->first
	       << ", SumBytes " << iter->second
	       << ", Average " << (float) (iter->second) / (float) (nevnts)
	       << endl;
	}

      for (iter = fibergroupsize.begin(); iter != fibergroupsize.end(); ++iter)
	{
	  cout << "Fiber group " << iter->first
	       << ", SumBytes " << iter->second
	       << ", Average " << (float) (iter->second) / (float) (nevnts)
	       << endl;
	}
    }
  if (what == "GRANULES")
    {
      map<string, pair<unsigned int, unsigned int> >::const_iterator mapiter;
      for (mapiter = granulepacketlimits.begin(); mapiter != granulepacketlimits.end(); ++mapiter)
        {
          cout << "Granule " << mapiter->first
	       << ", Min Packet " << mapiter->second.first
	       << ", Max Packet " << mapiter->second.second
	       << endl;
        }
    }
  if (what == "SORT")
    {
      multimap<double, int> sortlist;
      map<unsigned int, unsigned int>::const_iterator mapiter;
      for (mapiter = packetsize.begin(); mapiter != packetsize.end(); ++mapiter)
        {
          double aversize = (double) (mapiter->second) / (double)(nevnts);
          sortlist.insert(pair<double, int>(aversize, mapiter->first));
        }
      multimap<double, int>::const_iterator mmapiter;
      for (mmapiter = sortlist.begin(); mmapiter != sortlist.end(); ++mmapiter)
        {
          cout << "Packet " << mmapiter->second
	       << " Size: " << mmapiter->first
	       << " Bytes "
	       << endl;
        }
    }
  if (what == "DCMGROUP")
    {
      map<string,set<unsigned int> > dcms;
      map<unsigned int,string>::const_iterator iter;
      for (iter = dcmgroups.begin(); iter != dcmgroups.end(); ++iter)
	{
	  map<string,set<unsigned int> >::iterator iter2 = dcms.find(iter->second);
	  if (iter2 == dcms.end())
	    {
	      set<unsigned int> newset;
	      newset.insert(iter->first);
	      dcms[iter->second] = newset;
	    }
	  else
	    {
	      (iter2->second).insert(iter->first);
	    }
	}
      map<string,set<unsigned int> >::const_iterator iter3;
      set<unsigned int>::const_iterator iter4;
      for (iter3 = dcms.begin(); iter3 != dcms.end(); ++iter3)
	{
	  cout << "dcm group " << iter3->first << " packets: " << endl;
	  for (iter4 = (iter3->second).begin(); iter4 != (iter3->second).end(); ++iter4)
	    {
	      cout << *iter4 << " ";
	    }
	  cout << endl;
	}
    }
  return ;
}

int
PktSizeMon::UpdateDB(const int runno)
{
  map<string, pair<unsigned int, unsigned int> >::const_iterator graniter;
  string name;
  unsigned int lolim, hilim;
  map<unsigned int, unsigned int> granpackets;
  map<unsigned int, unsigned int>::iterator piter0, piter1, piter2;
  for (graniter = granulepacketlimits.begin(); graniter != granulepacketlimits.end(); ++graniter)
    {
      name = graniter->first;
      lolim = graniter->second.first;
      hilim = graniter->second.second;
      piter1 = packetsize.lower_bound(lolim);
      if (piter1->first < hilim)
        {
          if (verbosity > 0)
            {
              piter2 = packetsize.upper_bound(hilim);
              --piter2;
              cout << "Name: " << name << " first packet: " << piter1->first
		   << " last packet: " << piter2->first << endl;
            }
        }
      else
	{
	  if (verbosity > 0)
	    {
	      cout << "Name: " << name << " No packet found" << endl;
	    }
	  continue;
        }
      for (piter0 = piter1; piter0 != packetsize.upper_bound(hilim); ++piter0)
        {
          granpackets[piter0->first] = piter0->second;
        }
      db->AddRow(name, runno, nevnts, granpackets);
      packetsize.erase(packetsize.lower_bound(lolim), packetsize.upper_bound(hilim));
      granpackets.clear();
    }
  if (packetsize.size() > 0)
    {
      cout << "Non assigned packets found" << endl;
      for (piter0 = packetsize.begin(); piter0 != packetsize.end(); ++piter0)
        {
          cout << "Packet ID: " << piter0->first << endl;
        }
      ostringstream errmsg;
      errmsg << "PktSizeMon::UpdateDB() unassigned packets found:";
      for (piter0 = packetsize.begin(); piter0 != packetsize.end(); ++piter0)
        {
          errmsg << " " << piter0->first;
        }
      OnlMonServer *se = OnlMonServer::instance();
      se->send_message(this, MSG_SOURCE_DAQMON, MSG_SEV_ERROR, errmsg.str(), 2);
      db->AddRow("nogran", runno, nevnts, packetsize);
    }
  packetsize.clear();
  return 0;
}

