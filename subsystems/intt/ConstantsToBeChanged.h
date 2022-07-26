#ifndef CONSTANTS_TO_BE_CHANGED
#define CONSTANTS_TO_BE_CHANGED

const int NARMS = 2;
const int NSTATIONS = 4;
const int NROCS = 24;

const int NPACKETSARM[NARMS] = {24, 24};
const int FIRSTPACKET[NARMS] = {25001, 25101};
const int NTOTSTRIPS[NARMS] = {540672, 540672};


const int LW = 1; // line width
const int BCOL = 4; // border color
const int WCOL = 2; // warning color
const float TXTSIZE = 0.045;
const float TXTSCALE = 1.0; // how far above the max should the text be?
const float BSCALE = 1.0; // how far above the max should the borders go?
const float MSIZE = 0.5; // marker size

// constants for how to position error messages
const float ERRORSPACE = 0.1;
const float ERROROFF = 0.1;

// canvas appearance
const int NPADSPERARM = 4;
const float DISTBORD = 0.010; // distance to border

// we have a (0-1)x(0-1) space to place out our pads on
// Leave bottom third of each canvas empty for messages
const float PADSIZEYBOTTOM = 0.333; // Leave bottom third of canvas empty
const float PADSIZEX1 = 1.0; // For 1 pad across
const float PADSIZEY1 = 0.666; // For 1 pad vertically 
const float PADSIZEX2 = 0.5; // For 2 pads across
const float PADSIZEY2 = 0.333; // For 2 pads vertically 
const float PADSIZEY3 = 0.222; // For 3 pads vertically 
//
// // we need at least these many events
const int FVTXMINEVENTS = 1000;

const int MAXHITSPERPACKET[2] = {250,250}; // Run-16 Au+Au 200 GeV, yuhw
const float CUT_HOT_PACKET[2] = {160,160};
const float CUT_COLD_PACKET[2] = {30,30};//last one week //{50,50}; //first AuAu run

const float MAXHITSPERWEDGE[2] = {40,40};
const float CUT_COLD_WEDGE_ST0[2] = {1,1};
const float CUT_HOT_WEDGE_ST0[2]        = {20,20};
const float CUT_COLD_WEDGE_ST1[2] = {3,3};
const float CUT_HOT_WEDGE_ST1[2]        = {30,30};

const float MAX_CHIP_CHANNEL    = 0.01;         //Run-16 Au+Au 200 yuhw
const float MAX_YIELD_PER_PACKET_VS_TIME                                =  150;//linear scale //5000;//log scale //Run-16 Au+Au 200 yuhw
const float MAX_YIELD_PER_PACKET_VS_TIME_SHORT  =       200.; //Run-16 Au+Au 200 yuhw



#endif
