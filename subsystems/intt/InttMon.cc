#include "InttMon.h"

void InitPtrs()
{
	NumEvents = new TH1D
	(
		Form("NumEvents"),
		Form("NumEvents"),
		1,
		0.5,
		1.5
	);
	
	HitMap = new TH2D*[LAYER];
	HitRateMap = new TH2D*[LAYER];
	for(int layer = 0; layer < LAYER; layer++)
	{
		HitMap[layer] = new TH2D
		(
			Form("Layer%d_HitMap", layer + LAYER_OFFSET),
			Form("Layer%d_HitMap", layer + LAYER_OFFSET),
			2 * LADDER[layer],
			0.0,
			LADDER[layer],
			CHIP,
			0.0,
			CHIP
		);
		
		HitRateMap[layer] = new TH2D
		(
			Form("Layer%d_HitRateMap", layer + LAYER_OFFSET),
			Form("Layer%d_HitRateMap", layer + LAYER_OFFSET),
			2 * LADDER[layer],
			0.0,
			LADDER[layer],
			CHIP,
			0.0,
			CHIP
		);
	}

	ChipHists = new TH1D****[LAYER];
	for(int layer = 0; layer < LAYER; layer++)
	{
		ChipHists[layer] = new TH1D***[LADDER[layer]];
		for(int ladder = 0; ladder < LADDER[layer]; ladder++)
		{
			ChipHists[layer][ladder] = new TH1D**[NORTHSOUTH];
			for(int northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
			{
				ChipHists[layer][ladder][northsouth] = new TH1D*[CHIP];
				for(int chip = 0; chip < CHIP; chip++)
				{
					ChipHists[layer][ladder][northsouth][chip] = new TH1D
					(
						Form("Layer%d_Ladder%d_NorthSouth%d_U%d", layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET),
						Form("Layer%d_Ladder%d_NorthSouth%d_U%d", layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET),
						CHANNEL,
						0.0,
						CHANNEL
					);
				}
			}
		}
	}
}

void DeletePtrs()
{
	delete NumEvents;

	for(int layer = 0; layer < LAYER; layer++)
	{
		delete HitMap[layer];
		delete HitRateMap[layer];
	}
	delete HitMap;
	delete HitRateMap;

	for(int layer = 0; layer < LAYER; layer++)
	{
		for(int ladder = 0; ladder < LADDER[layer]; ladder++)
		{
			for(int northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
			{
				for(int chip = 0; chip < CHIP; chip++)
				{
					delete ChipHists[layer][ladder][northsouth][chip];
				}
				delete[] ChipHists[layer][ladder][northsouth];
			}
			delete[] ChipHists[layer][ladder];
		}
		delete ChipHists[layer];
	}
	delete[] ChipHists;
}

void FillHists(int N, int seed)
{
	double temp;

	int bin;

	int layer;
	int ladder;
	int northsouth;
	int chip;

	TF1* uniform = new TF1("uniform", "1.0", -1.0, CHANNEL + 1.0);

	TRandom* rng = new TRandom();
	rng->SetSeed(seed);

	NumEvents->SetBinContent(1, N);

	for(layer = 0; layer < LAYER; layer++)
	{
		for(ladder = 0; ladder < LADDER[layer]; ladder++)
		{
			for(northsouth = 0; northsouth < NORTHSOUTH; northsouth++)
			{
				for(chip = 0; chip < CHIP; chip++)
				{
					temp = (2.0 * northsouth - 1.0) * ( (CHIP / 2) - chip % (CHIP / 2) - 0.5 );

					GetBin(bin, layer + LAYER_OFFSET, ladder, northsouth, chip + CHIP_OFFSET);

					HitRateMap[layer]->SetBinContent(bin, TMath::Gaus(temp, 0.0, CHIP / 8.0));
					HitMap[layer]->SetBinContent(bin, rng->Poisson(HitRateMap[layer]->GetBinContent(bin) * N));
					ChipHists[layer][ladder][northsouth][chip]->FillRandom("uniform", HitMap[layer]->GetBinContent(bin));
				}
			}
		}
	}

	delete rng;
}

double GetNumEvents()
{
	if(NumEvents)return NumEvents->Integral();

	return 0.0;
}

TH2D* GetHitMap(int layer)
{
	layer -= LAYER_OFFSET;

	if(!(0 <= layer and layer < LAYER))return 0x0;

	return HitMap[layer];
}

TH2D* GetHitRateMap(int layer)
{
	layer -= LAYER_OFFSET;

	if(!(0 <= layer and layer < LAYER))return 0x0;

	return HitRateMap[layer];
}

TH1D* GetChipHitMap(int bin, int layer)
{
	int ladder = -1;
	int northsouth = -1;
	int chip = -1;

	GetLadderNorthSouthChip(bin, layer, ladder, northsouth, chip);

	if(!CheckIndexes(layer, ladder, northsouth, chip))return 0x0;

	return ChipHists[layer][ladder][northsouth][chip];
}

bool CheckIndexes(int layer, int ladder, int northsouth, int chip)
{
	layer -= LAYER_OFFSET;
	chip -= CHIP_OFFSET;
	
	if(!(0 <= layer and layer < LAYER))
	{
		cout << "Bad LAYER index" << endl;
	
		return false;
	}
	if(!(0 <= ladder and ladder < LADDER[layer]))
	{
		cout << "Bad LADDER index" << endl;
	
		return false;
	}
	if(!(0 <= northsouth and northsouth < NORTHSOUTH))
	{
		cout << "Bad NORTHSOUTH index" << endl;
	
		return false;
	}
	if(!(0 <= chip and chip < CHIP))
	{
		cout << "Bad CHIP index" << endl;
	
		return false;
	}

	return true;
}

void GetBin(int& bin, int layer, int ladder, int northsouth, int chip)
{
	if(!CheckIndexes(layer, ladder, northsouth, chip))return;

	layer -= LAYER_OFFSET;
	chip -= CHIP_OFFSET;

	if(northsouth == 0)//South
	{
		bin = HitMap[layer]->GetBin(2 * ladder + 1 + chip / (CHIP / 2), chip % (CHIP / 2) + 1);
	}
	if(northsouth == 1)//North
	{
		bin = HitMap[layer]->GetBin(2 * ladder + 2 - chip / (CHIP / 2), CHIP - chip % (CHIP / 2));
	}
}

void GetLadderNorthSouthChip(int bin, int layer, int& ladder, int& northsouth, int& chip)
{
	layer -= LAYER_OFFSET;

	if(!(0 <= layer and layer < LAYER))return;
	if(!HitMap[layer])return;

	int binx;
	int biny;
	int binz;

	HitMap[layer]->GetBinXYZ(bin, binx, biny, binz);
	binx -= 1;
	biny -= 1;

	ladder = binx / 2;
	northsouth = biny / (CHIP / 2);

	if(northsouth == 0)//South
	{
		chip = biny % (CHIP / 2) + (binx % 2) * (CHIP / 2);
	}
	if(northsouth == 1)//North
	{
		chip = CHIP / 2 - biny % (CHIP / 2) + (1 - binx % 2) * (CHIP / 2) - 1;
	}
}

