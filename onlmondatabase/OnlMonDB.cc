#include "OnlMonDB.h"
#include "OnlMonDBVar.h"
#include "OnlMonDBodbc.h"

#include <OnlMonServer.h>

#include <phool/phool.h>

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;

OnlMonDB::OnlMonDB(const string &thisname): OnlMonBase(thisname), db(0)
{
  return ;
}

OnlMonDB::~OnlMonDB()
{
  delete db;
  while(varmap.begin() != varmap.end())
    {
      delete varmap.begin()->second;
      varmap.erase(varmap.begin());
    }
}

void
OnlMonDB::Print() const
{
  cout << "OnlMonDB Name: " << ThisName << endl;
  map<const std::string, OnlMonDBVar *>::const_iterator iter;
  for (iter = varmap.begin(); iter != varmap.end(); ++iter)
    {
      iter->second->Print();
    }
  return ;
}

int 
OnlMonDB::registerVar(const string &varname)
{
  string cpstring = varname;
  transform(cpstring.begin(), cpstring.end(), cpstring.begin(), (int(*)(int))tolower);
  map<const string,OnlMonDBVar *>::const_iterator iter = varmap.find(cpstring);
  if (iter != varmap.end())
    {
      cout << "Variable " << varname << " allready registered in DB" << endl;
      return -1;
    }
  varmap[cpstring] = new OnlMonDBVar();
  return 0;
}

int 
OnlMonDB::SetVar(const std::string &varname, const float var, const float varerr, const float varqual)
{
  float vararray[3];
  vararray[0] = var;
  vararray[1] = varerr;
  vararray[2] = varqual;
  return SetVar(varname,vararray);
}

int 
OnlMonDB::SetVar(const string &varname, const float var[3])
{
  string cpstring = varname;
  transform(cpstring.begin(), cpstring.end(), cpstring.begin(), (int(*)(int))tolower);
  map<const string,OnlMonDBVar *>::iterator iter = varmap.find(cpstring);
  if (iter != varmap.end())
    {
      iter->second->SetVar(var);
      return 0;
    }
  cout << PHWHERE << " Could not find Variable " << varname << " in DB list" << endl;
  return -1;
}

int
OnlMonDB::DBInit()
{
  if (!db)
    {
      db = new OnlMonDBodbc(ThisName);
    }
  db->Verbosity(Verbosity());
  db->CheckAndCreateTable(varmap);
  return 0;
}

int 
OnlMonDB::DBcommit()
{
  OnlMonServer *se = OnlMonServer::instance();
  
  int iret = 0;
  if (!db)
    {
      cout << "Data Base not initialized, fix your code." << endl;
      cout << "You need to call DBInit() after you registered your variables" << endl;
      return -1;
    }
  iret = db->AddRow(se->CurrentTicks(), se->RunNumber(), varmap);
  if (iret)
    {
      printf("error in dbcommit, ret code %d\n",iret);
      return iret;
    }
  //db->Dump();
  map<const string,OnlMonDBVar *>::iterator iter;
  for (iter = varmap.begin(); iter != varmap.end(); ++iter)
    {
      iter->second->resetupdated();
    }
  return iret;
}

int 
OnlMonDB::DBcommitTest()
{
  static int ifirst = 1;
  int iret = 0;
  static time_t ticks = 0;
  static int runnumber = 90000;
  if (!db)
    {
      cout << "Data Base not initialized, fix your code." << endl;
      cout << "You need to call DBInit() after you registered your variables" << endl;
      return -1;
    }
  if (ifirst)
    {
      ticks = time(NULL);
      ticks -= 2*30*24*60*60;
      ifirst = 0;
    }
  else
    {
      ticks += 3*60;
      runnumber+= 1;
    }
  db->AddRow(ticks, runnumber, varmap);
  if (iret)
    {
      cout << "error in dbcommit" << endl;
      return iret;
    }
  //db->Dump();
  map<const string,OnlMonDBVar *>::iterator iter;
  for (iter = varmap.begin(); iter != varmap.end(); ++iter)
    {
      iter->second->resetupdated();
    }
  return iret;
}

int
OnlMonDB::GetVar(const time_t begin, const time_t end, const std::string &varname, std::vector<time_t> &timestp, std::vector<int> &runnumber, std::vector<float> &var, std::vector<float> &varerr)
{
  if (!db)
    {
      db = new OnlMonDBodbc(ThisName);
    }
  int iret = db->GetVar( begin, end, varname, timestp, runnumber, var, varerr);
  return iret;
}

void
OnlMonDB::Reset()
{
  map<const string,OnlMonDBVar *>::iterator iter;
  for (iter = varmap.begin(); iter != varmap.end(); ++iter)
    {
      iter->second->resetupdated();
    }
  return;
}
