#include "InttMonDraw.h"
#include "InttMonConstants.h"

InttMonDraw::FelixBcoFphxBco_s const
InttMonDraw::m_FelixBcoFphxBco {
	.cnvs_width = 1280, .cnvs_height = 720,
	.disp_frac = 0.1, .lgnd_frac = 0.15,
	.disp_text_size = 0.25,
	.lgnd_box_width = 0.16, .lgnd_box_height = 0.01, .lgnd_text_size = 0.08,
	.name = "InttFelixBcoFphxBco"
};

void
InttMonDraw::DrawFelixBcoFphxBco (
	std::string const& foo // just so the function signature matches
) {
	std::string name = foo;

	// use gROOT to find base TCanvas for this m_FelixBcoFphxBco.name
	name = Form("%s_style", m_FelixBcoFphxBco.name.c_str());
	TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
	if (!style) {
		style = new TStyle(
			name.c_str(),
			name.c_str());
		style->SetOptStat(0);
		style->SetOptTitle(0);
		//...
	}
	style->cd();

	// use gROOT to find base TCanvas for this m_FelixBcoFphxBco.name
	name = Form("%s", m_FelixBcoFphxBco.name.c_str());
	TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
	if(!cnvs) {// Make if it does not exist
		cnvs = new TCanvas (
			name.c_str(), name.c_str(),
			0, 0,
			m_FelixBcoFphxBco.cnvs_width, m_FelixBcoFphxBco.cnvs_height
		);
	}

	DrawFelixBcoFphxBco_DispPad();
	DrawFelixBcoFphxBco_LgndPad();
	DrawFelixBcoFphxBco_SubPads();
}

void
InttMonDraw::DrawFelixBcoFphxBco_DispPad (
) {
	std::string name;

	// use gROOT to find parent TPad (TCanvas)
	name = Form("%s", m_FelixBcoFphxBco.name.c_str());
	TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
	if(!cnvs) {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return;
	}

	// find or make this this pad
	name = Form("%s_disp_pad", m_FelixBcoFphxBco.name.c_str());
	TPad* disp_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
	if(!disp_pad) {// Make if it does not exist
		disp_pad = new TPad (
			name.c_str(), name.c_str(),
			0.0, 1.0 - m_FelixBcoFphxBco.disp_frac, // Southwest x, y
			1.0, 1.0                                // Northeast x, y
		);
		DrawPad(cnvs, disp_pad);
	}
	CdPad(disp_pad);

	name = Form("%s_disp_text", m_FelixBcoFphxBco.name.c_str());
	TText* disp_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
	if(!disp_text) {
		disp_text = new TText (0.5, 0.5, name.c_str());
		disp_text->SetName(name.c_str());
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(m_FelixBcoFphxBco.disp_text_size);
		disp_text->Draw();
	}

	OnlMonClient* cl = OnlMonClient::instance();
	std::time_t t = cl->EventTime("CURRENT"); // BOR, CURRENT, or EOR
	struct tm* ts = std::localtime(&t);

	TH1D* nevt_hist = dynamic_cast<TH1D*>(cl->getHisto(Form("INTTMON_0"), "InttNumEvents"));
	if(!nevt_hist) return;

	name = Form (
		"Run: %08d, Events: %d, Date: %02d/%02d/%4d",
		cl->RunNumber(),
		(int)nevt_hist->GetBinContent(1),
		ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900
	);
	disp_text->SetTitle(name.c_str());
	// disp_pad->Update();
}

void
InttMonDraw::DrawFelixBcoFphxBco_LgndPad (
) {
	std::string name;

	// use gROOT to find parent TPad (TCanvas)
	name = Form("%s", m_FelixBcoFphxBco.name.c_str());
	TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
	if(!cnvs) {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return;
	}

	// find or make this this pad
	name = Form("%s_lgnd_pad", m_FelixBcoFphxBco.name.c_str());
	TPad* lgnd_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
	if(lgnd_pad) return; // We've already made it and can return here

	lgnd_pad = new TPad (
		name.c_str(), name.c_str(),
		1.0 - m_FelixBcoFphxBco.lgnd_frac, 0.0, // Southwest x, y
		1.0, 1.0 - m_FelixBcoFphxBco.disp_frac  // Northeast x, y
	);
	DrawPad(cnvs, lgnd_pad);

	double x0, y0, x[4], y[4];
	for(int fee = 0; fee < 14; ++fee) {
		x0 = 0.5 - m_FelixBcoFphxBco.lgnd_box_width;
		y0 = (2.0 * fee + 1.0) / (2.0 * 14);

		TText* lgnd_text = new TText(
			x0 + 1.5 * m_FelixBcoFphxBco.lgnd_box_width,
			y0,
			Form("FCh %2d", fee)
		);
		lgnd_text->SetTextAlign(12);
		lgnd_text->SetTextSize(m_FelixBcoFphxBco.lgnd_text_size);
		lgnd_text->SetTextColor(kBlack);
		lgnd_text->Draw();

		x[0] = -1; x[1] = +1; x[2] = +1; x[3] = -1;
		y[0] = -1; y[1] = -1; y[2] = +1; y[3] = +1;
		for(int i = 0; i < 4; ++i) {
			x[i] *= 0.5 * m_FelixBcoFphxBco.lgnd_box_width;
			x[i] += x0;

			y[i] *= 0.5 * m_FelixBcoFphxBco.lgnd_box_height;
			y[i] += y0;
		}

		TPolyLine* box = new TPolyLine(4, x, y);
		box->SetFillColor(GetFeeColor(fee));
		box->SetLineColor(kBlack);
		box->SetLineWidth(1);
		box->Draw("f");
	}
}

