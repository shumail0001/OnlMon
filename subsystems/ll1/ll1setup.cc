#include "LL1HEADER.h"

#include <onlmon/OnlMon.h>

// Event library

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/msg_profile.h>

// ROOT

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

void process_jet(Packet *p, LL1HEADER *&ll1h);
void process_emcal(int pid, Packet *p, LL1HEADER *&ll1h);
void ll1setup(Event *evt, LL1HEADER *&ll1h)
{

  for ( Int_t ixmit = 0; ixmit < NXMIT; ixmit++ ) {

    Packet* p;
    Int_t pid= PACKET[ixmit];
    p = evt->getPacket( pid );
    if ( p ) {
      ll1h->runnumber = evt->getRunNumber();
      ll1h->evtnr = p->iValue(0,"EVTNR");
      ll1h->clock = p->iValue(0,"CLOCK");
      ll1h->monitor = p->iValue(0,"MONITOR");
      ll1h->nsamples = p->iValue(0,"SAMPLES");

      if (pid == 13002)
	{
	  process_jet(p, ll1h);
	}
      else if (pid >= 13010)
	{
	  process_emcal(pid, p, ll1h);
	}            
      else if (pid == 13001)
	{
	  int indx1=0; int inhit1=0; 
	  int indx2=0; int inhit2=0;
	  for(int is = 0; is < NRSAM; is++){
	    ll1h->nhit_s1[is] =  p->iValue(is,NHITCHANNEL+NADCSH*0);
	    ll1h->nhit_s2[is] =  p->iValue(is,NHITCHANNEL+NADCSH*1);
	    ll1h->nhit_n1[is] =  p->iValue(is,NHITCHANNEL+NADCSH*2);
	    ll1h->nhit_n2[is] =  p->iValue(is,NHITCHANNEL+NADCSH*3);

	    ll1h->nhit_n[is] = ll1h->nhit_n1[is] + ll1h->nhit_n2[is];
	    ll1h->nhit_s[is] = ll1h->nhit_s1[is] + ll1h->nhit_s2[is];

	    ll1h->timesum_s1[is] =  p->iValue(is,NHITCHANNEL+1+NADCSH*0);
	    ll1h->timesum_s2[is] =  p->iValue(is,NHITCHANNEL+2+NADCSH*1);
	    ll1h->timesum_n1[is] =  p->iValue(is,NHITCHANNEL+3+NADCSH*2);
	    ll1h->timesum_n2[is] =  p->iValue(is,NHITCHANNEL+4+NADCSH*3);

	    ll1h->timesum_s[is] = ll1h->timesum_n1[is] + ll1h->timesum_n2[is];
	    ll1h->timesum_n[is] = ll1h->timesum_s1[is] + ll1h->timesum_s2[is];

	    if( ll1h->nhit_n[is]>0){
	      if(inhit1 <  ll1h->nhit_n[is]){
		indx1 = is;
		inhit1 = ll1h->nhit_n[is];
	      } 
	    }

	    if( ll1h->nhit_s[is]>0){
	      if(inhit2 <  ll1h->nhit_s[is]){
		indx2 = is;
		inhit2 = ll1h->nhit_s[is];
	      } 
	    }

	    ll1h->chargesum_n1[is] = 0;
	    ll1h->chargesum_n2[is] = 0;
	    ll1h->chargesum_s1[is] = 0;
	    ll1h->chargesum_s2[is] = 0;
	    ll1h->chargesum_s[is] = 0;
	    ll1h->chargesum_n[is] = 0;

	    for(int ic = 0; ic <NCH; ic++) {
	      ll1h->channel[ic][is] = p->iValue(is,ic);
	      if(ic<NHITCHANNEL) ll1h->chargesum_s1[is] += p->iValue(is,ic);
	      else if(ic>=NADCSH*1 && ic<NHITCHANNEL+NADCSH*1) ll1h->chargesum_s2[is] += p->iValue(is,ic);
	      else if(ic>=NADCSH*2 && ic<NHITCHANNEL+NADCSH*2) ll1h->chargesum_n1[is] += p->iValue(is,ic);
	      else if(ic>=NADCSH*3 && ic<NHITCHANNEL+NADCSH*3) ll1h->chargesum_n2[is] += p->iValue(is,ic);
	      ll1h->chargesum_n[is] = ll1h->chargesum_n1[is] + ll1h->chargesum_n2[is];
	      ll1h->chargesum_s[is] = ll1h->chargesum_s1[is] + ll1h->chargesum_s2[is];
	    }
	    for(int it=NCH; it<(NCH+NTRIGWORDS); it++){
	      ll1h->triggerwords[it - NCH][is] =  p->iValue(is,it);
	    }
	  }	
	  ll1h->idxhitn = indx1;
	  ll1h->idxhits = indx2;

	  ll1h->idxsample = (ll1h->idxhitn==ll1h->idxhits) ? ll1h->idxhitn : -1;

	  delete p;
	}
    }
    }
}

void process_jet(Packet *p, LL1HEADER *&ll1h)
{
  
  // go through all input fibers

  for (int i = 0; i < 16; i++)
    {
      ll1h->jet_sample[i] = -1;
      for (int j =0; j < 24; j++)
	{
	  for (int is = 0; is < p->iValue(0, "SAMPLES"); is++)
	    {
	      int value = p->iValue(is, i*24 + j);
	      if (value)
		{
		  ll1h->jet_sample[i] = is;
		  ll1h->jet_input[j%12][j/12 + i*2] = value;
		}
	    }
	}
    }

  for (int i = 0; i < p->iValue(0, "TRIGGERWORDS"); i++)
    {
      for (int is = 0; is < p->iValue(0,"SAMPLES"); is++)
	{
	  int value = p->iValue(is, 16*24 + i);
	  if (value)
	    {
	      ll1h->jet_output[i/32][i%32] = value;
	    }
	}
    }

  return;
}

void process_emcal(int pid, Packet *p, LL1HEADER *&ll1h)
{
  
  // go through all input fibers

  int emcal_board = pid - 13010;

  ll1h->emcal_sample[emcal_board] = -1;

  for (int i = 0; i < p->iValue(0, "CHANNELS"); i++)
    {
      for (int is = 0; is < p->iValue(0, "SAMPLES"); is++)
	{
	  int value = p->iValue(is, i);

	  ll1h->emcal_2x2_map[((i/16)%12) * 4 + (i%4)][emcal_board*8 + (i%16)/4] = value;
	  
	}
    }
  for (int i = 0; i < p->iValue(0, "TRIGGERWORDS"); i++)
    {
      for (int is = 0; is < p->iValue(0, "SAMPLES"); is++)
	{
	  int value = p->iValue(is, 16*24 + i);
	  if (value)
	    {
	      ll1h->emcal_sample[emcal_board] = is;
	      ll1h->emcal_8x8_map[i%12][emcal_board*2 + i/12] = value;
	    }
	}
    }
  return;
}
