#include "OnlMonDraw.h"
#include <iostream>

using namespace std;

OnlMonDraw::OnlMonDraw(const string &name):
  ThisName(name),
  verbosity(0)
{}

int 
OnlMonDraw::Draw(const char *what)
{
  cout << "Draw not implemented by daughter class" << endl;
  return -1;
}

int 
OnlMonDraw::MakePS(const char *what)
{
  cout << "MakePS not implemented by daughter class" << endl;
  return -1;
}

int 
OnlMonDraw::MakeHtml(const char *what)
{
  cout << "MakeHtml not implemented by daughter class" << endl;
  return -1;
}

