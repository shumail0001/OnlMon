#include "InttMonConstants.h"

Color_t INTT::GetFeeColor(int fee)
{
  switch (fee % 7)
  {
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

void INTT::GetBcoBin(int& b, struct BcoData_s const& bco_data)
{
  int s = 1;
  b = 0;

  b += bco_data.bco * s;
  s *= BCO;

  b += bco_data.fee * s;
  s *= FELIX_CHANNEL;

  b += (bco_data.pid - 3001) * s;

  ++b;
}

void INTT::GetBcoIndexes(int b, struct BcoData_s& bco_data)
{
  --b;

  bco_data.bco = b % BCO;
  b /= BCO;

  bco_data.fee = b % FELIX_CHANNEL;
  b /= FELIX_CHANNEL;

  bco_data.pid = b + 3001;
}

void INTT::GetFelixBinFromIndexes(int& b, int felix_channel, struct Indexes_s const& indexes)
{
  int s = 1;
  b = 0;

  b += indexes.adc * s;
  s *= ADC;

  b += indexes.chn * s;
  s *= CHANNEL;

  b += indexes.chp * s;
  s *= CHIP;

  b += felix_channel * s;

  ++b;
}

void INTT::GetFelixIndexesFromBin(int b, int& felix_channel, struct Indexes_s& indexes)
{
  --b;

  indexes.adc = b % ADC;
  b /= ADC;

  indexes.chn = b % CHANNEL;
  b /= CHANNEL;

  indexes.chp = b % CHIP;
  b /= CHIP;

  felix_channel = b;
}

void INTT::GetGlobalChipBinXYFromIndexes(int& bin_x, int& bin_y, struct Indexes_s const& indexes)
{
  bin_x = 2 * indexes.ldr + (indexes.arm != (indexes.chp / (INTT::CHIP / 2)));
  ++bin_x;
  bin_y = indexes.arm ? INTT::CHIP - indexes.chp % (INTT::CHIP / 2) - 1 : indexes.chp % (INTT::CHIP / 2);
  ++bin_y;
}

void INTT::GetIndexesFromGlobalChipBinXY(int const& bin_x, int const& bin_y, struct Indexes_s& indexes)
{
  indexes.ldr = bin_x / 2;
  indexes.arm = bin_y / (INTT::CHIP / 2);

  indexes.chp = bin_y < INTT::CHIP / 2 ? bin_y : INTT::CHIP - bin_y - 1;
  indexes.chp += (indexes.arm != (bin_x % 2)) * (INTT::CHIP / 2);
}

void INTT::GetLocalChannelBinXYFromIndexes(int& bin_x, int& bin_y, struct Indexes_s const& indexes)
{
  bin_x = indexes.chn;
  ++bin_x;
  bin_y = indexes.adc;
  ++bin_y;
}

void INTT::GetIndexesFromLocalChannelBinXY(int const& bin_x, int const& bin_y, struct Indexes_s& indexes)
{
  indexes.chn = bin_x;
  indexes.adc = bin_y;
}

void INTT::GetGlobalLadderBinXYFromIndexes(int& bin_x, int& bin_y, struct Indexes_s const& indexes)
{
  bin_x = indexes.ldr;
  ++bin_x;
  bin_y = indexes.chp;
  ++bin_y;
}

void INTT::GetIndexesFromGlobalLadderBinXY(int const& bin_x, int const& bin_y, struct Indexes_s& indexes)
{
  indexes.ldr = bin_x;
  indexes.arm = bin_y;
}

void INTT::GetLocalChipBinXYFromIndexes(int& bin_x, int& bin_y, struct Indexes_s const& indexes)
{
  bin_x = (indexes.arm != indexes.chp / (INTT::CHIP / 2)) ? 2 * INTT::CHANNEL - indexes.chn - 1 : indexes.chn;
  ++bin_x;
  bin_y = indexes.arm ? INTT::CHIP / 2 - indexes.chp % (INTT::CHIP / 2) - 1 : indexes.chp % (INTT::CHIP / 2);
  ++bin_y;
}

void INTT::GetIndexesFromLocalChipBinXY(int const& bin_x, int const& bin_y, struct Indexes_s& indexes)
{
  indexes.chp = indexes.arm ? INTT::CHIP / 2 - bin_y - 1 : bin_y;
  indexes.chp += (indexes.arm != (bin_x % 2)) * (INTT::CHIP / 2);

  indexes.chn = bin_x < INTT::CHANNEL ? bin_x : 2 * INTT::CHANNEL - bin_x - 1;
}
