#include "InttMonDraw.h"

InttMonDraw::OPTIONS_TYPE InttMonDraw::OPTIONS =
{
	{"hitmap",	std::pair<MAIN_FUNC, EXEC_FUNC>{&InttMonDraw::GetLayerHitMap,	&InttMonDraw::GetChipHitMap}},
	{"hitmapz",	std::pair<MAIN_FUNC, EXEC_FUNC>{&InttMonDraw::GetLayerHitMapZ,	&InttMonDraw::GetChipHitMapZ}}
};

InttMonDraw::InttMonDraw(const std::string &name) : OnlMonDraw(name)
{
	//keeping it fairly empty
	//dbvars = new OnlMonDB(ThisName);

	return;
}

InttMonDraw::~InttMonDraw()
{
	return;
}

//===		Inherited Functions		===//
int InttMonDraw::Init()
{
	//===	~Initialize Drawing Histograms	===//

/*
	//===	Register OnlMon Histograms	===//
	OnlMonClient* omc = OnlMonClient::instance();

//	omc->AddServerHost("localhost");
	omc->requestHistoBySubSystem("INTTMON", 1);

	omc->registerHisto("InttHitMap", "INTTMON");
	omc->registerHisto("InttHitMapRef", "INTTMON");
	//...
	//===	~Register OnlMon Histograms	===//
*/

	return 0;
}

