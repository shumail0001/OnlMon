#include "OnlBbcCalib.h"

#include "BbcMonDefs.h"
#include <mbd/MbdGeomV1.h>

// Database Includes
//#include <ffamodules/CDBInterface.h>
//#include <cdbobjects/CDBTTree.h>

#include <phool/phool.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include <TString.h>

OnlBbcCalib::OnlBbcCalib()
{
  Reset();
  _mbdgeom = std::make_unique<MbdGeomV1>();

  //do_templatefit = 1;
}

int OnlBbcCalib::Download_All()
{
  //std::cout << PHWHERE << " In OnlBbcCalib::Download_All()" << std::endl;
  _status = 0;

  // if rc flag MBD_CALDIR does not exist, we create it and set it to an empty string
  const char *bbc_caldir = getenv("BBCCALIB");
  if (!bbc_caldir)
  {
    std::cout << "BBCCALIB environment variable not set" << std::endl;
    exit(1);
  }

  std::cout << "Reading MBD/BBC Calibrations from " << bbc_caldir << std::endl;

  std::string sampmax_file = std::string(bbc_caldir) + "/mbd_sampmax.calib";
  Download_SampMax(sampmax_file);

  std::string qfit_file = std::string(bbc_caldir) + "/mbd_qfit.calib";
  Download_Gains(qfit_file);

  std::string tq_t0_file = std::string(bbc_caldir) + "/mbd_tq_t0.calib";
  Download_TQT0(tq_t0_file);

  std::string tt_tcorr_file = std::string(bbc_caldir) + "/mbd_timecorr.calib";
  Download_TimeCorr(tt_tcorr_file);

  std::string tt_t0_file = std::string(bbc_caldir) + "/mbd_tt_t0.calib";
  Download_TTT0(tt_t0_file);

  std::string slew_file = std::string(bbc_caldir) + "/mbd_slewcorr.calib";
  Download_SlewCorr(slew_file);

  if (do_templatefit)
  {
    std::string shape_file = std::string(bbc_caldir) + "/mbd_shape.calib";
    Download_Shapes(shape_file);
  }

  return _status;
}

int OnlBbcCalib::Download_Gains(const std::string& dbase_location)
{
  // Reset All Values
  _qfit_integ.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_mpv.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_sigma.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_integerr.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_mpverr.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_sigmaerr.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_chi2ndf.fill(std::numeric_limits<float>::quiet_NaN());

  if (Verbosity() > 0)
  {
    std::cout << "Opening " << dbase_location << std::endl;
  }
  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int pmt = -1;
    while (infile >> pmt)
    {
      infile >> _qfit_integ[pmt] >> _qfit_mpv[pmt] >> _qfit_sigma[pmt] >> _qfit_integerr[pmt] >> _qfit_mpverr[pmt] >> _qfit_sigmaerr[pmt] >> _qfit_chi2ndf[pmt];
      if (Verbosity() > 0)
      {
        if (pmt < 5 || pmt >= bbc_onlmon::MBD_N_PMT - 5)
        {
          std::cout << pmt << "\t" << _qfit_integ[pmt] << "\t" << _qfit_mpv[pmt] << "\t" << _qfit_sigma[pmt]
                    << "\t" << _qfit_integerr[pmt] << "\t" << _qfit_mpverr[pmt] << "\t" << _qfit_sigmaerr[pmt]
                    << "\t" << _qfit_chi2ndf[pmt] << std::endl;
        }
      }
    }
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;
  }

  return 1;
}

int OnlBbcCalib::Download_TQT0(const std::string& dbase_location)
{
  // Reset All Values
  _tqfit_t0mean.fill(std::numeric_limits<float>::quiet_NaN());
  _tqfit_t0meanerr.fill(std::numeric_limits<float>::quiet_NaN());
  _tqfit_t0sigma.fill(std::numeric_limits<float>::quiet_NaN());
  _tqfit_t0sigmaerr.fill(std::numeric_limits<float>::quiet_NaN());

  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int pmt = -1;
    while (infile >> pmt)
    {
      infile >> _tqfit_t0mean[pmt] >> _tqfit_t0meanerr[pmt] >> _tqfit_t0sigma[pmt] >> _tqfit_t0sigmaerr[pmt];

      if (Verbosity() > 0)
      {
        if (pmt < 5 || pmt >= bbc_onlmon::MBD_N_PMT - 5)
        {
          std::cout << pmt << "\t" << _tqfit_t0mean[pmt] << "\t" << _tqfit_t0meanerr[pmt]
                    << "\t" << _tqfit_t0sigma[pmt] << "\t" << _tqfit_t0sigmaerr[pmt] << std::endl;
        }
      }
    }
    infile.close();
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;
  }

  return 1;
}

