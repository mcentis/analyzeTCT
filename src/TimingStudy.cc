#include "TimingStudy.h"

#include "stdlib.h"

#include "TDirectory.h"

TimingStudy::TimingStudy(AnalysisClass* acl, const char* dirName)
  : AnalysisPrototype(acl, dirName)
{
  //================================================= get stuff from config file =====================
  _xcutLo = atof(_acl->_cfgAnalysis->GetValue("xcutLo").c_str());
  _xcutHi = atof(_acl->_cfgAnalysis->GetValue("xcutHi").c_str());
  if(_xcutLo >= _xcutHi)
    _applyXcut = false;
  else
    _applyXcut = true;

  _ycutLo = atof(_acl->_cfgAnalysis->GetValue("ycutLo").c_str());
  _ycutHi = atof(_acl->_cfgAnalysis->GetValue("ycutHi").c_str());
  if(_ycutLo >= _ycutHi)
    _applyYcut = false;
  else
    _applyYcut = true;

  _zcutLo = atof(_acl->_cfgAnalysis->GetValue("zcutLo").c_str());
  _zcutHi = atof(_acl->_cfgAnalysis->GetValue("zcutHi").c_str());
  if(_zcutLo >= _zcutHi)
    _applyZcut = false;
  else
    _applyZcut = true;

  _pol1 = atoi(_acl->_cfgAnalysis->GetValue("polarity1").c_str());
  _pol1 /= abs(_pol1); // normalize polarity

  _pol2 = atoi(_acl->_cfgAnalysis->GetValue("polarity2").c_str());
  _pol2 /= abs(_pol2); // normalize polarity

  _nbStart1 = atof(_acl->_cfgAnalysis->GetValue("nbStart1").c_str());
  _nbStop1 = atof(_acl->_cfgAnalysis->GetValue("nbStop1").c_str());

  _nbStart2 = atof(_acl->_cfgAnalysis->GetValue("nbStart2").c_str());
  _nbStop2 = atof(_acl->_cfgAnalysis->GetValue("nbStop2").c_str());

  _intStart1 = atof(_acl->_cfgAnalysis->GetValue("intStart1").c_str());
  _intStop1 = atof(_acl->_cfgAnalysis->GetValue("intStop1").c_str());
  
  _intStart2 = atof(_acl->_cfgAnalysis->GetValue("intStart1").c_str());
  _intStop2 = atof(_acl->_cfgAnalysis->GetValue("intStop1").c_str());

  //======================================== histos =============================================
  _supPulse1 = new TH2D("supPulse1", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 1000, -0.5, 1.5);
  _supPulse2 = new TH2D("supPulse2", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 1000, -0.5, 1.5);

  _noiseDistr1 = new TH1D("noise1", ";Voltage [V];Entries", 1000, -0.5, 0.5);
  _noiseDistr2 = new TH1D("noise2", ";Voltage [V];Entries", 1000, -0.5, 0.5);

  _baselineDistr1 = new TH1D("baseline1", ";Voltage [V];Entries", 1000, -0.05, 0.05);
  _baselineDistr2 = new TH1D("baseline2", ";Voltage [V];Entries", 1000, -0.05, 0.05);

  _ampliDistr1 = new TH1D("amplitude1", ";Voltage [V];Entries", 1000, -0.1, 1.5);
  _ampliDistr2 = new TH1D("amplitude2", ";Voltage [V];Entries", 1000, -0.1, 1.5);

  _riseTime1090_1 = new TH1D("riseTime1090_1", ";Rise time 10% 90% [s];Entries", 1000, 0, 3e-9);
  _riseTime1090_2 = new TH1D("riseTime1090_2", ";Rise time 10% 90% [s];Entries", 1000, 0, 3e-9);

  _riseTimeAmpli1 = new TH2D("riseTimeAmpli1", ";Amplitude [V];Rise time 10% 90% [s];Entries", 1000, -0.1, 1.5, 1000, 0, 3e-9);
  _riseTimeAmpli2 = new TH2D("riseTimeAmpli2", ";Amplitude [V];Rise time 10% 90% [s];Entries", 1000, -0.1, 1.5, 1000, 0, 3e-9);

  InitThrStudy();
  
  return;
}