void
InttMonDraw::DrawFelixBcoFphxBco_SubPads (
) {
	std::string name;

	// use gROOT to find parent TPad (TCanvas)
	name = Form("%s", m_FelixBcoFphxBco.name.c_str());
	TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
	if(!cnvs)  {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return;
	}

	double x_min = 0.0; double x_max = 1.0 - m_FelixBcoFphxBco.lgnd_frac;
	double y_min = 0.0; double y_max = 1.0 - m_FelixBcoFphxBco.disp_frac;
	for(int i = 0; i < 8; ++i) {
		name = Form("%s_hist_pad_%d", m_FelixBcoFphxBco.name.c_str(), i);
		TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
		if(hist_pad) continue;

		hist_pad = new TPad (
			name.c_str(), name.c_str(),
			x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0, // Southwest x, y
			x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0  // Southwest x, y
		);
		DrawPad(cnvs, hist_pad, (struct Margin_s){.b = .15, .l = .15});
	}

	for(int i = 0; i < 8; ++i) {
		DrawFelixBcoFphxBco_SubPad(i);
	}
}

void
InttMonDraw::DrawFelixBcoFphxBco_SubPad (
	int i 
) {
	std::string name;

	// use gROOT to find parent TPad
	name = Form("%s_hist_pad_%d", m_FelixBcoFphxBco.name.c_str(), i);
	TPad* prnt_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
	if(!prnt_pad) {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return;
	}
	CdPad(prnt_pad);

	// For now, just the histogram
	// Other niceties (manual axis labels/ticks, maybe gridlines)
	//   in the future (broken up into other methods)

	OnlMonClient* cl = OnlMonClient::instance();
	name = "InttBcoDiffMap";
	TH1D* server_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
	if (!server_hist) {
		std::cerr << __PRETTY_FUNCTION__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
		return;
	}

	int max = 0;
	int bin;
	struct INTT::BcoData_s bco_data;
	bco_data.pid = i + 3001;
	TH1D* hist[INTT::FELIX_CHANNEL];
    for(int fee = 0; fee < INTT::FELIX_CHANNEL; ++fee) {
		name = Form("%s_hist_%01d_%02d", m_FelixBcoFphxBco.name.c_str(), i, fee);
		hist[fee] = dynamic_cast<TH1D*>(gROOT->FindObject(name.c_str()));
		if (!hist[fee]) {
			hist[fee] = new TH1D (
				name.c_str(), name.c_str(),
				128,
				0, 127
			);
			hist[fee]->GetXaxis()->SetNdivisions(16);//, true);
		}
		hist[fee]->Reset();
		
		// Fill
		bco_data.fee = fee;
		for(int bco = 0; bco < INTT::BCO; ++bco) {
			bco_data.bco = bco;
			INTT::GetBcoBin(bin, bco_data);
			bin = server_hist->GetBinContent(bin); // reuse the index as the value in that bin
			if(bin > max)max = bin;
			hist[fee]->SetBinContent(bco + 1, bin); // + 1 is b/c the 0th bin is an underflow bin
		}
	}

	prnt_pad->SetLogy();
	for(int fee = 0; fee < INTT::FELIX_CHANNEL; ++fee) {
		hist[fee]->SetLineColor(INTT::GetFeeColor(fee));
		hist[fee]->GetYaxis()->SetRangeUser(1, max ? max * 10 : 10);
		if(fee) {
			hist[fee]->Draw("same");
		} else {
			hist[fee]->SetTitle(Form("intt%01d;Felix BCO - FPHX BCO;Counts (Hits)", i));
			hist[fee]->Draw();
		}
	}
}
