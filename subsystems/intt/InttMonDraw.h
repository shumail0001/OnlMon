#ifndef INTT_MON_DRAW_H
#define INTT_MON_DRAW_H

#include "InttMonConstants.h"
#include "InttMon.h"

#include <onlmon/OnlMonDraw.h>
#include <onlmon/OnlMonClient.h>
#include <onlmon/OnlMonDB.h>

#include <TStyle.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TSystem.h>
#include <TROOT.h>

#include <TLine.h>
#include <TText.h>

#include <TH2D.h>
#include <TH1D.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

class InttMonDraw : public OnlMonDraw
{
public:
	InttMonDraw(const std::string &name);
	~InttMonDraw() override;

	int Init() override;
	int Draw(const std::string &what = "ALL") override;
	int MakePS(const std::string &what = "ALL") override;
	int MakeHtml(const std::string &what = "ALL") override;

	static void InttExec(const std::string&, int);
	static void InttExecExec(const std::string&);

	typedef TH1* (*MAIN_FUNC)(int);
	typedef TH1* (*EXEC_FUNC)(int, int, int, int);
	typedef std::map<std::string, std::pair<MAIN_FUNC, EXEC_FUNC>> OPTIONS_TYPE;
	static OPTIONS_TYPE OPTIONS;

	static TH1* GetLayerHitMap(int);
	static TH1* GetChipHitMap(int, int, int, int);

	static TH1* GetLayerHitMapZ(int);
	static TH1* GetChipHitMapZ(int, int, int, int);
	//...

private:
	OnlMonDB *dbvars = nullptr;
	
	//===	Constants for Drawing	===//
	static constexpr double NUM_SIG = 2.0;

	static constexpr int CNVS_WIDTH = 1280;
	static constexpr int CNVS_HEIGHT = 720;

	static constexpr int EXEC_CNVS_WIDTH = 1280;
	static constexpr int EXEC_CNVS_HEIGHT = 1440;

	static constexpr double T_MARGIN = 0.01;
	static constexpr double B_MARGIN = 0.01;
	static constexpr double L_MARGIN = 0.03;
	static constexpr double R_MARGIN = 0.10;

	static constexpr double EXEC_T_MARGIN = 0.10;
	static constexpr double EXEC_B_MARGIN = 0.10;
	static constexpr double EXEC_L_MARGIN = 0.10;
	static constexpr double EXEC_R_MARGIN = 0.10;

	static constexpr double DISP_TEXT_SIZE = 0.20;
	static constexpr double EXEC_DISP_TEXT_SIZE = 0.20;

	static constexpr double TITLE_TEXT_SIZE = 0.50;
	static constexpr double EXEC_TITLE_TEXT_SIZE = 0.50;

	static constexpr double KEY_TEXT_SIZE1 = 0.10;
	static constexpr double KEY_TEXT_SIZE2 = 0.05;

	static constexpr double LABEL_TEXT_SIZE1 = 0.35;
	static constexpr double LABEL_TEXT_SIZE2 = 0.25;

	static constexpr double KEY_FRAC = 0.1;
	static constexpr double LABEL_FRAC = 0.1;
	static constexpr double TITLE_FRAC = 0.1;
	static constexpr double DISP_FRAC = 0.1;

	static constexpr double EXEC_TITLE_FRAC = 0.1;
	static constexpr double EXEC_DISP_FRAC = 0.1;
	//===	~Constants for Drawing	===//

	//===	Drawing Methods		===//
	static void DrawPad(TPad*);
	static void DrawExecPad(TPad*);

	static void DrawDisp(TPad*, const std::string&);
	//---	Main Drawing Methods	---//
	static void DrawOption(const std::string&);

	static void DrawTitle	(TPad*, const std::string&, int);
	static void DrawKey	(TPad*, const std::string&, int);
	static void DrawHist	(TPad*, const std::string&, int);
	static void DrawGrid	(TPad*, const std::string&, int);
	static void DrawLabels	(TPad*, const std::string&, int);
	static void DrawExec	(TPad*, const std::string&, int);
	//---	~Main Drawing Methods	---//

	//---	Exec Drawing Methods	---//
	static void DrawExecOption(const std::string&, int, int, int, int);

	static void DrawExecTitle	(TPad*, const std::string&, int, int, int, int);
	static void DrawExecHist	(TPad*, const std::string&, int, int, int, int);
	static void DrawExecExec	(TPad*, const std::string&, int, int, int, int);
	static void DrawExecLines	(TPad*, const std::string&, int, int, int, int);
	static void DrawExecDisp	(TPad*, const std::string&, int, int, int, int);
	//---	~Exec Drawing Methods	---//
	//===	~Drawing Methods	===//
};

#endif