TimingStudy::~TimingStudy()
{
  delete _supPulse1;
  delete _supPulse2;

  delete _noiseDistr1;
  delete _noiseDistr2;

  delete _baselineDistr1;
  delete _baselineDistr2;

  delete _ampliDistr1;
  delete _ampliDistr2;

  delete _riseTime1090_1;
  delete _riseTime1090_2;

  delete _riseTimeAmpli1;
  delete _riseTimeAmpli2;

  delete _timeDiffMeanCFDfrac;
  delete _timeDiffStdDevCFDfrac;
  delete _bestCFDdistr;

  delete _neventsLEDthr;
  delete _timeDiffMeanLEDthr;
  delete _timeDiffStdDevLEDthr;
  delete _bestLEDdistr;

  return;
}

void TimingStudy::AnalysisAction()
{
  //position cut
  if(_applyXcut && (_acl->_x <= _xcutLo || _acl->_x >= _xcutHi))
    return;

  if(_applyYcut && (_acl->_y <= _ycutLo || _acl->_y >= _ycutHi))
    return;

  if(_applyZcut && (_acl->_z <= _zcutLo || _acl->_z >= _zcutHi))
    return;
  // if arrived till here the event passed the position cut
  
  for(int i = 0; i < _acl->_npt; ++i){
    _supPulse1->Fill(_acl->_time1[i], _acl->_trace1[i] * _pol1);
    _supPulse2->Fill(_acl->_time2[i], _acl->_trace2[i] * _pol2);

    if(_acl->_time1[i] > _nbStart1 && _acl->_time1[i] < _nbStop1)
      _noiseDistr1->Fill(_acl->_trace1[i] * _pol1);
    
    if(_acl->_time2[i] > _nbStart2 && _acl->_time2[i] < _nbStop2)
      _noiseDistr2->Fill(_acl->_trace2[i] * _pol2);
  }

  _bl1 = CalcBaseline(_acl->_trace1, _acl->_time1, _acl->_npt, _pol1, _nbStart1, _nbStop1);
  _baselineDistr1->Fill(_bl1);

  _bl2 = CalcBaseline(_acl->_trace2, _acl->_time2, _acl->_npt, _pol2, _nbStart2, _nbStop2);
  _baselineDistr2->Fill(_bl2);

  FindMax(_acl->_trace1, _acl->_time1, _acl->_npt, _pol1, _ampli1, _maxPos1);
  _ampli1 -= _bl1;
  _ampliDistr1->Fill(_ampli1);
  
  FindMax(_acl->_trace2, _acl->_time2, _acl->_npt, _pol2, _ampli2, _maxPos2);
  _ampli2 -= _bl2;
  _ampliDistr2->Fill(_ampli2);

  FillRiseTime_RiseTimeAmpli();

  FillThrStudy();
  
  return;
}

void TimingStudy::Save()
{  
  TDirectory* dir = _acl->_outFile->mkdir(_dirName.c_str());
  dir->cd();

  _supPulse1->Write();
  _supPulse2->Write();

  _noiseDistr1->Write();
  _noiseDistr2->Write();

  _baselineDistr1->Write();
  _baselineDistr2->Write();  

  _ampliDistr1->Write();
  _ampliDistr2->Write();

  _riseTime1090_1->Write();
  _riseTime1090_2->Write();
  
  _riseTimeAmpli1->Write();
  _riseTimeAmpli2->Write();

  ProcessThrStudy(); // not the best place for this. Dedicated func in prototype?
  WriteThrStudy();
  
  return;
}

double TimingStudy::CalcBaseline(Double_t* tra, Double_t* tim, Int_t n, int pol, double start, double stop)
{
  double sum = 0;
  int count = 0;
  for(int i = 0; i < n; ++i)
    if(tim[i] > start && tim[i] < stop){
      sum += tra[i];
      count++;
    }
 
  return pol * sum/count;
}

