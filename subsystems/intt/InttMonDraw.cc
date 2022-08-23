#include "InttMon.h"
#include "InttMonDraw.h"

void DrawHitMap()
{
	if(!HitMapStyle)
	{
		HitMapStyle = new TStyle
		(
			Form("INTTHitMapStyle"),
			Form("INTTHitMapStyle")
		);

		HitMapStyle->SetOptStat(0);
	}

	HitMapStyle->cd();

	if(!HitMapCanvas)
	{
		HitMapCanvas = new TCanvas
		(
			Form("InttHitMap"),
			Form("InttHitMap"),
			1200,
			1800
		);

		HitMapCanvas->Draw();
		HitMapCanvas->AddExec("ShowChipHitMap","HitMapExec");
	}

	for(int layer = 0; layer < LAYER; layer++)
	{
		HitMapCanvas->cd();
		PrepHitMap(layer + LAYER_OFFSET);
		DrawINTTLayer(HitMapCanvas, layer + LAYER_OFFSET, 0, HitMapDrawHist);
	}

	HitMapCanvas->Update();
}

void PrepHitMap(int layer)
{
	layer -= LAYER_OFFSET;

	if(!(0 <= layer and layer < LAYER))return;

	HitMapDrawHist = GetHitMap(layer + LAYER_OFFSET);

	HitMapDrawHist->GetXaxis()->SetNdivisions(1, true);
	HitMapDrawHist->GetYaxis()->SetNdivisions(1, true);

	HitMapDrawHist->GetXaxis()->SetLabelSize(0.0);
	HitMapDrawHist->GetYaxis()->SetLabelSize(0.0);

	HitMapDrawHist->GetXaxis()->SetTickLength(0.0);
	HitMapDrawHist->GetYaxis()->SetTickLength(0.0);

	HitMapDrawHist->SetMinimum(-1);

	//do stuff with HitMapDrawHist
}

void HitMapExec()
{
	if(!gPad)return;
	//if(gPad->GetEvent() != 11)return;

	int x = gPad->GetEventX();
	int y = gPad->GetEventY();

	cout << x << y << endl;
}