int OnlBbcCalib::Download_TTT0(const std::string& dbase_location)
{
  // Reset All Values
  _ttfit_t0mean.fill(std::numeric_limits<float>::quiet_NaN());
  _ttfit_t0meanerr.fill(std::numeric_limits<float>::quiet_NaN());
  _ttfit_t0sigma.fill(std::numeric_limits<float>::quiet_NaN());
  _ttfit_t0sigmaerr.fill(std::numeric_limits<float>::quiet_NaN());

  if (Verbosity() > 0)
  {
    std::cout << "Opening " << dbase_location << std::endl;
  }
  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int pmt = -1;
    while (infile >> pmt)
    {
      infile >> _ttfit_t0mean[pmt] >> _ttfit_t0meanerr[pmt] >> _ttfit_t0sigma[pmt] >> _ttfit_t0sigmaerr[pmt];

      if (Verbosity() > 0)
      {
        if (pmt < 5 || pmt >= bbc_onlmon::MBD_N_PMT - 5)
        {
          std::cout << pmt << "\t" << _ttfit_t0mean[pmt] << "\t" << _ttfit_t0meanerr[pmt]
                    << "\t" << _ttfit_t0sigma[pmt] << "\t" << _ttfit_t0sigmaerr[pmt] << std::endl;
        }
      }
    }
    infile.close();
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;
  }

  return 1;
}

int OnlBbcCalib::Download_SampMax(const std::string& dbase_location)
{
  // Reset All Values
  _sampmax.fill(-1);

  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int feech = -1;
    while (infile >> feech)
    {
      infile >> _sampmax[feech];
      if (Verbosity() > 0)
      {
        if (feech < 5 || feech >= bbc_onlmon::MBD_N_FEECH - 5)
        {
          std::cout << "sampmax\t" << feech << "\t" << _sampmax[feech] << std::endl;
        }
      }
    }
    infile.close();
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;  // file not found
  }

  return 1;
}

int OnlBbcCalib::Download_Shapes(const std::string& dbase_location)
{
  // Verbosity(100);
  if (Verbosity())
  {
    std::cout << "In OnlBbcCalib::Download_Shapes" << std::endl;
  }
  // Reset All Values
  for (auto& shape : _shape_y)
  {
    shape.clear();
  }
  for (auto& sherr : _sherr_yerr)
  {
    sherr.clear();
  }

  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    if (Verbosity())
    {
      std::cout << "Reading from " << dbase_location << std::endl;
    }
    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int temp_feech = -1;
    int temp_npoints = -1;
    float temp_begintime = -1;
    float temp_endtime = -1;
    while (infile >> temp_feech >> temp_npoints >> temp_begintime >> temp_endtime)
    {
      if (Verbosity())
      {
        std::cout << "shape " << temp_feech << "\t" << temp_npoints << "\t" << temp_begintime << "\t" << temp_endtime << std::endl;
      }
      if (temp_feech < 0 || temp_feech > 255)
      {
        std::cout << "ERROR, invalid FEECH " << temp_feech << " in MBD waveforms calibration" << std::endl;
        _status = -2;
        return _status;
      }

      _shape_npts[temp_feech] = temp_npoints;
      _shape_minrange[temp_feech] = temp_begintime;
      _shape_maxrange[temp_feech] = temp_endtime;

      float temp_val{0.};
      for (int isamp = 0; isamp < temp_npoints; isamp++)
      {
        infile >> temp_val;
        _shape_y[temp_feech].push_back(temp_val);
        if (Verbosity() && (temp_feech == 8 || temp_feech == 255))
        {
          std::cout << _shape_y[temp_feech][isamp] << " ";
          if (isamp % 10 == 9)
          {
            std::cout << std::endl;
          }
        }
      }
      if (Verbosity())
      {
        std::cout << std::endl;
      }
    }

    infile.close();

    // Now read in the sherr file
    std::string sherr_dbase_location = std::regex_replace(dbase_location, std::regex("bbc_shape.calib"), "bbc_sherr.calib");
    if (Verbosity())
    {
      std::cout << "Reading from " << sherr_dbase_location << std::endl;
    }
    infile.open(sherr_dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << sherr_dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    temp_feech = -1;
    temp_npoints = -1;
    temp_begintime = -1;
    temp_endtime = -1;
    while (infile >> temp_feech >> temp_npoints >> temp_begintime >> temp_endtime)
    {
      if (Verbosity())
      {
        std::cout << "sheer " << temp_feech << "\t" << temp_npoints << "\t" << temp_begintime << "\t" << temp_endtime << std::endl;
      }
      if (temp_feech < 0 || temp_feech > 255)
      {
        std::cout << "ERROR, invalid FEECH " << temp_feech << " in MBD waveforms calibration" << std::endl;
        _status = -2;
        return _status;
      }

      _sherr_npts[temp_feech] = temp_npoints;
      _sherr_minrange[temp_feech] = temp_begintime;
      _sherr_maxrange[temp_feech] = temp_endtime;

      float temp_val{0.};
      for (int isamp = 0; isamp < temp_npoints; isamp++)
      {
        infile >> temp_val;
        _sherr_yerr[temp_feech].push_back(temp_val);
        if (Verbosity() && (temp_feech == 8 || temp_feech == 255))
        {
          std::cout << _sherr_yerr[temp_feech][isamp] << " ";
          if (isamp % 10 == 9)
          {
            std::cout << std::endl;
          }
        }
      }
      if (Verbosity())
      {
        std::cout << std::endl;
      }
    }

    infile.close();
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;  // file not found
  }

  // Verbosity(0);
  return 1;
}


