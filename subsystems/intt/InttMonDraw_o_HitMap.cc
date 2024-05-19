#include "InttMon.h"
#include "InttMonDraw.h"

InttMonDraw::HitMap_s const
InttMonDraw::m_HitMap {
	.cnvs_width = 1280, .cnvs_height = 720,
	.disp_frac = 0.1,
	.disp_text_size = 0.25,
	.name = "INTT_HitMap"
};

int
InttMonDraw::DrawHitMap (
	int icnvs
) {
	std::string name;

	// use gROOT to find TStyle
	name = Form("%s_style", m_HitMap.name.c_str());
	TStyle* style = dynamic_cast<TStyle*>(gROOT->FindObject(name.c_str()));
	if (!style) {
		style = new TStyle(name.c_str(), name.c_str());
		style->SetOptStat(0);
		style->SetOptTitle(0);
		//...
	}
	style->cd();

	// use member TClonesArray instead of (purely) gROOT here
	name = Form("%s", m_HitMap.name.c_str());
	if(!dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()))) { // Only allocate if gROOT doesn't find it
		// This next line would throw a double free() IF ROOT freed the memory when a user closed the TCanvas
		// (I added another delete to see what ROOT does for a double free()--it crashes)
		// Yet, the next line is safe...
		delete TC[icnvs];
		TC[icnvs] = new TCanvas (
			name.c_str(), name.c_str(),
			0, 0,
			m_HitMap.cnvs_width, m_HitMap.cnvs_height
		);
	}
	gSystem->ProcessEvents(); // ...ROOT garbage collection?

	int iret = 0;
	iret += DrawHitMap_DispPad();
	iret += DrawHitMap_SubPads();

	return iret;
}

int
InttMonDraw::DrawHitMap_DispPad (
) {
	std::string name;

	// use gROOT to find parent TPad (TCanvas)
	name = Form("%s", m_HitMap.name.c_str());
	TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
	if(!cnvs) {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return 1;
	}

	// find or make this this pad
	name = Form("%s_disp_pad", m_HitMap.name.c_str());
	TPad* disp_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
	if(!disp_pad) {// Make if it does not exist
		disp_pad = new TPad (
			name.c_str(), name.c_str(),
			0.0, 1.0 - m_HitMap.disp_frac, // Southwest x, y
			1.0, 1.0                                // Northeast x, y
		);
		DrawPad(cnvs, disp_pad);
	}
	CdPad(disp_pad);

	name = Form("%s_disp_text", m_HitMap.name.c_str());
	TText* disp_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
	if(!disp_text) {
		disp_text = new TText (0.5, 0.5, name.c_str());
		disp_text->SetName(name.c_str());
		disp_text->SetTextAlign(22);
		disp_text->SetTextSize(m_HitMap.disp_text_size);
		disp_text->Draw();
	}

	name = "InttEvtHist";
	OnlMonClient* cl = OnlMonClient::instance();
	TH1D* evt_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", 0), name);
	if (!evt_hist) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", 0) << std::endl;
		return 1;
	}

	std::time_t t = cl->EventTime("CURRENT"); // BOR, CURRENT, or EOR
	struct tm* ts = std::localtime(&t);
	name = Form (
		"Run: %08d, Events: %d, Date: %02d/%02d/%4d",
		cl->RunNumber(),
		(int)evt_hist->GetBinContent(1),
		ts->tm_mon + 1, ts->tm_mday, ts->tm_year + 1900
	);
	disp_text->SetTitle(name.c_str());

	name = Form("%s_title_text", m_HitMap.name.c_str());
	TText* title_text = dynamic_cast<TText*>(gROOT->FindObject(name.c_str()));
	if(title_text) return 0; // Early return since the title text is unchanging, and this means we've drawn it

	title_text = new TText (0.5, 0.75, name.c_str());
	title_text->SetName(name.c_str());
	title_text->SetTextAlign(22);
	title_text->SetTextSize(m_HitMap.disp_text_size);
	title_text->Draw();

	name = Form("%s", m_HitMap.name.c_str());
	title_text->SetTitle(name.c_str());

	return 0;
}

