#ifndef INTT_CONSTANTS_H
#define INTT_CONSTANTS_H

const int LAYER = 4;
const int LAYER_OFFSET = 3;
const int LADDER[LAYER] = {12, 12, 16, 16};
const int NORTHSOUTH = 2;
const int CHIP = 26;
const int CHIP_OFFSET = 1;
const int CHANNEL = 128;

//      Ladder Structure                //
//======================================//
//      U14     U1      Type B  North   //
//      U15     U2        .       .     //
//      U16     U3        .       .     //
//      U17     U4        .       .     //
//      U18     U5      Type B    .     //
//------------------------------  .     //
//      U19     U6      Type A    .     //
//      U20     U7        .       .     //
//      U21     U8        .       .     //
//      U22     U9        .       .     //
//      U23     U10       .       .     //
//      U24     U11       .       .     //
//      U25     U12       .       .     //
//      U26     U13     Type A  North   //
//--------------------------------------//
//      U13     U26     Type A  South   //
//      U12     U25       .       .     //
//      U11     U24       .       .     //
//      U10     U23       .       .     //
//      U9      U22       .       .     //
//      U8      U21       .       .     //
//      U7      U20       .       .     //
//      U6      U19     Type A    .     //
//------------------------------  .     //
//      U5      U18     Type B    .     //
//      U4      U17       .       .     //
//      U3      U16       .       .     //
//      U2      U15       .       .     //
//      U1      U14     Type B  South   //
//======================================//

#endif
