#include "InttExecs.h"

void HitMapExec(int layer)
{
	layer -= LAYER_OFFSET;

	if(gPad->GetEvent() != 11)return; //left click

	int binx = gPad->AbsPixeltoX(gPad->GetEventX()) * (2 * LADDER[layer]); //normalized click position times number of x axis bins
	int biny = gPad->AbsPixeltoY(gPad->GetEventY()) * (CHIP); //normalized click position times number of y axis bins

	if(binx < 0)binx = 0;
	if(biny < 0)biny = 0;

	if(binx >= 2 * LADDER[layer])binx = 2 * LADDER[layer] - 1;
	if(biny >= CHIP)biny = CHIP - 1;


	int ladder = binx / 2;
	int northsouth = 1 - biny / (CHIP / 2);
	int chip;

	if(northsouth == 0) //North
	{
		chip = CHIP / 2 - biny % (CHIP / 2) + (1 - binx % 2) * (CHIP / 2) - 1;
	}
	if(northsouth == 1) //South
	{
		chip = biny % (CHIP / 2) + (binx % 2) * (CHIP / 2);
	}

	std::cout << "Layer: " << layer + LAYER_OFFSET << "\tLadder: " << ladder << "\tNorthSouth: " << northsouth << "\tChip: " << chip + CHIP_OFFSET << std::endl;
}
