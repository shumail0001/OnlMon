#include "InttMon.h"

//public methods

int InttMon::Init()
{
  OnlMonServer* se = OnlMonServer::instance();

  NumEvents = new TH1D(
      Form("NumEvents"),
      Form("NumEvents"),
      1,
      0.5,
      1.5);
  se->registerHisto(this, NumEvents);

  //HitMap chip level histograms
  HitMap = new TH2D*[LAYER];
  HitRateMap = new TH2D*[LAYER];
  for (int layer = 0; layer < LAYER; layer++)
  {
    HitMap[layer] = new TH2D(
        Form("Layer%d_HitMap", layer + LAYER_OFFSET),
        Form("Layer%d_HitMap", layer + LAYER_OFFSET),
        2 * LADDER[layer],
        0.0,
        LADDER[layer],
        CHIP,
        0.0,
        CHIP);
    se->registerHisto(this, HitMap[layer]);

    HitRateMap[layer] = new TH2D(
        Form("Layer%d_HitRateMap", layer + LAYER_OFFSET),
        Form("Layer%d_HitRateMap", layer + LAYER_OFFSET),
        2 * LADDER[layer],
        0.0,
        LADDER[layer],
        CHIP,
        0.0,
        CHIP);
    se->registerHisto(this, HitRateMap[layer]);
  }

  //HitMap channel level histograms
  ChipHitMap = new TH1D****[LAYER];
  for (int layer = 0; layer < LAYER; layer++)
  {
    ChipHitMap[layer] = new TH1D***[LADDER[layer]];
    for (int ladder = 0; ladder < LADDER[layer]; ladder++)
    {
      ChipHitMap[layer][ladder] = new TH1D**[NORTHSOUTH];
      for (int northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
      {
        ChipHitMap[layer][ladder][northsouth] = new TH1D*[CHIP];
        for (int chip = 0; chip < CHIP; chip++)
        {
          ChipHitMap[layer][ladder][northsouth][chip] = new TH1D(
              Form("Layer%d_Ladder%d_NorthSouth%d_Chip%d", layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET),
              Form("Layer%d_Ladder%d_NorthSouth%d_Chip%d", layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET),
              CHANNEL,
              0.0,
              CHANNEL);
          se->registerHisto(this, ChipHitMap[layer][ladder][northsouth][chip]);
        }
      }
    }
  }

  //ADC chip level histograms
  //ADC channel level histograms

  //...

  dbvars = new OnlMonDB(Name());
  DBVarInit();
  Reset();

  //Read in calibrartion data from InttMonData.dat
  std::string fullfile = std::string(getenv("INTTCALIB")) + "/" + "InttMonData.dat";
  std::ifstream calib(fullfile);
  //probably need to do stuff here (maybe write to expectation maps)
  //or reimplment in BeginRun()
  calib.close();

  // for testing/debugging without unpacker, remove later
  InitExpectationHists();
  rng = new TRandom(1234);
  //~for testing/debugging without unpacker, remove later

  return 0;
}

InttMon::~InttMon()
{
  //delete NumEvents (replace NumEvents with a DBVar most likely)
  delete NumEvents;

  //delete HitMap chip level histograms
  for (int layer = 0; layer < LAYER; layer++)
  {
    delete HitMap[layer];
    delete HitRateMap[layer];
    //delete ADC[]
    //...
  }
  delete[] HitMap;
  delete[] HitRateMap;
  //delete[] ADC
  //...

  //delete channel level histograms
  for (int layer = 0; layer < LAYER; layer++)
  {
    for (int ladder = 0; ladder < LADDER[layer]; ladder++)
    {
      for (int northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
      {
        for (int chip = 0; chip < CHIP; chip++)
        {
          delete ChipHitMap[layer][ladder][northsouth][chip];
          //delete ADC[]...
          //...
        }
        delete[] ChipHitMap[layer][ladder][northsouth];
        //delete[] ADC[]...
        //...
      }
      delete[] ChipHitMap[layer][ladder];
      //delete[] ADC[]...
      //...
    }
    delete[] ChipHitMap[layer];
    //delete[] ADC[]
    //..
  }
  delete[] ChipHitMap;
  //delete ADC
  //...

  //delete ADC chip level histograms
  //delete ADC channel level histograms

  //delete ...

  delete dbvars;
}

int InttMon::BeginRun(const int /* run_num */)
{
  //per-run calibrations; don't think we need to do anything here yet

  return 0;
}

int InttMon::process_event(Event* /* evt */)
{
  //dummy method since unpacker is not done yet
  double temp;

  int bin;

  int layer;
  int ladder;
  int northsouth;
  int chip;
  int channel;

  //int adc;
  //...

  int hits = rng->Poisson(HITS_PER_EVENT);
  for (int hit = 0; hit < hits; hit++)
  {
    //randomly choose a chip/channel for the hit to occur
    //including guards in case upper bound of TRandom::Uniform(Double_t) is inclusive
    layer = rng->Uniform(LAYER);
    if (layer == LAYER) layer -= 1;

    ladder = rng->Uniform(LADDER[layer]);
    if (ladder == LADDER[layer]) ladder -= 1;

    northsouth = rng->Uniform(NORTHSOUTH);
    if (northsouth == NORTHSOUTH) northsouth -= 1;

    chip = rng->Uniform(CHIP);
    if (chip == CHIP) chip -= 1;

    channel = rng->Uniform(CHANNEL);
    if (channel == CHANNEL) channel -= 1;

    temp = (2.0 * northsouth - 1.0) * ((CHIP / 2) - chip % (CHIP / 2) - 0.5);

    //get the bin these indexes correspond to
    if (GetBin(bin, layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET)) continue;  //if GetBin returns nonzero there was an error

    //HitMap chip level stuff
    HitMap[layer]->SetBinContent(bin, HitMap[layer]->GetBinContent(bin) + 1);

    //HitMap channel level stuff
    ChipHitMap[layer][ladder][northsouth][chip]->SetBinContent(channel + 1, ChipHitMap[layer][ladder][northsouth][chip]->GetBinContent(bin) + 1);

    //ADC chip level stuff
    //ADC channel level stuff
    //...

    bin = temp;  //dummy line so it will compile; temp is "used"
  }

  NumEvents->SetBinContent(1, NumEvents->GetBinContent(1) + 1);

  return 0;
}

int InttMon::Reset()
{
  //reset our DBVars

  //clear our histogram entries
  int bin;

  int layer;
  int ladder;
  int northsouth;
  int chip;

  //NumEvents
  for (bin = 1; bin <= NumEvents->GetNcells(); bin++)
  {
    NumEvents->SetBinContent(bin, 0);
  }

  //chip level clearing
  for (layer = 0; layer < LAYER; layer++)
  {
    for (bin = 1; bin <= HitMap[layer]->GetNcells(); bin++)
    {
      //HitMap
      HitMap[layer]->SetBinContent(bin, 0);

      //ADC
      //...
    }
  }

  //channel level clearing
  for (layer = 0; layer < LAYER; layer++)
  {
    for (ladder = 0; ladder < LADDER[layer]; ladder++)
    {
      for (northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
      {
        for (chip = 0; chip < CHIP; chip++)
        {
          //ChipHitMap
          for (bin = 1; bin <= ChipHitMap[layer][ladder][northsouth][chip]->GetNcells(); bin++)
          {
            ChipHitMap[layer][ladder][northsouth][chip]->SetBinContent(bin, 0);
          }

          //ChipADC
          //...
        }
      }
    }
  }

  return 0;
}

//protected methods
int InttMon::DBVarInit()
{
  std::string var_name;

  var_name = "intt_mon_dum1";
  dbvars->registerVar(var_name);
  var_name = "intt_mon_dum2";
  dbvars->registerVar(var_name);

  dbvars->DBInit();

  return 0;
}

bool InttMon::CheckIndexes(int layer, int ladder, int northsouth, int chip)
{
  layer -= LAYER_OFFSET;
  chip -= CHIP_OFFSET;

  if (!(0 <= layer && layer < LAYER))
  {
    std::cout << "Bad LAYER index" << std::endl;

    return false;
  }
  if (!(0 <= ladder && ladder < LADDER[layer]))
  {
    std::cout << "Bad LADDER index" << std::endl;

    return false;
  }
  if (!(0 <= northsouth && northsouth < NORTHSOUTH))
  {
    std::cout << "Bad NORTHSOUTH index" << std::endl;

    return false;
  }
  if (!(0 <= chip && chip < CHIP))
  {
    std::cout << "Bad CHIP index" << std::endl;

    return false;
  }

  return true;
}

int InttMon::GetBin(int& bin, int layer, int ladder, int northsouth, int chip)
{
  if (!CheckIndexes(layer, ladder, northsouth, chip)) return 1;

  layer -= LAYER_OFFSET;
  chip -= CHIP_OFFSET;

  if (northsouth == 0)  //North
  {
    bin = HitMap[layer]->GetBin(2 * ladder + 2 - chip / (CHIP / 2), CHIP - chip % (CHIP / 2));
  }
  if (northsouth == 1)  //South
  {
    bin = HitMap[layer]->GetBin(2 * ladder + 1 + chip / (CHIP / 2), chip % (CHIP / 2) + 1);
  }

  return 0;
}

int InttMon::GetLadderNorthSouthChip(int bin, int layer, int& ladder, int& northsouth, int& chip)
{
  layer -= LAYER_OFFSET;

  if (!(0 <= layer && layer < LAYER)) return 1;
  if (!HitMap[layer]) return 1;

  int binx;
  int biny;
  int binz;

  HitMap[layer]->GetBinXYZ(bin, binx, biny, binz);
  binx -= 1;
  biny -= 1;

  ladder = binx / 2;
  northsouth = biny / (CHIP / 2);

  if (northsouth == 0)  //North
  {
    chip = CHIP / 2 - biny % (CHIP / 2) + (1 - binx % 2) * (CHIP / 2) - 1;
  }
  if (northsouth == 1)  //South
  {
    chip = biny % (CHIP / 2) + (binx % 2) * (CHIP / 2);
  }

  return 0;
}

// for testing/debugging without unpacker, remove later
int InttMon::MiscDebug()
{
  //write methods to debug/test here

  return 0;
}

int InttMon::InitExpectationHists()
{
  double temp;

  int bin;

  int layer;
  int ladder;
  int northsouth;
  int chip;

  for (layer = 0; layer < LAYER; layer++)
  {
    for (ladder = 0; ladder < LADDER[layer]; ladder++)
    {
      for (northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
      {
        for (chip = 0; chip < CHIP; chip++)
        {
          //set temp to be z coordinate of chip (in units of number of chips from z = 0)
          temp = (2.0 * northsouth - 1.0) * ((CHIP / 2) - chip % (CHIP / 2) - 0.5);

          GetBin(bin, layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET);

          //Expectation histograms for HitMap at chip level
          HitRateMap[layer]->SetBinContent(bin, HITS_PER_EVENT / (52 * 56));

          //Expectation histograms for ADC at chip level

          //...
          bin = temp;  //dummy line so it will compile; temp is "used"
        }
      }
    }
  }

  return 0;
}
//~for testing/debugging without unpacker, remove later