int InttMonDraw::Draw(const std::string &what)
{
	bool found = false;

	for(OPTIONS_TYPE::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
	{
		//to lower
		if(what == "ALL" || itr->first == what)
		{
			found = true;
			DrawOption(itr->first);
		}
	}

	if(!found)
	{
		std::cout << "Option \"" << what << "\" not found" << std::endl;
		std::cout << "Try \"ALL\" or one of the following (case insensitive)" << std::endl;
		for(OPTIONS_TYPE::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
		{
			std::cout << "\t" << itr->first << std::endl;
		}
	}

	return 0;
}

int InttMonDraw::MakePS(const std::string &what)
{
	OnlMonClient* omc = OnlMonClient::instance();
	if(Draw(what))return 1;

	TCanvas* canvas = nullptr;
	std::ostringstream filename;
	for(OPTIONS_TYPE::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
	{
		filename.str("");
		if(what == "ALL" or what == itr->first)
		{
			filename << ThisName << "_" <<  itr->first << "_" << omc->RunNumber() << ".ps";
			canvas = (TCanvas*)gROOT->FindObject(Form("INTT_%s_Canvas", itr->first.c_str()));
			if(canvas)canvas->Print(filename.str().c_str());
			if(canvas)omc->CanvasToPng(canvas, omc->htmlRegisterPage(*this, itr->first, itr->first, "png"));
			//needs attention
		}
	}

	return 0;
}

int InttMonDraw::MakeHtml(const std::string &what)
{
	OnlMonClient* omc = OnlMonClient::instance();
	if(Draw(what))return 1;

	TCanvas* canvas = nullptr;
	for(OPTIONS_TYPE::iterator itr = OPTIONS.begin(); itr != OPTIONS.end(); ++itr)
	{
		if(what == "ALL" or what == itr->first)
		{
			canvas = (TCanvas*)gROOT->FindObject(Form("INTT_%s_Canvas", itr->first.c_str()));
			if(canvas)omc->CanvasToPng(canvas, omc->htmlRegisterPage(*this, itr->first, itr->first, "png"));
		}
	}

	return 0;
}
//===		~Inherited Functions		===//

//===		Drawing Methods			===//
void InttMonDraw::DrawPad(TPad* pad)
{
	pad->SetFillStyle(4000); //transparent
	pad->Range(0.0, 0.0, 1.0, 1.0);
	pad->SetTopMargin(T_MARGIN);
	pad->SetBottomMargin(B_MARGIN);
	pad->SetLeftMargin(L_MARGIN);
	pad->SetRightMargin(R_MARGIN);

	pad->Draw();
}

void InttMonDraw::DrawExecPad(TPad* pad)
{
	pad->SetFillStyle(4000); //transparent
	pad->Range(0.0, 0.0, 1.0, 1.0);
	pad->SetTopMargin(EXEC_T_MARGIN);
	pad->SetBottomMargin(EXEC_B_MARGIN);
	pad->SetLeftMargin(EXEC_L_MARGIN);
	pad->SetRightMargin(EXEC_R_MARGIN);

	pad->Draw();
}

void InttMonDraw::DrawDisp(TPad* base_pad, std::string option)
{
	if(OPTIONS.find(option) == OPTIONS.end())return;

	if(!base_pad)return;

	TPad* disp_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_DispPad", option.c_str()));
	if(!disp_pad)
	{
		base_pad->cd();
		base_pad->Update();
		disp_pad = new TPad
		(
			Form("INTT_%s_DispPad", option.c_str()),
			Form("INTT_%s_DispPad", option.c_str()),
			0.0,
			0.0,
			1.0,
			DISP_FRAC
		);
		DrawPad(disp_pad);
	}

	TText* disp_text = (TText*)gROOT->FindObject(Form("INTT_%s_DispText", option.c_str()));
	if(!disp_text)
	{
		disp_pad->cd();
		disp_pad->Update();
		disp_text = new TText
		(
			0.5,
			0.5,
			Form("Layer: %2d\tLadder: %2d\t(%s)\tChip: %2d", INTT::LAYER_OFFSET, 0, "North", INTT::CHIP_OFFSET)
		);
		disp_text->SetName(Form("INTT_%s_DispText", option.c_str()));
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(DISP_TEXT_SIZE);

		disp_text->Draw();
	}
}

//---		Main Drawing Methods		---//
void InttMonDraw::DrawOption(std::string option)
{
	//if(!(0 <= opt and opt < OPT))return;
	if(OPTIONS.find(option) == OPTIONS.end())return;

	//TStyle
	TStyle* style = (TStyle*)gROOT->FindObject(Form("INTT_%s_Style", option.c_str()));
	if(!style)
	{
		style = new TStyle(Form("INTT_%s_Style", option.c_str()), Form("INTT_%s_Style", option.c_str()));
		style->SetOptStat(0);
		style->SetOptTitle(0);
		//...
	}
	style->cd();
	//style->Update();

	//TCanvas
	TCanvas* canvas = (TCanvas*)gROOT->FindObject(Form("INTT_%s_Canvas", option.c_str()));
	if(!canvas)
	{
		canvas = new TCanvas
		(
			Form("INTT_%s_Canvas", option.c_str()),
			Form("INTT_%s_Canvas", option.c_str()),
			0,//-1, //no menu bar
			0,
			CNVS_WIDTH,
			CNVS_HEIGHT
		);
		//canvas->...
		//...
	}

	DrawDisp(canvas, option);

	TPad* base_pad;
	for(int layer = 0; layer < INTT::LAYER; ++layer)
	{
		base_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_BasePad", option.c_str(), layer + INTT::LAYER_OFFSET));
		if(!base_pad)
		{
			canvas->cd();
			canvas->Update();
			base_pad = new TPad
			(
				Form("INTT_%s_Layer%d_BasePad", option.c_str(), layer + INTT::LAYER_OFFSET),
				Form("INTT_%s_Layer%d_BasePad", option.c_str(), layer + INTT::LAYER_OFFSET),
				0.0,
				(INTT::LAYER - layer - 1.0) / INTT::LAYER * (1.0 - DISP_FRAC) + DISP_FRAC,
				1.0,
				(INTT::LAYER - layer + 0.0) / INTT::LAYER * (1.0 - DISP_FRAC) + DISP_FRAC
			);
			DrawPad(base_pad);
		}

		base_pad->cd();
		base_pad->Update();
		DrawTitle	(base_pad, option, layer + INTT::LAYER_OFFSET);
		DrawKey		(base_pad, option, layer + INTT::LAYER_OFFSET);
		DrawHist	(base_pad, option, layer + INTT::LAYER_OFFSET);
		DrawGrid	(base_pad, option, layer + INTT::LAYER_OFFSET);
		DrawLabels	(base_pad, option, layer + INTT::LAYER_OFFSET);
		DrawExec	(base_pad, option, layer + INTT::LAYER_OFFSET);
	}

	//Show the canvas
	canvas->Update();
	canvas->Show();
	canvas->SetEditable(0);
}

void InttMonDraw::DrawTitle(TPad* base_pad, std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;

	if(!base_pad)return;

	TPad* title_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_TitlePad", option.c_str(), layer + INTT::LAYER_OFFSET));
	if(!title_pad)
	{
		base_pad->cd();
		base_pad->Update();
		title_pad = new TPad
		(
			Form("INTT_%s_Layer%d_TitlePad", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("INTT_%s_Layer%d_TitlePad", option.c_str(), layer + INTT::LAYER_OFFSET),
			KEY_FRAC,
			1.0 - TITLE_FRAC,
			1.0,
			1.0
		);
		DrawPad(title_pad);

		title_pad->cd();
		title_pad->Update();
		TText title_text
		(
			(L_MARGIN + 1.0 - R_MARGIN) / 2.0,
			0.5,
			Form("Layer %d", layer + INTT::LAYER_OFFSET)
		);
		title_text.SetTextAlign(22);
		title_text.SetTextSize(TITLE_TEXT_SIZE);
		title_text.DrawClone();
	}
}

void InttMonDraw::DrawKey(TPad* base_pad, std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;

	if(!base_pad)return;

	TPad* key_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_KeyPad", option.c_str(), layer + INTT::LAYER_OFFSET));
	if(!key_pad)
	{
		base_pad->cd();
		base_pad->Update();
		key_pad = new TPad
		(
			Form("INTT_%s_Layer%d_KeyPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("INTT_%s_Layer%d_KeyPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			0.0,
			LABEL_FRAC,
			KEY_FRAC,
			1.0 - TITLE_FRAC
		);
		DrawPad(key_pad);
		key_pad->cd();
		key_pad->Update();

		//want to draw it to scale with the histogram
		//the hist pad is drawn on (1.0 - KEY_FRAC) of the layer pad
		//the hist pad itself has margins L_MARGIN, R_MARGIN of the hist pad on either side
		//this is further divided into INTT::LADDER[layer] sections
		//thus a hist ladder is (1.0 - KEY_FRAC) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[layer] of the layer pad wide
		
		//this should be as wide
		//the key is drawn on KEY_FRAC of the layer pad
		//so KEY_FRAC * width = (1.0 - KEY_FRAC) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[layer]
	
		//there are at least 12 ladders, these lines will never run
		//in principle, it could be a problem so I am being pedantic
	
		//shift rightward slightly by half the distance to the edge of pad

		int i;
	
		double x;
		double y;
	
		double xmin = 0.5 - (1.0 - KEY_FRAC) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[layer] / KEY_FRAC / 2;
		double xmax = 0.5 + (1.0 - KEY_FRAC) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[layer] / KEY_FRAC / 2;
	
		if(xmin < 0.0)xmin = 0.1;
		if(xmax > 1.0)xmax = 0.9;
	
		xmax += xmin / 2.0;
		xmin += xmin / 2.0;

		TLine key_line;
		TText key_text;

		//Draw lines
		for(i = 0; i < 3; ++i)
		{
			x = xmin + i * (xmax - xmin) / 2;

			key_line.SetX1(x);
			key_line.SetY1(B_MARGIN);
			key_line.SetX2(x);
			key_line.SetY2(1.0 - T_MARGIN);
			key_line.SetLineStyle((i % 2) ? 3 : 1);
			key_line.SetLineWidth((i % 2) ? 1 : 2);
			key_line.DrawClone();
		}
	
		for(i = 0; i < INTT::CHIP + 1; ++i)
		{
			y = B_MARGIN + i * (1.0 - T_MARGIN - B_MARGIN) / INTT::CHIP;

			key_line.SetX1(xmin);
			key_line.SetY1(y);
			key_line.SetX2(xmax);
			key_line.SetY2(y);
			key_line.SetLineStyle((i % (INTT::CHIP / 2) ? 3 : 1));
			key_line.SetLineWidth((i % (INTT::CHIP / 2) ? 1 : 2));
			key_line.DrawClone();
		}

		//Text
		key_text.SetTextAlign(22);
		key_text.SetTextSize(KEY_TEXT_SIZE2);

		for(i = 0; i < INTT::CHIP; ++i)
		{
			x = ( (3 - 2 * (i / (INTT::CHIP / 2))) * xmax + (1 + 2 * (i / (INTT::CHIP / 2))) * xmin ) / 4.0;
			y = (1.0 - T_MARGIN + B_MARGIN) / 2.0 + (INTT::CHIP / 2 - i % (INTT::CHIP / 2) - 0.5) * (0.5 - T_MARGIN / 2.0 - B_MARGIN / 2.0) / (INTT::CHIP / 2);

			key_text.SetX(x);
			key_text.SetY(y);
			key_text.SetTitle(Form("%d", i + INTT::CHIP_OFFSET));
			key_text.DrawClone();
		}
	
		for(i = 0; i < INTT::CHIP; ++i)
		{
			x = ( (1 + 2 * (i / (INTT::CHIP / 2))) * xmax + (3 - 2 * (i / (INTT::CHIP / 2))) * xmin ) / 4.0;
			y = B_MARGIN + (i % (INTT::CHIP / 2) + 0.5) * (0.5 - T_MARGIN / 2.0 - B_MARGIN / 2.0) / (INTT::CHIP / 2);

			key_text.SetX(x);
			key_text.SetY(y);
			key_text.SetTitle(Form("%d", i + INTT::CHIP_OFFSET));
			key_text.DrawClone();
		}

		key_text.SetTextSize(KEY_TEXT_SIZE1);
		key_text.SetTextAngle(90);
		key_text.SetX(xmin / 2.0);

		key_text.SetY(0.75);
		key_text.SetTitle("North");
		key_text.DrawClone();

		key_text.SetY(0.25);
		key_text.SetTitle("South");
		key_text.DrawClone();
	}
}

void InttMonDraw::DrawHist(TPad* base_pad, std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;

	if(!base_pad)return;

	TPad* hist_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_HistPad", option.c_str(), layer + INTT::LAYER_OFFSET));
	if(!hist_pad)
	{
		base_pad->cd();
		base_pad->Update();
		hist_pad = new TPad
		(
			Form("INTT_%s_Layer%d_HistPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("INTT_%s_Layer%d_HistPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			KEY_FRAC,
			LABEL_FRAC,
			1.0,
			1.0 - TITLE_FRAC);
		DrawPad(hist_pad);
	}
	hist_pad->cd();
	hist_pad->Update();

	TH1* hist = (*(OPTIONS.find(option)->second.first))(layer + INTT::LAYER_OFFSET);
	if(hist)hist->DrawCopy("COLZ");
}

void InttMonDraw::DrawGrid(TPad* base_pad, std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;

	if(!base_pad)return;

	TPad* grid_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_GridPad", option.c_str(), layer + INTT::LAYER_OFFSET));
	if(!grid_pad)
	{
		base_pad->cd();
		base_pad->Update();
		grid_pad = new TPad
		(
			Form("INTT_%s_Layer%d_GridPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("INTT_%s_Layer%d_GridPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			KEY_FRAC,
			LABEL_FRAC,
			1.0,
			1.0 - TITLE_FRAC
		);
		DrawPad(grid_pad);
		grid_pad->cd();
		grid_pad->Update();

		int i;
		double temp;

		TLine grid_line;

		for(i = 0; i < 2 * INTT::LADDER[layer] + 1; ++i)
		{
			temp = L_MARGIN + (i / 2.0) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[layer];
		
			grid_line.SetX1(temp);
			grid_line.SetY1(B_MARGIN);
			grid_line.SetX2(temp);
			grid_line.SetY2(1.0 - T_MARGIN);
			grid_line.SetLineStyle((i % 2) ? 3 : 1);
			grid_line.SetLineWidth((i % 2) ? 1 : 2);
			grid_line.DrawClone();
		}
	
		for(i = 0; i < INTT::CHIP + 1; ++i)
		{
			temp = B_MARGIN + i * (1.0 - T_MARGIN - B_MARGIN) / INTT::CHIP;
		
			grid_line.SetX1(L_MARGIN);
			grid_line.SetY1(temp);
			grid_line.SetX2(1.0 - R_MARGIN);
			grid_line.SetY2(temp);
			grid_line.SetLineStyle((i % (INTT::CHIP / 2)) ? 3 : 1);
			grid_line.SetLineWidth((i % (INTT::CHIP / 2)) ? 1 : 2);
			grid_line.DrawClone();
		}
	}
}

void InttMonDraw::DrawLabels(TPad* base_pad, std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;

	if(!base_pad)return;

	TPad* label_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_LabelPad", option.c_str(), layer + INTT::LAYER_OFFSET));
	if(label_pad)
	{
		base_pad->cd();
		base_pad->Update();
		label_pad = new TPad
		(
			Form("INTT_%s_Layer%d_LabelPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("INTT_%s_Layer%d_LabelPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			KEY_FRAC,
			0.0,
			1.0,
			LABEL_FRAC
		);
		DrawPad(label_pad);
		label_pad->cd();
		label_pad->Update();

		int i;
		double temp;
	
		TText label_text;

		label_text.SetTextAlign(22);
		label_text.SetTextSize(LABEL_TEXT_SIZE2);

		for(i = 0; i < INTT::LADDER[layer]; ++i)
		{
			temp = L_MARGIN + (i + 0.5) * (1.0 - L_MARGIN - R_MARGIN) / INTT::LADDER[layer];
		
			label_text.SetX(temp);
			label_text.SetY(0.875);
			label_text.SetTitle(Form("%d", i));
			label_text.DrawClone();
		}
	
		temp = (L_MARGIN + 1.0 - R_MARGIN) / 2.0;
	
		label_text.SetTextSize(LABEL_TEXT_SIZE1);
		label_text.SetX(temp);
		label_text.SetY(0.5);
		label_text.SetTitle(Form("Ladder"));
		label_text.DrawClone();
	}
}

void InttMonDraw::DrawExec(TPad* base_pad, std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;

	if(!base_pad)return;

	TPad* exec_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_Layer%d_ExecPad", option.c_str(), layer + INTT::LAYER_OFFSET));
	if(!exec_pad)
	{
		base_pad->cd();
		base_pad->Update();
		exec_pad = new TPad
		(
			Form("INTT_%s_Layer%d_ExecPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("INTT_%s_Layer%d_ExecPad", option.c_str(), layer + INTT::LAYER_OFFSET),
			KEY_FRAC + L_MARGIN * (1.0 - KEY_FRAC),
			LABEL_FRAC + B_MARGIN * (1.0 - TITLE_FRAC - LABEL_FRAC),
			1.0 - R_MARGIN * (1.0 - KEY_FRAC),
			1.0 - TITLE_FRAC - T_MARGIN * (1.0 - TITLE_FRAC - LABEL_FRAC)
		);
		DrawPad(exec_pad);
		exec_pad->AddExec
		(
			Form("INTT_%s_Layer%d_Exec", option.c_str(), layer + INTT::LAYER_OFFSET),
			Form("InttMonDraw::InttExec(\"%s\", %d)", option.c_str(), layer + INTT::LAYER_OFFSET)
		);
	}
}
//---		~Main Drawing Methods		---//

//---		Exec Drawing Methods		---//
void InttMonDraw::DrawExecOption(std::string option, int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;
	if(!(0 <= ladder && ladder < INTT::LADDER[layer]))return;
	if(!(0 <= northsouth && northsouth < INTT::NORTHSOUTH))return;
	if(!(0 <= chip && chip < INTT::CHIP))return;

	//TStyle
	TStyle* style = (TStyle*)gROOT->FindObject(Form("INTT_%s_ExecStyle", option.c_str()));
	if(!style)
	{
		style = new TStyle(Form("INTT_%s_ExecStyle", option.c_str()), Form("INTT_%s_ExecStyle", option.c_str()));
		style->SetOptStat(0);
		style->SetOptTitle(0);
		//...
	}
	style->cd();
	//style->Update();

	//TCanvas
	TCanvas* canvas = (TCanvas*)gROOT->FindObject(Form("INTT_%s_ExecCanvas", option.c_str()));
	if(!canvas)
	{
		canvas = new TCanvas
		(
			Form("INTT_%s_ExecCanvas", option.c_str()),
			Form("INTT_%s_ExecCanvas", option.c_str()),
			0,//-1, //no menu bar
			0,
			CNVS_WIDTH,
			CNVS_HEIGHT
		);
		//canvas->...
		//...
	}

	canvas->Update();
	canvas->cd();

	TPad* base_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecBasePad", option.c_str()));
	if(!base_pad)
	{
		canvas->Update();
		base_pad = new TPad
		(
			Form("INTT_%s_ExecBasePad", option.c_str()),
			Form("INTT_%s_ExecBasePad", option.c_str()),
			0.0,
			0.0,
			1.0,
			1.0
		);
		DrawExecPad(base_pad);
	}

	base_pad->Update();
	base_pad->cd();

	DrawExecTitle	(base_pad, option, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
	DrawExecHist	(base_pad, option, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
	DrawExecLines	(base_pad, option, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
	DrawExecDisp	(base_pad, option, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
	DrawExecExec	(base_pad, option, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
}

void InttMonDraw::DrawExecTitle(TPad* base_pad, std::string option, int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;
	if(!(0 <= ladder && ladder < INTT::LADDER[layer]))return;
	if(!(0 <= northsouth && northsouth < INTT::NORTHSOUTH))return;
	if(!(0 <= chip && chip < INTT::CHIP))return;

	if(!base_pad)return;

	TPad* title_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecTitlePad", option.c_str()));
	if(!title_pad)
	{
		base_pad->cd();
		base_pad->Update();
		title_pad = new TPad
		(
			Form("INTT_%s_ExecTitlePad", option.c_str()),
			Form("INTT_%s_ExecTitlePad", option.c_str()),
			0.0,
			1.0 - EXEC_TITLE_FRAC,
			1.0,
			1.0
		);
		DrawExecPad(title_pad);

		std::string northsouth_str = "North";
		if(northsouth)northsouth_str = "South";

		title_pad->cd();
		title_pad->Update();
		TText title_text
		(
			(L_MARGIN + 1.0 - R_MARGIN) / 2.0,
			0.5,
			Form("Layer: %2d\tLadder: %2d\t(%s)\tChip: %2d", layer + INTT::LAYER_OFFSET, ladder, northsouth_str.c_str(), chip + INTT::CHIP_OFFSET)
		);
		title_text.SetTextAlign(22);
		title_text.SetTextSize(EXEC_TITLE_TEXT_SIZE);
		title_text.DrawClone();
	}
}

void InttMonDraw::DrawExecHist(TPad* base_pad, std::string option, int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;
	if(!(0 <= ladder && ladder < INTT::LADDER[layer]))return;
	if(!(0 <= northsouth && northsouth < INTT::NORTHSOUTH))return;
	if(!(0 <= chip && chip < INTT::CHIP))return;

	if(!base_pad)return;

	TPad* hist_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecHistPad", option.c_str()));
	if(!hist_pad)
	{
		base_pad->cd();
		base_pad->Update();
		hist_pad = new TPad
		(
			Form("INTT_%s_ExecHistPad", option.c_str()),
			Form("INTT_%s_ExecHistPad", option.c_str()),
			0.0,
			EXEC_DISP_FRAC,
			1.0,
			1.0 - EXEC_TITLE_FRAC
		);
		DrawExecPad(hist_pad);
	}
	hist_pad->cd();
	hist_pad->Update();

	TH1* hist = (*(OPTIONS.find(option)->second.second))(layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
	if(hist)hist->DrawCopy("COLZ");
}

void InttMonDraw::DrawExecExec(TPad* base_pad, std::string option, int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;
	if(!(0 <= ladder && ladder < INTT::LADDER[layer]))return;
	if(!(0 <= northsouth && northsouth < INTT::NORTHSOUTH))return;
	if(!(0 <= chip && chip < INTT::CHIP))return;

	if(!base_pad)return;

	TPad* exec_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecExecPad", option.c_str()));
	if(!exec_pad)
	{
		base_pad->cd();
		base_pad->Update();
		exec_pad = new TPad
		(
			Form("INTT_%s_ExecExecPad", option.c_str()),
			Form("INTT_%s_ExecExecPad", option.c_str()),
			EXEC_L_MARGIN,
			EXEC_DISP_FRAC + EXEC_B_MARGIN * (1.0 - TITLE_FRAC - EXEC_DISP_FRAC),
			1.0 - EXEC_R_MARGIN,
			1.0 - EXEC_TITLE_FRAC - EXEC_T_MARGIN * (1.0 - TITLE_FRAC - EXEC_DISP_FRAC)
		);
		DrawExecPad(exec_pad);
		exec_pad->AddExec
		(
			Form("INTT_%s_Exec", option.c_str()),
			Form("InttMonDraw::InttExecExec(\"%s\")", option.c_str())
		);
	}
}

void InttMonDraw::DrawExecLines(TPad* base_pad, std::string option, int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;
	if(!(0 <= ladder && ladder < INTT::LADDER[layer]))return;
	if(!(0 <= northsouth && northsouth < INTT::NORTHSOUTH))return;
	if(!(0 <= chip && chip < INTT::CHIP))return;

	if(!base_pad)return;

	TPad* line_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecLinePad", option.c_str()));
	if(!line_pad)
	{
		base_pad->cd();
		base_pad->Update();
		line_pad = new TPad
		(
			Form("INTT_%s_ExecLinePad", option.c_str()),
			Form("INTT_%s_ExecLinePad", option.c_str()),
			EXEC_L_MARGIN,
			EXEC_DISP_FRAC + EXEC_B_MARGIN * (1.0 - TITLE_FRAC - EXEC_DISP_FRAC),
			1.0 - EXEC_R_MARGIN,
			1.0 - EXEC_TITLE_FRAC - EXEC_T_MARGIN * (1.0 - TITLE_FRAC - EXEC_DISP_FRAC)
		);
		DrawExecPad(line_pad);
	}
}

void InttMonDraw::DrawExecDisp(TPad* base_pad, std::string option, int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer && layer < INTT::LAYER))return;
	if(!(0 <= ladder && ladder < INTT::LADDER[layer]))return;
	if(!(0 <= northsouth && northsouth < INTT::NORTHSOUTH))return;
	if(!(0 <= chip && chip < INTT::CHIP))return;

	if(!base_pad)return;

	TPad* disp_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecDispPad", option.c_str()));
	if(!disp_pad)
	{
		base_pad->cd();
		base_pad->Update();
		disp_pad = new TPad
		(
			Form("INTT_%s_ExecDispPad", option.c_str()),
			Form("INTT_%s_ExecDispPad", option.c_str()),
			0.0,
			0.0,
			1.0,
			EXEC_DISP_FRAC
		);
		DrawExecPad(disp_pad);
	}

	TText* disp_text = (TText*)gROOT->FindObject(Form("INTT_%s_ExecDispText", option.c_str()));
	if(!disp_text)
	{
		disp_pad->cd();
		disp_pad->Update();
		disp_text = new TText
		(
			0.5,
			0.5,
			Form("Channel: %d", 0)
		);
		disp_text->SetName(Form("INTT_%s_ExecDispText", option.c_str()));
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(EXEC_DISP_TEXT_SIZE);

		disp_text->Draw();
	}
}
//---		~Exec Drawing Methods		---//
//===		~Drawing Methods		===//

//===		Exec Functions			===//
void InttMonDraw::InttExec(std::string option, int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(OPTIONS.find(option) == OPTIONS.end())return;
	if(!(0 <= layer and layer < INTT::LAYER))return;

	int binx = gPad->AbsPixeltoX(gPad->GetEventX()) * (2 * INTT::LADDER[layer]);
	int biny = gPad->AbsPixeltoY(gPad->GetEventY()) * (INTT::CHIP);

	if(binx < 0)binx = 0;
	if(biny < 0)biny = 0;

	if(binx >= 2 * INTT::LADDER[layer])binx = 2 * INTT::LADDER[layer] - 1;
	if(biny >= INTT::CHIP)biny = INTT::CHIP - 1;

	int ladder = binx / 2;
	int northsouth = 1 - biny / (INTT::CHIP / 2);
	int chip;

	std::string northsouth_str;
	if(northsouth == 0) //North
	{
		chip = INTT::CHIP / 2 - biny % (INTT::CHIP / 2) + (1 - binx % 2) * (INTT::CHIP / 2) - 1;
		northsouth_str = "North";
	}
	if(northsouth == 1) //South
	{
		chip = biny % (INTT::CHIP / 2) + (binx % 2) * (INTT::CHIP / 2);
		northsouth_str = "South";
	}

	TText* disp_text = (TText*)gROOT->FindObject(Form("INTT_%s_DispText", option.c_str()));
	if(disp_text)disp_text->SetTitle(Form("Layer: %2d\tLadder: %2d\t(%s)\tChip: %2d", layer + INTT::LAYER_OFFSET, ladder, northsouth_str.c_str(), chip + INTT::CHIP_OFFSET));
	TPad* disp_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_DispPad", option.c_str()));
	if(disp_pad)disp_pad->GetCanvas()->Update();

	if(gPad->GetEvent() != 11)return; //left click

	DrawExecOption(option, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET);
}

void InttMonDraw::InttExecExec(std::string option)
{
	if(OPTIONS.find(option) == OPTIONS.end())return;

	float x = gPad->AbsPixeltoX(gPad->GetEventX());
	float y = gPad->AbsPixeltoY(gPad->GetEventY());

	int channel = gPad->AbsPixeltoX(gPad->GetEventX()) * INTT::CHANNEL;

	TText* exec_disp_text = (TText*)gROOT->FindObject(Form("INTT_%s_ExecDispText", option.c_str()));
	if(exec_disp_text)exec_disp_text->SetTitle(Form("Channel: %d", channel));
	TPad* exec_text_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecDispPad", option.c_str()));
	if(exec_text_pad)exec_text_pad->Update();

	TPad* exec_line_pad = (TPad*)gROOT->FindObject(Form("INTT_%s_ExecLinePad", option.c_str()));
	if(exec_line_pad)
	{
		exec_line_pad->Clear();

		exec_line_pad->cd();
		exec_line_pad->Update();

		TLine hline(0.0, y, 1.0, y);
		hline.DrawClone();

		TLine vline(x, 0.0, x, 1.0);
		vline.DrawClone();

		exec_line_pad->Update();
	}
}


//For option "hitmap"
TH1* InttMonDraw::GetLayerHitMap(int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(!(0 <= layer && layer < INTT::LAYER))return nullptr;

	//===	Retrieve Hists from gROOT	===//
	TH2D* hist = (TH2D*)gROOT->FindObject(Form("INTT_HitMap_Layer%d_Hist", layer + INTT::LAYER_OFFSET));

	if(!hist)
	{
		hist = new TH2D
		(
			Form("INTT_HitMap_Layer%d_Hist", layer + INTT::LAYER_OFFSET),
			Form("INTT_HitMap_Layer%d_Hist", layer + INTT::LAYER_OFFSET),
			2 * INTT::LADDER[layer],
			-0.5,
			2 * INTT::LADDER[layer] - 0.5,
			INTT::CHIP,
			-0.5,
			INTT::CHIP - 0.5
		);
		hist->GetXaxis()->SetNdivisions(1, true);
		hist->GetYaxis()->SetNdivisions(1, true);

		hist->GetXaxis()->SetLabelSize(0.0);
		hist->GetYaxis()->SetLabelSize(0.0);

		hist->GetXaxis()->SetTickLength(0.0);
		hist->GetYaxis()->SetTickLength(0.0);

		hist->SetMinimum(-1);
		//...
	}

	hist->Reset();
	//===	~Retrieve Hists from gROOT	===//

	//===	~Retrieve Hists and Vars from OnlMon	===//
	OnlMonClient* omc = OnlMonClient::instance();

	TH1D* hitmap = (TH1D*)(omc->getHisto("InttHitMap"));
	if(!hitmap)
	{
		std::cout << "In InttMonDraw::GetLayerHitMap()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMap\"" << std::endl;
		std::cout << "from OnlMonClient::instance()" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}
	//===	~Retrieve Hists and Vars from OnlMon	===//

	//===	Configure Histograms		===//
	double count;

	int ladder;
	int northsouth;
	int chip;
	int channel;

	int bin_local;
	int bin_global;

	for(bin_local = 1; bin_local < hist->GetNcells() - 1; bin_local++)
	{
		INTT::HitMap::FindLayerIndices(bin_local, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip);
		count = 0;
		for(channel = 0; channel < INTT::CHANNEL; channel++)
		{
			INTT::HitMap::FindGlobalBin(bin_global, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip, channel);
			count += hitmap->GetBinContent(bin_global);
		}
		hist->SetBinContent(bin_local, count);
	}
	//===	~Configure Histograms		===//

	return hist;
}

TH1* InttMonDraw::GetChipHitMap(int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	//===	Retrieve Hists from gROOT		===//
	TH1D* hist = (TH1D*)gROOT->FindObject("INTT_HitMap_ExecHist");
	if(!hist)
	{
		hist = new TH1D
		(
			"INTT_HitMap_ExecHist", 
			"INTT_HitMap_ExecHist", 
			INTT::CHANNEL,
			-0.5,
			INTT::CHANNEL - 0.5
		);
		hist->GetXaxis()->SetNdivisions(INTT::CHANNEL / 4, true);
		//hist->GetXaxis()->SetNdivisions(1, true);

		//hist->GetXaxis()->SetLabelSize(0.0);
		//hist->GetYaxis()->SetLabelSize(0.0);

		//hist->GetXaxis()->SetTickLength(0.0);
		//hist->GetYaxis()->SetTickLength(0.0);

		hist->SetMinimum(-1);
		//...
	}
	hist->Reset();
	//===	~Retrieve Hists from gROOT		===//

	//===	Retrieve Hists and Vars from OnlMon	===//
	OnlMonClient* omc = OnlMonClient::instance();

	TH1D* hitmap = (TH1D*)(omc->getHisto("InttHitMap"));
	if(!hitmap)
	{
		std::cout << "TH1* InttMonDraw::GetChipHitMap()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMap\"" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}
	//===	~Retrieve Hists and Vars from OnlMon	===//

	//===	Configure Histograms			===//
	int bin;
	int channel;

	for(channel = 0; channel < INTT::CHANNEL; channel++)
	{
		INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel);
		hist->SetBinContent(channel + 1, hitmap->GetBinContent(bin));
	}
	//===	~Configure Histograms			===//

	return hist;
}


//for option "hitmapz"
TH1* InttMonDraw::GetLayerHitMapZ(int layer)
{
	layer -= INTT::LAYER_OFFSET;

	if(!(0 <= layer && layer < INTT::LAYER))return nullptr;

	//===	Retrieve Hists from gROOT	===//
	TH2D* hist = (TH2D*)gROOT->FindObject(Form("INTT_HitMapZ_Layer%d_Hist", layer + INTT::LAYER_OFFSET));

	if(!hist)
	{
		hist = new TH2D
		(
			Form("INTT_HitMapZ_Layer%d_Hist", layer + INTT::LAYER_OFFSET),
			Form("INTT_HitMapZ_Layer%d_Hist", layer + INTT::LAYER_OFFSET),
			2 * INTT::LADDER[layer],
			-0.5,
			2 * INTT::LADDER[layer] - 0.5,
			INTT::CHIP,
			-0.5,
			INTT::CHIP - 0.5
		);
		hist->GetXaxis()->SetNdivisions(1, true);
		hist->GetYaxis()->SetNdivisions(1, true);

		hist->GetXaxis()->SetLabelSize(0.0);
		hist->GetYaxis()->SetLabelSize(0.0);

		hist->GetXaxis()->SetTickLength(0.0);
		hist->GetYaxis()->SetTickLength(0.0);

		hist->SetMinimum(-2.0 * NUM_SIG);
		hist->SetMaximum(2.0 * NUM_SIG);

		double levels[2] = {-NUM_SIG, NUM_SIG};
		//double levels[4] = {-FLT_MAX, -NUM_SIG, NUM_SIG, FLT_MAX};
		hist->SetContour(2, levels);
		//...
	}

	hist->Reset();
	//===	~Retrieve Hists from gROOT	===//

	//===	~Retrieve Hists and Vars from OnlMon	===//
	OnlMonClient* omc = OnlMonClient::instance();

	TH1D* hitmap = (TH1D*)(omc->getHisto("InttHitMap"));
	if(!hitmap)
	{
		std::cout << "In InttMonDraw::GetLayerHitMapZ()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMap\"" << std::endl;
		std::cout << "from OnlMonClient::instance()" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}

	TH1D* hitmapref = (TH1D*)(omc->getHisto("InttHitMapRef"));
	if(!hitmap)
	{
		std::cout << "In InttMonDraw::GetLayerHitMapZ()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMapRef\"" << std::endl;
		std::cout << "from OnlMonClient::instance()" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}

	TH1D* num_events = (TH1D*)(omc->getHisto("InttNumEvents"));
	if(!num_events)
	{
		std::cout << "In InttMonDraw::GetLayerHitMapZ()" << std::endl;
		std::cout << "Could not get histogram \"InttHitNumEvents\"" << std::endl;
		std::cout << "from OnlMonClient::instance()" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}
	//===	~Retrieve Hists and Vars from OnlMon	===//

	//===	Configure Histograms		===//
	double count = 0;
	double rate = 0;

	int ladder = 0;
	int northsouth = 0;
	int chip = 0;
	int channel = 0;

	int bin_local = 0;
	int bin_global = 0;

	for(bin_local = 1; bin_local < hist->GetNcells() - 1; bin_local++)
	{
		if(INTT::HitMap::FindLayerIndices(bin_local, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip))
		{
			std::cout << "bad bin to indices:" << std::endl;
			std::cout << "\tbin: " << bin_local << std::endl;
			std::cout << "\tlayer: " << layer + INTT::LAYER_OFFSET << std::endl;
		}
		count = 0;
		rate = 0;
		for(channel = 0; channel < INTT::CHANNEL; channel++)
		{
			if(INTT::HitMap::FindGlobalBin(bin_global, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip, channel))
			{
				std::cout << "bad indices to bin:" << std::endl;
				std::cout << "\tlayer: " << layer + INTT::LAYER_OFFSET << std::endl;
				std::cout << "\tladder: " << ladder << std::endl;
				std::cout << "\tnorthsouth: " << northsouth << std::endl;
				std::cout << "\tchip: " << chip + INTT::CHIP_OFFSET << std::endl;
				std::cout << "\tchannel: " << channel << std::endl;
			}
			count += hitmap->GetBinContent(bin_global);
			rate += hitmapref->GetBinContent(bin_global);
		}
		rate *= num_events->GetBinContent(1);
		hist->SetBinContent(bin_local, (count - rate) / sqrt(rate));
	}
	//===	~Configure Histograms		===//

	return hist;
}

TH1* InttMonDraw::GetChipHitMapZ(int layer, int ladder, int northsouth, int chip)
{
	layer -= INTT::LAYER_OFFSET;
	chip -= INTT::CHIP_OFFSET;

	//===	Retrieve Hists from gROOT		===//
	TH2D* hist = (TH2D*)gROOT->FindObject("INTT_HitMapZ_ExecHist");
	if(!hist)
	{
		hist = new TH2D
		(
			"INTT_HitMapZ_ExecHist", 
			"INTT_HitMapZ_ExecHist", 
			INTT::CHANNEL,
			-0.5,
			INTT::CHANNEL - 0.5,
			1,
			-0.5,
			0.5
		);
		hist->GetXaxis()->SetNdivisions(INTT::CHANNEL / 4, true);
		//hist->GetXaxis()->SetNdivisions(1, true);

		//hist->GetXaxis()->SetLabelSize(0.0);
		//hist->GetYaxis()->SetLabelSize(0.0);

		//hist->GetXaxis()->SetTickLength(0.0);
		//hist->GetYaxis()->SetTickLength(0.0);

		hist->SetMinimum(-2.0 * NUM_SIG);
		hist->SetMaximum(2.0 * NUM_SIG);

		double levels[2] = {-NUM_SIG, NUM_SIG};
		//double levels[4] = {-FLT_MAX, -NUM_SIG, NUM_SIG, FLT_MAX};
		hist->SetContour(2, levels);
		//...
	}
	hist->Reset();
	//===	~Retrieve Hists from gROOT		===//

	//===	Retrieve Hists and Vars from OnlMon	===//
	OnlMonClient* omc = OnlMonClient::instance();

	TH1D* hitmap = (TH1D*)(omc->getHisto("InttHitMap"));
	if(!hitmap)
	{
		std::cout << "In InttMonDraw::GetChipHitMapZ()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMap\"" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}

	TH1D* hitmapref = (TH1D*)(omc->getHisto("InttHitMapRef"));
	if(!hitmap)
	{
		std::cout << "In InttMonDraw::GetChipHitMapZ()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMapRef\"" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}

	TH1D* num_events = (TH1D*)(omc->getHisto("InttNumEvents"));
	if(!num_events)
	{
		std::cout << "In InttMonDraw::GetLayerHitMapZ()" << std::endl;
		std::cout << "Could not get histogram \"InttHitMapRef\"" << std::endl;
		std::cout << "from OnlMonClient::instance()" << std::endl;
		std::cout << "Exiting" << std::endl;

		return hist;
	}
	//===	~Retrieve Hists and Vars from OnlMon	===//

	//===	Configure Histograms			===//
	double count;
	double rate;

	int bin;
	int channel;

	for(channel = 0; channel < INTT::CHANNEL; channel++)
	{
		INTT::HitMap::FindGlobalBin(bin, layer + INTT::LAYER_OFFSET, ladder, northsouth, chip + INTT::CHIP_OFFSET, channel);
		count = hitmap->GetBinContent(bin);
		rate = hitmapref->GetBinContent(bin) * num_events->GetBinContent(1);
		hist->SetBinContent(channel + 1 + (INTT::CHANNEL + 2), (count - rate) / sqrt(rate));
	}
	//===	~Configure Histograms			===//

	return (TH1*)hist;
}