int OnlBbcCalib::Download_TimeCorr(const std::string& dbase_location)
{
  //Verbosity(100);
  if ( Verbosity() )
  {
    std::cout << "In OnlBbcCalib::Download_TimeCorr" << std::endl;
  }
  // Reset All Values
  for(auto& tcorr : _tcorr_y) {
    tcorr.clear();
  }

  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    if ( Verbosity() )
    {
      std::cout << "Reading from " << dbase_location << std::endl;
    }
    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int temp_feech = -1;
    int temp_npoints = -1;
    float temp_begintime = -1;
    float temp_endtime = -1;
    while ( infile >> temp_feech >> temp_npoints >> temp_begintime >> temp_endtime )
    {
      if ( Verbosity() )
      {
        std::cout << "tcorr " << temp_feech << "\t" <<  temp_npoints << "\t" <<  temp_begintime << "\t" <<  temp_endtime << std::endl;
      }
      if ( temp_feech<0 || temp_feech>255 )
      {
        std::cout << "ERROR, invalid FEECH " << temp_feech << " in MBD timecorr calibration" << std::endl;
        _status = -2;
        return _status;
      }

      _tcorr_npts[temp_feech] = temp_npoints;
      _tcorr_minrange[temp_feech] = temp_begintime;
      _tcorr_maxrange[temp_feech] = temp_endtime;

      float temp_val{0.};
      for (int isamp=0; isamp<temp_npoints; isamp++)
      {
        infile >> temp_val;
        _tcorr_y[temp_feech].push_back( temp_val );
        if ( Verbosity() && (temp_feech==0 || temp_feech==64) )
        {
          std::cout << _tcorr_y[temp_feech][isamp] << " ";
          if ( isamp%10==9 )
          {
            std::cout << std::endl;
          }
        }
      }
      if ( Verbosity() )
      {
        std::cout << std::endl;
      }
    }

    infile.close();
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;  // file not found
  }

  // Now we interpolate the timecorr
  for (size_t ifeech=0; ifeech<bbc_onlmon::MBD_N_FEECH; ifeech++) 
  {
    if ( _mbdgeom->get_type(ifeech) == 1 )
    {
      continue;  // skip q-channels
    }

    int step = static_cast<int>( (_tcorr_maxrange[ifeech] - _tcorr_minrange[ifeech]) / (_tcorr_npts[ifeech]-1) );
    //std::cout << ifeech << " step = " << step << std::endl;

    for (int itdc=0; itdc<=_tcorr_maxrange[ifeech]; itdc++)
    {
      int calib_index = itdc/step;
      int interp = itdc%step;

      // simple linear interpolation for now
      double slope = (_tcorr_y[ifeech][calib_index+1] - _tcorr_y[ifeech][calib_index])/step;
      float tcorr_interp = _tcorr_y[ifeech][calib_index] + interp*slope;
 
      _tcorr_y_interp[ifeech].push_back( tcorr_interp );

      /*
      if ( ifeech==0 && itdc<2*step )
      {
        std::cout << _tcorr_y_interp[ifeech][itdc] << " ";
        if ( itdc%step==(step-1) ) std::cout << std::endl;
      }
      */
    }

  }

  //Verbosity(0);
  return 1;
}

