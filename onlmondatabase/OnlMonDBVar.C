#include <OnlMonDBVar.h>
#include <iostream>
#include <cmath>

using namespace std;

OnlMonDBVar::OnlMonDBVar()
{
  updated = 0;
  for (short i = 0; i < 3; i++)
    {
      val[i] = NAN;
    }
  return ;
}

int
OnlMonDBVar::SetVar(const float rval[3])
{
  updated = 1;
  for (short int i = 0; i < 3 ;i++)
    {
      val[i] = rval[i];
    }
  return 0;
}

void
OnlMonDBVar::Print() const
{
  cout << "Value: " << val[0] << ", Error: " << val[1]
       << ", Quality: " << val[2]
       << ", updated: " << updated << endl;
  return ;
}
