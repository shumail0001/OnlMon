#include "InttMonDraw.h"

#include <TROOT.h>
#include <TPad.h>

void
InttMonDraw::DrawPad (
	TPad* b,
	TPad* p,
	struct InttMonDraw::Margin_s const& m
) {
	if(!b)return;
	if(!p)return;

	p->SetFillStyle(4000);	// transparent
	p->Range(0.0, 0.0, 1.0, 1.0);
	p->SetTopMargin(m.t);
	p->SetBottomMargin(m.b);
	p->SetLeftMargin(m.l);
	p->SetRightMargin(m.r);

	CdPad(b);
	p->Draw();
	CdPad(p);
}

void
InttMonDraw::CdPad (
	TPad* p
) {
	if(!p)return;

	p->cd();

	// Shit like this is why I hate ROOT
	gROOT->SetSelectedPad(p); // So TObject::DrawClone draws where you expect
	// ... and whatever other dumb global pointer things I may find later
}

Color_t
InttMonDraw::GetFeeColor (
	int const& fee
) {
	switch (fee % 7) {
	case 0:
		return (fee / 7) ? kGray + 3 : kBlack;
	case 1:
		return kRed + 3 * (fee / 7);
	case 2:
		return kYellow + 3 * (fee / 7);
	case 3:
		return kGreen + 3 * (fee / 7);
	case 4:
		return kCyan + 3 * (fee / 7);
	case 5:
		return kBlue + 3 * (fee / 7);
	case 6:
		return kMagenta + 3 * (fee / 7);
	}
	return kBlack;
}