int OnlBbcCalib::Download_SlewCorr(const std::string& dbase_location)
{
  //Verbosity(100);
  if ( Verbosity() )
  {
    std::cout << "In OnlBbcCalib::Download_SlewCorr" << std::endl;
  }
  // Reset All Values
  for(auto& scorr : _scorr_y) {
    scorr.clear();
  }
  std::fill(_scorr_npts.begin(), _scorr_npts.end(), 0);
  
  TString dbase_file = dbase_location;
  if (dbase_file.EndsWith("calib"))
  {
    if ( Verbosity() )
    {
      std::cout << "Reading from " << dbase_location << std::endl;
    }

    std::ifstream infile(dbase_location);
    if (!infile.is_open())
    {
      std::cout << PHWHERE << "unable to open " << dbase_location << std::endl;
      _status = -3;
      return _status;
    }

    int temp_feech = -1;
    int temp_npoints = 0;
    float temp_beginadc = -1;
    float temp_endadc = -1;
    while ( infile >> temp_feech >> temp_npoints >> temp_beginadc >> temp_endadc )
    {
      if ( Verbosity() )
      {
        std::cout << "scorr " << temp_feech << "\t" <<  temp_npoints << "\t" <<  temp_beginadc << "\t" <<  temp_endadc << std::endl;
      }

      if ( temp_feech<0 || temp_feech>255 )
      {
        std::cout << "ERROR, invalid FEECH " << temp_feech << " in MBD slewcorr calibration" << std::endl;
        _status = -2;
        return _status;
      }

      _scorr_npts[temp_feech] = temp_npoints;
      _scorr_minrange[temp_feech] = temp_beginadc;
      _scorr_maxrange[temp_feech] = temp_endadc;

      float temp_val{0.};
      for (int isamp=0; isamp<temp_npoints; isamp++)
      {
        infile >> temp_val;
        _scorr_y[temp_feech].push_back( temp_val );
        if ( Verbosity() && (temp_feech==0 || temp_feech==64) )
        {
          std::cout << _scorr_y[temp_feech][isamp] << " ";
          if ( isamp%10==9 )
          {
            std::cout << std::endl;
          }
        }
      }
      if ( Verbosity() )
      {
        std::cout << std::endl;
      }
    }

    infile.close();
  }
  else
  {
    std::cout << PHWHERE << ", ERROR, unknown file type, " << dbase_location << std::endl;
    _status = -1;
    return _status;  // file not found
  }

  // Now we interpolate the slewcorr
  for (size_t ifeech=0; ifeech<bbc_onlmon::MBD_N_FEECH; ifeech++) 
  {
    if ( _mbdgeom->get_type(ifeech) == 1 )
    {
      continue;  // skip q-channels
    }
    // skip bad t-channels
    if ( _scorr_npts[ifeech] == 0 )
    {
      //std::cout << "skipping " << ifeech << std::endl;
      continue;
    }

    int step = static_cast<int>( (_scorr_maxrange[ifeech] - _scorr_minrange[ifeech]) / (_scorr_npts[ifeech]-1) );
    //std::cout << ifeech << " step = " << step << std::endl;

    for (int iadc=0; iadc<=_scorr_maxrange[ifeech]; iadc++)
    {
      int calib_index = iadc/step;
      int interp = iadc%step;

      // simple linear interpolation for now
      double slope = (_scorr_y[ifeech][calib_index+1] - _scorr_y[ifeech][calib_index])/step;
      float scorr_interp = _scorr_y[ifeech][calib_index] + interp*slope;
 
      _scorr_y_interp[ifeech].push_back( scorr_interp );


      if ( ifeech==4 && iadc<12 && Verbosity() )
      {
        if ( iadc==0 )
        {
          std::cout << "slewcorr " << ifeech << "\t" << _scorr_npts[ifeech] << "\t"
            << _scorr_minrange[ifeech] << "\t" << _scorr_maxrange[ifeech] << std::endl;
        }
        std::cout << _scorr_y_interp[ifeech][iadc] << " ";
        if ( iadc%step==(step-1) )
        {
          std::cout << std::endl;
        }
      }
    }

  }

  //Verbosity(0);
  return 1;
}