void TimingStudy::FindMax(Double_t* tra, Double_t* tim, Int_t n, int pol, double& max, double& maxpos)
{
  int ptPos = -1;
  double maximum = -1e6;
  for(int i = 0; i < n; ++i)
    if(pol * tra[i] > maximum){
      maximum = pol * tra[i];
      ptPos = i;
    }
  
  // interpolation using points near maximum
  // function y = a x**2 + b x + c
  double y1 = tra[ptPos - 1];
  double y2 = tra[ptPos];
  double y3 = tra[ptPos + 1];
  double x1 = tim[ptPos - 1];
  double x2 = tim[ptPos];
  double x3 = tim[ptPos + 1];

  double a = ( y3-y1 - (y2-y1)*(x3-x1)/(x2-x1) ) / ( pow(x3,2)-pow(x1,2) - (pow(x2,2)-pow(x1,2))*(x3-x1)/(x2-x1) );
  double b = ( y2-y1 - a*(pow(x2,2)-pow(x1,2)) ) / (x2-x1);
  double c = y1 - a*pow(x1,2) - b*x1;
  
  max = - pow(b,2)/(4*a) + c;
  max *= pol;
  maxpos = -b/(2*a);
    
  return;
}

double TimingStudy::CalcTimeThrLinear2pt(Double_t* tra, Double_t* tim, Int_t n, int pol, double thr, double offset)
{
  int i = 0;
  for(; i < n; ++i)
    if(pol * tra[i] - offset > thr) break;

  // linear interpolation between points below and above thr
  // y = a x + b
  double y1 = pol * tra[i] - offset;
  double y2 = pol * tra[i - 1] - offset;
  double x1 = tim[i];
  double x2 = tim[i - 1];

  double a = (y2-y1)/(x2-x1);
  double b = y1 -a*x1;

  return (thr - b)/a;
}

void TimingStudy::CalcMeanStdDev(std::vector<double> vec, double& mean, double& stdDev, double& Emean, double& EstdDev)
{
  int N = vec.size();

  if(N == 0){
    mean = 0;
    stdDev = 0;
    Emean = 0;
    EstdDev = 0;
    return;
  }

  if(N < 4){ // to avoid strange results in error calculation
    mean = vec[0];
    stdDev = 0;
    Emean = 0;
    EstdDev = 0;
    return;
  }

  double sum = 0;
  double sumD2 = 0;
  double sumD4 = 0;
  
  for(std::vector<double>::iterator it = vec.begin(); it != vec.end(); ++it)
    sum += *it;

  mean = sum / N; // mean

  for(std::vector<double>::iterator it = vec.begin(); it != vec.end(); ++it){
    sumD2 += pow(*it - mean, 2);
    sumD4 += pow(*it - mean, 4);
  }

  double mu2 = sumD2 / (N - 1); // second central moment (variance)
  
  stdDev = sqrt(mu2); // std dev

  Emean = sqrt(mu2/N);

  double mu4 = ( pow(N, 2)*sumD4/(N-1) - 3*(2*N-3)*pow(mu2, 2) ) / (pow(N, 2)-3*N+3); // fourth central moment from http://mathworld.wolfram.com/SampleCentralMoment.html

  double Emu2 = sqrt((pow(N-1, 2) * mu4 - (N-1) * (N-3) * pow(mu2, 2)) / pow(N, 3)); // std dev of mu2 distribution from http://mathworld.wolfram.com/SampleVarianceDistribution.html
    
  EstdDev = 0.5 * Emu2 / sqrt(mu2);

  return;
}

void TimingStudy::FillRiseTime_RiseTimeAmpli()
{
  double t1, t2;

  t1 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _pol1, 0.1 * _ampli1, _bl1);
  t2 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _pol1, 0.9 * _ampli1, _bl1);

  _riseTime1090_1->Fill(t2-t1);
  _riseTimeAmpli1->Fill(_ampli1, t2-t1);
  
  t1 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _pol2, 0.1 * _ampli2, _bl2);
  t2 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _pol2, 0.9 * _ampli2, _bl2);

  _riseTime1090_2->Fill(t2-t1);
  _riseTimeAmpli2->Fill(_ampli2, t2-t1);

  return;
}

