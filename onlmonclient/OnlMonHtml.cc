#include "OnlMonHtml.h"

#include <onlmon/RunDBodbc.h>

#include <phool/phool.h>

#include <TSystem.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <iterator>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

namespace {

  //___________________________________________________________________________
  vector<string> split(const char sep, const string& s)
  {    
    string str = s;
    std::vector<size_t> slashes_pos;
    
    if ( str[0] != sep ) 
      { 
	str.insert(str.begin(),sep);
      }
    
    if ( str[str.size()-1] != sep ) 
      {
	str.push_back(sep);
      }
    
    for (size_t i = 0 ; i < str.size() ; i++) 
      {
	if ( str[i] == sep ) 
	  { 
	    slashes_pos.push_back(i);
	  }
      }
    
    vector<string> parts;
    
    if ( slashes_pos.size() > 0 ) 
      {
	for (size_t i = 0 ; i < slashes_pos.size()-1 ; i++) 
	  {
	    parts.push_back(str.substr(slashes_pos[i]+1,
				       slashes_pos[i+1]-slashes_pos[i]-1));
	  }
      }  
    
    return parts;    
  }

  //___________________________________________________________________________
  string join(const char sep, const vector<string>& parts)
  {
    string rv;
    for ( size_t i = 0; i < parts.size(); ++i ) 
      {
	rv += parts[i];
	if ( i+1 < parts.size() )
	  {
	    rv += sep;
	  }
      }
    return rv;
  }
}

//_____________________________________________________________________________
OnlMonHtml::OnlMonHtml(const char* topdir) :
  fVerbosity(0),
  fRunNumber(0)
{
  if ( topdir ) 
    {
      fHtmlDir = topdir;
    }
  else
    {
      fHtmlDir = "./";
    }
  rundb = new RunDBodbc();
}

OnlMonHtml::~OnlMonHtml()
{
  if (rundb)
    {
      delete rundb;
    }
}

//_____________________________________________________________________________
void
OnlMonHtml::addMenu(const string& header, const string& path, 
		    const string& relfilename)
{
  ostringstream menufile;

  menufile << fHtmlRunDir << "/menu";

  ifstream in(menufile.str().c_str());

  if (!in.good())
    {
      if (verbosity())
	{
	  cout << PHWHERE << "File " << menufile.str() << " does not exist."
	       << "I'm creating it now" << endl;
	}
      ofstream out(menufile.str().c_str());
      out.close();
    }
  else
    {
      if (verbosity())
	{
	  cout << PHWHERE << "Reading file " << menufile.str() << endl;
	}
    }

  // we read back the old menu file...
  vector<string> lines;
  char str[1024];
  while (in.getline(str,1024,'\n'))
    {
      lines.push_back(str);
    }
  in.close();

  // ... we then append the requested new entry...
  ostringstream sline;
  sline << header << "/" << path << "/" << relfilename;

  lines.push_back(sline.str());

  // ... and we sort this out...
  sort(lines.begin(),lines.end());

  // ... and we remove duplicates lines...
  set<string> olines;
  copy(lines.begin(),lines.end(),
       insert_iterator<set<string> >(olines,olines.begin()));

  // ... and finally we write the full new menu file out.
  ofstream out(menufile.str().c_str());
  copy(olines.begin(),olines.end(),ostream_iterator<string>(out,"\n"));
  out.close();

  // --end of normal menu generation--

  // -- For those who do not have javascript (and thus the menu file
  // created by addMenu will be useless) or 
  // in case cgi script(s) won't be allowed for some reason, 
  // make a plain html menu file too.
  plainHtmlMenu(olines);
}

//_____________________________________________________________________________
void
OnlMonHtml::plainHtmlMenu(const set<string>& olines)
{
  ostringstream htmlmenufile;

  htmlmenufile << fHtmlRunDir << "/menu.html";

  // First get the list of directories found in menu file above (contained
  // in olines set). The olines are of the form:
  // D1/D2/TITLE/link (where link is generally somefile.gif)
  // The dir in this case is D1/D2, which is why we look for 2 slashes
  // below (the one before TITLE and the one before link).
  set<string> dirlist;
  set<string>::const_iterator it;
  for ( it = olines.begin(); it != olines.end(); ++it ) 
    {
      const string& line = *it;
      string::size_type pos = line.find_last_of('/');
      pos = line.substr(0,pos).find_last_of('/');
      string dir = line.substr(0,pos);
      vector<string> parts = split('/',dir);
      for ( size_t i = 0; i <= parts.size(); ++i ) 
	{
	  string dir2 = join('/',parts);
	  dirlist.insert(dir2);
	  parts.pop_back();
	}
    }

  // We now generate the menu.html file.
  ofstream out(htmlmenufile.str().c_str());
  if (!out.good())
    {
      cerr << PHWHERE << " cannot open output file "
	   << htmlmenufile.str() << endl;
      return;
    }

  for ( it = dirlist.begin(); it != dirlist.end(); ++it ) 
    {
      // in the example above, dir is D1/D2
      const string& dir = *it;
      int nslashes = count(dir.begin(),dir.end(),'/')+1;
      string name = dir;
      string::size_type pos = dir.find_last_of('/');
      if ( pos < dir.size() )
	{
	  name = dir.substr(pos+1);
	}
      else
	{
	  out << "<HR><BR>\n";
	}
      out << "<H" << nslashes << ">" << name
	  << "</H" << nslashes << "><BR>\n";

      // We then loop on all the olines, and for those matching the
      // dir pattern, we generate link <A HREF="link">TITLE</A>
      set<string>::const_iterator it2;
      for ( it2 = olines.begin(); it2 != olines.end(); ++it2 ) 
	{
	  const string& line = *it2; 
	  string::size_type pos2 = line.find_last_of('/');
	  pos2 = line.substr(0,pos2).find_last_of('/');
	  string ldir = line.substr(0,pos2);
	  if ( ldir == dir ) // we get a matching line
	    {
	      string sline = line.substr(dir.size()+1);
	      // in the example above, sline is TITLE/link...
	      pos2 = sline.find('/');
	      // ...which we split at the slash pos
	      if ( pos2 < sline.size() )
		{
		  out << "<A HREF=\"" 
		      << sline.substr(pos+1) << "\">"
		      << sline.substr(0,pos) << "</A><BR>\n";
		}
	    }
	}
    }
  out.close();
}

