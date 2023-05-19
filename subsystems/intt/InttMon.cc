#include "InttMon.h"

//===	Public Methods		===//
InttMon::InttMon(const std::string &name) : OnlMon(name)
{
	//leaving fairly empty
	return;
}

InttMon::~InttMon()
{
	delete dbvars;
}

int InttMon::Init()
{
	OnlMonServer *omc = OnlMonServer::instance();

	//dbvars
	dbvars = new OnlMonDB(ThisName);
	DBVarInit();

	//histograms
	NumEvents = new TH1D(Form("InttNumEvents"), Form("InttNumEvents"), 1, 0, 1);
	HitMap = new TH1D(Form("InttMap"), Form("InttMap"), INTT::ADCS, 0, INTT::ADCS);
	//...

	omc->registerHisto(this, NumEvents);
	omc->registerHisto(this, HitMap);
	//...

	//Read in calibrartion data from InttMonData.dat
	std::string fullfile = std::string(getenv("INTTCALIB")) + "/" + "InttMonData.dat";
	std::ifstream calib(fullfile);
	//probably need to do stuff here (maybe write to expectation maps)
	//or reimplment in BeginRun()
	calib.close();

	// for testing/debugging without unpacker, remove later
	rng = new TRandom(1234);
	//~for testing/debugging without unpacker, remove later

	Reset();

	return 0;
}

int InttMon::BeginRun(const int /* run_num */)
{
	//per-run calibrations; don't think we need to do anything here yet

	return 0;
}

int InttMon::process_event(Event* evt)
{
	int bin;
	int N;
	int n;

	int pid;
	int felix;
	int felix_channel;
	struct INTT_Felix::Ladder_s lddr_s;

	//int bin;
	struct INTT::Indexes_s indexes;

	for(pid = 3001; pid < 3009; ++pid)
	{
		Packet* p = evt->getPacket(pid);
		if(!p)continue;

		N = p->iValue(0, "NR_HITS");

		if(N)std::cout << N << std::endl;

		for(n = 0; n < N; ++n)
		{
			felix = pid - 3001;
			felix_channel = p->iValue(n, "FEE");

			INTT_Felix::FelixMap(felix, felix_channel, lddr_s);

			indexes.lyr = lddr_s.barrel * 2 + lddr_s.layer;
			indexes.ldr = lddr_s.ladder;

			indexes.arm = ((pid - 3001) / 4) % 2;

			indexes.chp = p->iValue(n, "CHP_ID");
			indexes.chp = (indexes.chp - 1) % 26;

			indexes.chn = p->iValue(n, "CHANNEL_ID");

			indexes.adc = p->iValue(n, "ADC");

			INTT::GetBinFromIndexes(bin, indexes);
			HitMap->AddBinContent(bin);
		}
	}

	////dummy method since unpacker is not done yet
	//double temp;


	////...

	//int hits = rng->Poisson(HITS_PER_EVENT);
	//for(int hit = 0; hit < hits; hit++)
	//{
	//	//randomly choose a chip/channel for the hit to occur
	//	//including guards in case upper bound of TRandom::Uniform(Double_t) is inclusive
	//	layer = rng->Uniform(INTT::LAYER);
	//	if(layer == INTT::LAYER)layer -= 1;

	//	ladder = rng->Uniform(INTT::LADDER[layer]);
	//	if(ladder == INTT::LADDER[layer])ladder -= 1;

	//	northsouth = rng->Uniform(INTT::NORTHSOUTH);
	//	if(northsouth == INTT::NORTHSOUTH)northsouth -= 1;

	//	chip = rng->Uniform(INTT::CHIP);
	//	if(chip == INTT::CHIP)chip -= 1;

	//	channel = rng->Uniform(INTT::CHANNEL);
	//	if(channel == INTT::CHANNEL)channel -= 1;

	//	adc = rng->Uniform(INTT::ADC);
	//	if(adc == INTT::ADC)adc -= 1;

	//	//Set temp to be the unscaled z position of the chip, to introduce eta dependence for the dummy method
	//	//currently not used; hitrates are flat
	//	temp = (2.0 * northsouth - 1.0) * ( (INTT::CHIP / 2) - chip % (INTT::CHIP / 2) - 0.5 );

	//	//get the bin these indexes correspond to
	//	//INTT::FindBin returns 1 if there is an error; extra guard statement for typos
	//	if(INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel))continue;
	//	HitMap->AddBinContent(bin);
	//	if(INTT::ADCN::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel, adc))continue;
	//	ADCMap->AddBinContent(bin);

	//	std::string northsouth_str = "North";
	//	if(northsouth)northsouth_str = "South";

	//	printf("Layer:%2d\tLadder:%3d (%s)\tChip:%3d\tChannel:%4d\n", layer + INTT::LAYER_OFFSET, ladder, northsouth_str.c_str(), chip + INTT::CHIP_OFFSET, channel);
	//	//...

	//	bin = temp;//dummy line so it will compile; temp is "used"
	//}

	NumEvents->AddBinContent(1);

	DBVarUpdate();

	return 0;
}