void TimingStudy::InitThrStudy()
{
  //======================== CFD thresholds
  const int fracBins = 19;
  double fracMin = 0.025;
  double fracMax = 0.975;
  _timeDiffMeanCFDfrac = new TH2D("timeDiffMeanCFDfrac", ";CFD frac 1;CFD frac 2;Mean #Delta t [s]", fracBins, fracMin, fracMax, fracBins, fracMin, fracMax);
  _timeDiffStdDevCFDfrac = new TH2D("timeDiffStdDevCFDfrac", ";CFD frac 1;CFD frac 2;Std Dev #Delta t [s]", fracBins, fracMin, fracMax, fracBins, fracMin, fracMax);
  for(int i = 0; i < _timeDiffMeanCFDfrac->GetNbinsX(); ++i)
    _CFDfracVec.push_back(_timeDiffMeanCFDfrac->GetXaxis()->GetBinCenter(i+1)); // threshold vector
  
  _dtCFDfrac = new std::vector<double>*[fracBins + 2]; // underflow and overflow bins
  for(int i = 0; i < fracBins + 2; ++i)
    _dtCFDfrac[i] = new std::vector<double>[fracBins + 2];

  _bestCFDdistr = new TH1D("bestCFDdistr", ";#Delta t [s];Events", 4000, 0, 5e-9);

  //======================== LED thresholds
  const int thrBins = 25;
  double thrMin = 0.01;
  double thrMax = 0.51;
  _neventsLEDthr = new TH2D("neventsLEDthr", ";Thr 1 [V];Thr 2 [V];Entries", thrBins, thrMin, thrMax, thrBins, thrMin, thrMax);
  _timeDiffMeanLEDthr = new TH2D("timeDiffMeanLEDthr", ";Thr 1 [V];Thr 2 [V];Mean #Delta t [s]", thrBins, thrMin, thrMax, thrBins, thrMin, thrMax);
  _timeDiffStdDevLEDthr = new TH2D("timeDiffStdDevLEDthr", ";Thr 1 [V];Thr 2 [V];Std Dev #Delta t [s]", thrBins, thrMin, thrMax, thrBins, thrMin, thrMax);

  for(int i = 0; i < _timeDiffMeanLEDthr->GetNbinsX(); ++i)
    _LEDthrVec.push_back(_timeDiffMeanLEDthr->GetXaxis()->GetBinCenter(i+1)); // threshold vector

  _dtLEDthr = new std::vector<double>*[thrBins + 2]; // underflow and overflow bins
  for(int i = 0; i < thrBins + 2; ++i)
    _dtLEDthr[i] = new std::vector<double>[thrBins + 2];

  _bestLEDdistr = new TH1D("bestLEDdistr", ";#Delta t [s];Events", 4000, 0, 5e-9);
  
  return;
}

void TimingStudy::FillThrStudy()
{
  double t1, t2;
  int iBin, jBin;

  //======================= CFD thresholds
  for(unsigned int i = 0; i < _CFDfracVec.size(); ++i)
    for(unsigned int j = 0; j < _CFDfracVec.size(); ++j){
      t1 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _pol1, _CFDfracVec[i] * _ampli1, _bl1);
      t2 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _pol2, _CFDfracVec[j] * _ampli2, _bl2);
      
      iBin = _timeDiffMeanCFDfrac->GetXaxis()->FindBin(_CFDfracVec[i]);
      jBin = _timeDiffMeanCFDfrac->GetYaxis()->FindBin(_CFDfracVec[j]);
      
      _dtCFDfrac[iBin][jBin].push_back(t2-t1);
    }
  
  //======================= LED thresholds
  for(unsigned int i = 0; i < _LEDthrVec.size(); ++i)
    for(unsigned int j = 0; j < _LEDthrVec.size(); ++j)
      if(_ampli1 > _LEDthrVec[i] * 1.05 && _ampli2 > _LEDthrVec[j] * 1.05){ // the maximum is at least 5% bigger than the threshold
	t1 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _pol1, _LEDthrVec[i], _bl1);
	t2 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _pol2, _LEDthrVec[j], _bl2);
	
	iBin = _timeDiffMeanLEDthr->GetXaxis()->FindBin(_LEDthrVec[i]);
	jBin = _timeDiffMeanLEDthr->GetYaxis()->FindBin(_LEDthrVec[j]);
	
	_dtLEDthr[iBin][jBin].push_back(t2-t1);
      }
  
  return;
}