int
InttMonDraw::DrawHitMap_SubPads (
) {
	std::string name;

	// use gROOT to find parent TPad (TCanvas)
	name = Form("%s", m_HitMap.name.c_str());
	TCanvas* cnvs = dynamic_cast<TCanvas*>(gROOT->FindObject(name.c_str()));
	if(!cnvs)  {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return 1;
	}

	double x_min = 0.0; double x_max = 1.0;
	double y_min = 0.0; double y_max = 1.0 - m_HitMap.disp_frac;
	for(int i = 0; i < 8; ++i) {
		name = Form("%s_hist_pad_%d", m_HitMap.name.c_str(), i);
		TPad* hist_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
		if(hist_pad) continue;

		hist_pad = new TPad (
			name.c_str(), name.c_str(),
			x_min + (x_max - x_min) * (i % 4 + 0) / 4.0, y_min + (y_max - y_min) * (i / 4 + 0) / 2.0, // Southwest x, y
			x_min + (x_max - x_min) * (i % 4 + 1) / 4.0, y_min + (y_max - y_min) * (i / 4 + 1) / 2.0  // Southwest x, y
		);
		hist_pad->SetRightMargin(0.2);
		DrawPad(cnvs, hist_pad);
	}

	int iret = 0;
	for(int i = 0; i < 8; ++i) {
		iret += DrawHitMap_SubPad(i);
	}

	return iret;
}

int
InttMonDraw::DrawHitMap_SubPad (
	int i 
) {
	std::string name;

	// use gROOT to find parent TPad
	name = Form("%s_hist_pad_%d", m_HitMap.name.c_str(), i);
	TPad* prnt_pad = dynamic_cast<TPad*>(gROOT->FindObject(name.c_str()));
	if(!prnt_pad) {// If we fail to find it, give up
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCouldn't get parent pad \"" << name << "\"" << std::endl;
		return 1;
	}
	CdPad(prnt_pad);

	// For now, just the histogram
	// Other niceties (manual axis labels/ticks, maybe gridlines)
	//   in the future (broken up into other methods)

	name = "InttHitHist";
	OnlMonClient* cl = OnlMonClient::instance();
	TH1D* bco_hist = (TH1D*) cl->getHisto(Form("INTTMON_%d", i), name);
	if (!bco_hist) {
		std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << "\n"
		          << "\tCould not get \"" << name << "\" from " << Form("INTTMON_%d", i) << std::endl;
		return 1;
	}

	name = Form("%s_hist_%01d", m_HitMap.name.c_str(), i);
	TH2D* hist = dynamic_cast<TH2D*>(gROOT->FindObject(name.c_str()));
	if (!hist) {
		hist = new TH2D (
			name.c_str(), name.c_str(),
			26, -0.5, 25.5,
			14, -0.5, 13.5
		);
		hist->GetXaxis()->SetNdivisions(13);//, true);
		hist->GetYaxis()->SetNdivisions(14);//, true);
	}
	hist->Reset();
	prnt_pad->SetLogz();
		
	// Fill
	double bin, norm, max = 0;
	struct InttMon::HitData_s hit_data;
	for(hit_data.fee = 0; hit_data.fee < 14; ++hit_data.fee) {
		for(hit_data.chp = 0; hit_data.chp < 26; ++hit_data.chp) {
			// get entries from server hist
			bin = InttMon::HitBin(hit_data);
			bin = bco_hist->GetBinContent((int)bin); // reuse the index as the value in that bin

			// normalize by strip length--different values for different sensors
			norm = (hit_data.chp % 13 < 5) ? 2.0 : 1.6;
			bin /= norm;

			if(bin > max)max = bin;
			hist->SetBinContent(hit_data.chp + 1, hit_data.fee + 1, bin); // + 1 is b/c the 0th bin is an underflow bin
		}
		hist->SetTitle(Form("intt%01d;Chip ID (0-base);Felix Channel", i));
	}
	hist->GetZaxis()->SetRangeUser(1, max ? max : 10);
	hist->Draw("COLZ");

	return 0;
}