int InttMon::Reset()
{
	//reset our DBVars
	evtcnt = 0;
	
	//clear our histogram entries
	NumEvents->Reset();
	HitMap->Reset();

	return 0;
}
//===	~Public Methods		===//

//===	Private Methods		===//
int InttMon::DBVarInit()
{
	std::string var_name;

	var_name = "intt_evtcnt";
	dbvars->registerVar(var_name);

	dbvars->DBInit();

	return 0;
}

int InttMon::DBVarUpdate()
{
	dbvars->SetVar("intt_evtcnt", (float)evtcnt, 0.1 * evtcnt, (float)evtcnt);

	return 0;
}
//===	~Private Methods		===//

// for testing/debugging
int InttMon::MiscDebug()
{
	int b;
	int c;

	INTT::Indexes_s indexes;
	INTT::Indexes_s jndexes;

	while(true)
	{
		INTT::GetBinFromIndexes(b, indexes);
		INTT::GetIndexesFromBin(b, jndexes);
		INTT::GetBinFromIndexes(c, jndexes);

		if(b != c)
		{
			std::cout << "Broke for round trip:" << std::endl;
			std::cout << "\tindexes:" << std::endl;
			std::cout << "\t\tadc: " << indexes.adc << " -> " << jndexes.adc << std::endl;
			std::cout << "\t\tchn: " << indexes.chn << " -> " << jndexes.chn << std::endl;
			std::cout << "\t\tchp: " << indexes.chp << " -> " << jndexes.chp << std::endl;
			std::cout << "\t\tarm: " << indexes.arm << " -> " << jndexes.arm << std::endl;
			std::cout << "\t\tldr: " << indexes.ldr << " -> " << jndexes.ldr << std::endl;
			std::cout << "\t\tlyr: " << indexes.lyr << " -> " << jndexes.lyr << std::endl;
			std::cout << "\tbin: " << b << " -> " << c << std::endl;

			break;
		}

		++indexes.adc;
		if(indexes.adc < INTT::ADC)continue;
		indexes.adc = 0;
		
		++indexes.chn;
		if(indexes.chn < INTT::CHANNEL)continue;
		indexes.chn = 0;

		++indexes.chp;
		if(indexes.chp < INTT::CHIP)continue;
		indexes.chp = 0;

		++indexes.arm;
		if(indexes.arm < INTT::ARM)continue;
		indexes.arm = 0;

		++indexes.ldr;
		if(indexes.ldr < INTT::LADDER[indexes.lyr])continue;
		indexes.ldr = 0;

		++indexes.lyr;
		if(indexes.lyr < INTT::LAYER)continue;
		indexes.lyr = 0;

		break;
	}

	std::cout << "Round trip worked" << std::endl;

	return 0;
}