void TimingStudy::ProcessThrStudy()
{
  int nBins;
  double mean, stdDev, Emean, EstdDev;
  int xBin, yBin, zBin;
  double bestThr1, bestThr2;
  char title[200];
  
  //====================== CFD thresholds
  nBins = _timeDiffMeanCFDfrac->GetNbinsX() + 2;
  for(int i = 0; i < nBins; ++i)
    for(int j = 0; j < nBins; ++j){
      if(_dtCFDfrac[i][j].size() == 0) continue;
      CalcMeanStdDev(_dtCFDfrac[i][j], mean, stdDev, Emean, EstdDev);
      _timeDiffMeanCFDfrac->SetBinContent(i, j, mean);
      _timeDiffStdDevCFDfrac->SetBinContent(i, j, stdDev);
    }
  
  // fill distr of best threshold settings
  _timeDiffStdDevCFDfrac->GetMinimumBin(xBin, yBin, zBin);
  bestThr1 = _timeDiffStdDevCFDfrac->GetXaxis()->GetBinCenter(xBin);
  bestThr2 = _timeDiffStdDevCFDfrac->GetYaxis()->GetBinCenter(yBin);

  CalcMeanStdDev(_dtCFDfrac[xBin][yBin], mean, stdDev, Emean, EstdDev);
  sprintf(title, "CFD: Thr1 %.2F, Thr2 %.2F, #sigma = %.2F #pm %.2F ps, %i events", bestThr1, bestThr2, stdDev * 1e12, EstdDev * 1e12, (int) _dtCFDfrac[xBin][yBin].size());
  _bestCFDdistr->SetTitle(title);
  for(std::vector<double>::iterator it = _dtCFDfrac[xBin][yBin].begin(); it != _dtCFDfrac[xBin][yBin].end(); ++it)
    _bestCFDdistr->Fill(*it);

  //====================== LED thresholds
  nBins = _timeDiffMeanLEDthr->GetNbinsX() + 2;
  for(int i = 0; i < nBins; ++i)
    for(int j = 0; j < nBins; ++j){
      if(_dtLEDthr[i][j].size() == 0) continue;
      CalcMeanStdDev(_dtLEDthr[i][j], mean, stdDev, Emean, EstdDev);
      _neventsLEDthr->SetBinContent(i, j, _dtLEDthr[i][j].size());
      _timeDiffMeanLEDthr->SetBinContent(i, j, mean);
      _timeDiffStdDevLEDthr->SetBinContent(i, j, stdDev);
    }

  // fill distr of best threshold settings
  _timeDiffStdDevLEDthr->GetMinimumBin(xBin, yBin, zBin);
  bestThr1 = _timeDiffStdDevLEDthr->GetXaxis()->GetBinCenter(xBin);
  bestThr2 = _timeDiffStdDevLEDthr->GetYaxis()->GetBinCenter(yBin);

  CalcMeanStdDev(_dtLEDthr[xBin][yBin], mean, stdDev, Emean, EstdDev);
  sprintf(title, "LED: Thr1 %.0F mV, Thr2 %.0F mV, #sigma = %.2F #pm %.2F ps, %i events", bestThr1 * 1e3, bestThr2 * 1e3, stdDev * 1e12, EstdDev * 1e12, (int) _dtLEDthr[xBin][yBin].size());
  _bestLEDdistr->SetTitle(title);
  for(std::vector<double>::iterator it = _dtLEDthr[xBin][yBin].begin(); it != _dtLEDthr[xBin][yBin].end(); ++it)
    _bestLEDdistr->Fill(*it);

  return;
}

void TimingStudy::WriteThrStudy()
{
  _timeDiffMeanCFDfrac->Write();
  _timeDiffStdDevCFDfrac->Write();
  _bestCFDdistr->Write();

  _neventsLEDthr->Write();
  _timeDiffMeanLEDthr->Write();
  _timeDiffStdDevLEDthr->Write();
  _bestLEDdistr->Write();
  
  return;
}