int OnlBbcCalib::Write_SampMax(const std::string& dbfile)
{
  std::ofstream cal_file;
  cal_file.open(dbfile);
  for (int ifeech = 0; ifeech < bbc_onlmon::MBD_N_FEECH; ifeech++)
  {
    cal_file << ifeech << "\t" << _sampmax[ifeech] << std::endl;
  }
  cal_file.close();

  return 1;
}

int OnlBbcCalib::Write_TTT0(const std::string& dbfile)
{
  std::ofstream cal_t0_file;
  cal_t0_file.open(dbfile);
  for (int ipmt = 0; ipmt < bbc_onlmon::MBD_N_PMT; ipmt++)
  {
    cal_t0_file << ipmt << "\t" << _ttfit_t0mean[ipmt] << "\t" << _ttfit_t0meanerr[ipmt]
      << "\t" << _ttfit_t0sigma[ipmt] << "\t" << _ttfit_t0sigmaerr[ipmt] << std::endl;
  }
  cal_t0_file.close();

  return 1;
}

int OnlBbcCalib::Write_TQT0(const std::string& dbfile)
{
  std::ofstream cal_t0_file;
  cal_t0_file.open(dbfile);
  for (int ipmt = 0; ipmt < bbc_onlmon::MBD_N_PMT; ipmt++)
  {
    cal_t0_file << ipmt << "\t" << _tqfit_t0mean[ipmt] << "\t" << _tqfit_t0meanerr[ipmt]
      << "\t" << _tqfit_t0sigma[ipmt] << "\t" << _tqfit_t0sigmaerr[ipmt] << std::endl;
  }
  cal_t0_file.close();

  return 1;
}

void OnlBbcCalib::Update_TQT0(const float dz)
{
  // dz is what we need to move the MBD z by
  const float dt = dz/bbc_onlmon::C;

  for (int ipmt=0; ipmt<bbc_onlmon::MBD_N_PMT; ipmt++)
  {
    if ( ipmt<64 )  // south
    {
      _tqfit_t0mean[ipmt] -= dt;
    }
    else
    {
      _tqfit_t0mean[ipmt] += dt;
    }
  }
}

void OnlBbcCalib::Update_TTT0(const float dz)
{
  // dz is what we need to move the MBD z by
  const float dt = dz/bbc_onlmon::C;

  for (int ipmt=0; ipmt<bbc_onlmon::MBD_N_PMT; ipmt++)
  {
    if ( ipmt<64 )  // south
    {
      _ttfit_t0mean[ipmt] -= dt;
    }
    else
    {
      _ttfit_t0mean[ipmt] += dt;
    }
  }
}

void OnlBbcCalib::Reset_TTT0()
{
  _ttfit_t0mean.fill( 0. );
  _ttfit_t0meanerr.fill( 0. );
  _ttfit_t0sigma.fill(std::numeric_limits<float>::quiet_NaN());
  _ttfit_t0sigmaerr.fill(std::numeric_limits<float>::quiet_NaN());
}

void OnlBbcCalib::Reset_TQT0()
{
  _tqfit_t0mean.fill( 0. );
  _tqfit_t0meanerr.fill( 0. );
  _tqfit_t0sigma.fill(std::numeric_limits<float>::quiet_NaN());
  _tqfit_t0sigmaerr.fill(std::numeric_limits<float>::quiet_NaN());
}

void OnlBbcCalib::Reset_Gains()
{
  // Set all initial values
  _qfit_integ.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_mpv.fill( 1.0 );
  _qfit_sigma.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_integerr.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_mpverr.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_sigmaerr.fill(std::numeric_limits<float>::quiet_NaN());
  _qfit_chi2ndf.fill(std::numeric_limits<float>::quiet_NaN());
}

void OnlBbcCalib::Reset()
{
  Reset_TTT0();
  Reset_TQT0();
  Reset_Gains();

  _sampmax.fill(-1);
}
