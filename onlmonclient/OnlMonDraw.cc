#include "OnlMonDraw.h"
#include <iostream>

using namespace std;

OnlMonDraw::OnlMonDraw(const string &name):
  ThisName(name),
  verbosity(0)
{}

int 
OnlMonDraw::Draw(const std::string & /* what */)
{
  cout << "Draw not implemented by daughter class" << endl;
  return -1;
}

int 
OnlMonDraw::MakePS(const std::string & /* what */)
{
  cout << "MakePS not implemented by daughter class" << endl;
  return -1;
}

int 
OnlMonDraw::MakeHtml(const std::string & /* what */)
{
  cout << "MakeHtml not implemented by daughter class" << endl;
  return -1;
}

