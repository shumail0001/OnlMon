#include "OnlMonDraw.h"

#include <iostream>

OnlMonDraw::OnlMonDraw(const std::string &name)
  : ThisName(name)
{
}

int OnlMonDraw::Draw(const std::string & /* what */)
{
  std::cout << "Draw not implemented by daughter class" << std::endl;
  return -1;
}

int OnlMonDraw::MakePS(const std::string & /* what */)
{
  std::cout << "MakePS not implemented by daughter class" << std::endl;
  return -1;
}

int OnlMonDraw::MakeHtml(const std::string & /* what */)
{
  std::cout << "MakeHtml not implemented by daughter class" << std::endl;
  return -1;
}
