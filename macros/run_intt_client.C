#include <CommonFuncs.C>

#include <onlmon/intt/InttMonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlinttmon_client.so)

void inttDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
	
	int NARMS = 2;
	int NSTATIONS = 4;
	int NROCS = 24;
	for(int i = 0; i < NARMS; i++)
	{
		for(int istation = 0; istation < NSTATIONS; istation++)
		{
			cl->registerHisto(Form("hFvtxDisk%d_%d", i, istation), "INTTMON");
			cl->registerHisto(Form("hFvtxAdc%d_%d", i, istation), "INTTMON");
		}
  		cl->registerHisto(Form("hFvtxYields%d", i), "INTTMON");
  		cl->registerHisto(Form("hFvtxYieldsByPacket%d", i), "INTTMON");
  		cl->registerHisto(Form("hFvtxControlWord%d", i), "INTTMON");
  		cl->registerHisto(Form("hFvtxYieldsByPacketVsTime%d", i), "INTTMON");
  		cl->registerHisto(Form("hFvtxYieldsByPacketVsTimeShort%d", i), "INTTMON");
	}

	cl->registerHisto("fvtxH1NumEvent", "INTTMON");
	cl->registerHisto("hFvtxEventNumberVsTime", "INTTMON");
	cl->registerHisto("hFvtxEventNumberVsTimeShort", "INTTMON");

	for(int iroc = 0; iroc < NROCS; iroc++)
	{
		cl->registerHisto(Form("hFvtxChipChannel_ROC%d", iroc), "INTTMON");
	}

  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("INTTMON", 1);
  OnlMonDraw *inttmon = new InttMonDraw();    // create Drawing Object
  cl->registerDrawer(inttmon);              // register with client framework
}

void inttDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("INTTMON");         // update histos
  cl->Draw("INTTMON",what);                       // Draw Histos of registered Drawers
}

void inttPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("INTTMON");                          // Create PS files
  return;
}

void inttHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("INTTMON");                        // Create html output
  return;
}