//_____________________________________________________________________________
void
OnlMonHtml::namer(const string& header, 
		  const string& basefilename,
		  const string& ext, 
		  string& fullfilename,
		  string& filename)
{
  ostringstream sfilename;

  sfilename << header << "_";
  if (!basefilename.empty())
    {
      sfilename << basefilename << "_";
    }
  sfilename << runNumber() << "." << ext;

  ostringstream sfullfilename;

  sfullfilename << fHtmlRunDir << "/" << sfilename.str();

  fullfilename = sfullfilename.str();
  filename = sfilename.str();

  if (verbosity())
    {
      cout << PHWHERE << "namer: header=" << header
	   << " basefilename=" << basefilename << " ext=" << ext
	   << endl
	   << "fullfilename=" << fullfilename
	   << " filename=" << filename
	   << endl;
    }
}

//_____________________________________________________________________________
string
OnlMonHtml::registerPage(const string& header,
			 const string& path,
			       const string& basefilename,
			       const string& ext)
{
  string fullfilename;
  string filename;

  namer(header,basefilename,ext,fullfilename,filename);
  addMenu(header,path,filename);
  return fullfilename;
}

//_____________________________________________________________________________
void
OnlMonHtml::runInit()
{
  // Check if html output directory for this run exist. 
  // If not create it.
  // Then check (and create if necessary) the "menu" template file.
  string runtype = "unknowndata";
  string runtmp =  rundb->RunType(fRunNumber);
  if (runtmp == "JUNK")
    {
      runtype = "junkdata";
    }
  else if (runtmp == "PHYSICS")
    {
      runtype = "eventdata";
    }
  else if (runtmp == "CALIBRATION")
    {
      runtype = "calibdata";
    }
  else if (runtmp == "PREJECTED")
    {
      runtype = "prejecteddata";
    }
  else if (runtmp == "LOCALPOLARIMETER")
    {
      runtype = "localpoldata";
    }
  else if (runtmp == "PEDESTAL")
    {
      runtype = "pedestaldata";
    }
  else if (runtmp == "VERNIERSCAN")
    {
      runtype = "vernierscandata";
    }
  else if (runtmp == "ZEROFIELD")
    {
      runtype = "zerofielddata";
    }
  cout << "runtype is " << runtype << endl;

  ostringstream fulldir;

  fulldir << fHtmlDir << "/" << runtype << "/"
	  << runRange() << "/" << runNumber();

  fHtmlRunDir = fulldir.str();
  DIR *htdir = opendir(fulldir.str().c_str());
  if (!htdir)
    {
      vector<string> mkdirlist;
      mkdirlist.push_back(fulldir.str());
      string updir = fulldir.str();
      string::size_type pos1;
      while ((pos1 = updir.rfind("/") ) != string::npos)
	{
	  updir.erase(pos1, updir.size());
	  htdir = opendir(updir.c_str());
	  if (!htdir)
	    {
	      mkdirlist.push_back(updir);
	    }
	  else
	    {
	      closedir(htdir);
	      break;
	    }
	}
      while (mkdirlist.rbegin() != mkdirlist.rend())
	{
	  string md = *(mkdirlist.rbegin());
       if (verbosity())
 	{
 	  cout << PHWHERE << "Trying to create dir " << md << endl;
 	}
	  if (mkdir(md.c_str(), S_IRWXU | S_IRWXG | S_IRWXO))
	    {
	      cout << PHWHERE << "Error creating directory " << md << endl;
	      fHtmlRunDir = fHtmlDir;
	      break;
	    }
	  mkdirlist.pop_back();
	}
      fHtmlRunDir = fulldir.str();
      // use ostringstream fulldir which contains the full directory
      // to create .htaccess file for automatic gunzipping
      fulldir << "/.htaccess";
      ofstream htaccess;
      htaccess.open(fulldir.str().c_str(), ios::trunc); // overwrite if exists
      htaccess << "<Files *.html.gz>" << endl;
      htaccess << "     AddEncoding x-gzip .gz" << endl;
      htaccess << "     AddType text/html .gz" << endl;
      htaccess << "</Files>" << endl;
      htaccess << "<Files *.txt.gz>" << endl;
      htaccess << "     AddEncoding x-gzip .gz" << endl;
      htaccess << "     AddType text/html .gz" << endl;
    htaccess << "</Files>" << endl;
    htaccess.close();
  }
else
  {
    closedir(htdir);
  }

  if (verbosity())
    {
      cout << PHWHERE << "OK. fHtmlRunDir=" << fHtmlRunDir << endl;
    }
}

//_____________________________________________________________________________
void
OnlMonHtml::runNumber(int runnumber)
{
  fRunNumber=runnumber;
  runInit();
}

//_____________________________________________________________________________
string
OnlMonHtml::runRange()
{
  const int range = 1000;
  int start = runNumber()/range;

  ostringstream s;

  s << "run_" << setw(10) << setfill('0') << start*range
    << "_" << setw(10) << setfill('0') << (start+1)*range;

  return s.str();
}
