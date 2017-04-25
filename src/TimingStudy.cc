#include "TimingStudy.h"

#include "iostream"

#include "TDirectory.h"

TimingStudy::TimingStudy(AnalysisClass* acl, const char* dirName)
  : AnalysisPrototype(acl, dirName)
{
  //======================================== histos =============================================
  _supPulse1 = new TH2D("supPulse1", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 2000, -0.15, 1.15);
  _supPulse2 = new TH2D("supPulse2", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 2000, -0.15, 1.15);

  _noiseDistr1 = new TH1D("noise1", ";Voltage [V];Entries", 1000, -0.15, 0.15);
  _noiseDistr2 = new TH1D("noise2", ";Voltage [V];Entries", 1000, -0.15, 0.15);

  _baselineDistr1 = new TH1D("baseline1", ";Voltage [V];Entries", 1000, -0.015, 0.015);
  _baselineDistr2 = new TH1D("baseline2", ";Voltage [V];Entries", 1000, -0.015, 0.015);

  _ampliDistr1 = new TH1D("amplitude1", ";Voltage [V];Entries", 1000, -0.1, 1.15);
  _ampliDistr2 = new TH1D("amplitude2", ";Voltage [V];Entries", 1000, -0.1, 1.15);

  _riseTime2080_1 = new TH1D("riseTime2080_1", ";Rise time 20% 80% [s];Entries", 1000, 0, 2e-9);
  _riseTime2080_2 = new TH1D("riseTime2080_2", ";Rise time 20% 80% [s];Entries", 1000, 0, 2e-9);

  _riseTimeAmpli1 = new TH2D("riseTimeAmpli1", ";Amplitude [V];Rise time 20% 80% [s];Entries", 1000, -0.1, 1.15, 1000, 0, 2e-9);
  _riseTimeAmpli2 = new TH2D("riseTimeAmpli2", ";Amplitude [V];Rise time 20% 80% [s];Entries", 1000, -0.1, 1.15, 1000, 0, 2e-9);

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

  delete _riseTime2080_1;
  delete _riseTime2080_2;

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
  //if(_acl->_posCutPassed == false) // position cut
  //  return;
  
  for(int i = 0; i < _acl->_npt; ++i){
    _supPulse1->Fill(_acl->_time1[i], _acl->_trace1[i] * _acl->_pol1);
    _supPulse2->Fill(_acl->_time2[i], _acl->_trace2[i] * _acl->_pol2);

    if(_acl->_time1[i] > _acl->_nbStart1 && _acl->_time1[i] < _acl->_nbStop1)
      _noiseDistr1->Fill(_acl->_trace1[i] * _acl->_pol1);
    
    if(_acl->_time2[i] > _acl->_nbStart2 && _acl->_time2[i] < _acl->_nbStop2)
      _noiseDistr2->Fill(_acl->_trace2[i] * _acl->_pol2);
  }

  _baselineDistr1->Fill(_acl->_bl1);

  _baselineDistr2->Fill(_acl->_bl2);

  _ampliDistr1->Fill(_acl->_ampli1);
  
  _ampliDistr2->Fill(_acl->_ampli2);

  FillRiseTime_RiseTimeAmpli();

  FillThrStudy();
  
  return;
}

void TimingStudy::Save(TDirectory* parent)
{  
  TDirectory* dir = parent->mkdir(_dirName.c_str());
  dir->cd();

  _supPulse1->Write();
  _supPulse2->Write();

  _noiseDistr1->Write();
  _noiseDistr2->Write();

  _baselineDistr1->Write();
  _baselineDistr2->Write();  

  _ampliDistr1->Write();
  _ampliDistr2->Write();

  _riseTime2080_1->Write();
  _riseTime2080_2->Write();
  
  _riseTimeAmpli1->Write();
  _riseTimeAmpli2->Write();

  WriteThrStudy();
  
  return;
}

void TimingStudy::Process()
{
  ProcessThrStudy();

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

  t1 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _acl->_pol1, 0.2 * _acl->_ampli1, _acl->_bl1);
  t2 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _acl->_pol1, 0.8 * _acl->_ampli1, _acl->_bl1);

  _riseTime2080_1->Fill(t2-t1);
  _riseTimeAmpli1->Fill(_acl->_ampli1, t2-t1);
  
  t1 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _acl->_pol2, 0.2 * _acl->_ampli2, _acl->_bl2);
  t2 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _acl->_pol2, 0.8 * _acl->_ampli2, _acl->_bl2);

  _riseTime2080_2->Fill(t2-t1);
  _riseTimeAmpli2->Fill(_acl->_ampli2, t2-t1);

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

  //binning with two ranges: 0.5 to 10 mV in 1 mV step, the rest in 20 mV step
  const int thrBins1 = 9;
  double thrMin1 = 0.0005;
  double thrMax1 = 0.0095;
  const int thrBins2 = 25;
  double thrMin2 = 0.01;
  double thrMax2 = 0.51;

  const int totThrBins = thrBins1 + thrBins2;
  double binArray[totThrBins + 1]; // array with the low edges of the bins (+1 to have upper edge of last bin)
  int iBin = 0;

  for(; iBin < thrBins1; ++iBin)
    binArray[iBin] = thrMin1 + (thrMax1 - thrMin1) / thrBins1 * iBin;
  
  for(; iBin < totThrBins + 1; ++iBin)
    binArray[iBin] = thrMin2 + (thrMax2 - thrMin2) / thrBins2 * (iBin - thrBins1);

  // for(int i = 0; i < totThrBins + 1; ++i)
  //   std::cout << binArray[i] << std::endl;
  
  _neventsLEDthr = new TH2D("neventsLEDthr", ";Thr 1 [V];Thr 2 [V];Entries", totThrBins, binArray, totThrBins, binArray);
  _timeDiffMeanLEDthr = new TH2D("timeDiffMeanLEDthr", ";Thr 1 [V];Thr 2 [V];Mean #Delta t [s]", totThrBins, binArray, totThrBins, binArray);
  _timeDiffStdDevLEDthr = new TH2D("timeDiffStdDevLEDthr", ";Thr 1 [V];Thr 2 [V];Std Dev #Delta t [s]", totThrBins, binArray, totThrBins, binArray);

  
  for(int i = 0; i < _timeDiffMeanLEDthr->GetNbinsX(); ++i)
    _LEDthrVec.push_back(_timeDiffMeanLEDthr->GetXaxis()->GetBinCenter(i+1)); // threshold vector

  _dtLEDthr = new std::vector<double>*[totThrBins + 2]; // underflow and overflow bins
  for(int i = 0; i < totThrBins + 2; ++i)
    _dtLEDthr[i] = new std::vector<double>[totThrBins + 2];

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
      t1 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _acl->_pol1, _CFDfracVec[i] * _acl->_ampli1, _acl->_bl1);
      t2 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _acl->_pol2, _CFDfracVec[j] * _acl->_ampli2, _acl->_bl2);
      
      iBin = _timeDiffMeanCFDfrac->GetXaxis()->FindBin(_CFDfracVec[i]);
      jBin = _timeDiffMeanCFDfrac->GetYaxis()->FindBin(_CFDfracVec[j]);
      
      _dtCFDfrac[iBin][jBin].push_back(t2-t1);
    }
  
  //======================= LED thresholds
  for(unsigned int i = 0; i < _LEDthrVec.size(); ++i)
    for(unsigned int j = 0; j < _LEDthrVec.size(); ++j)
      if(_acl->_ampli1 > _LEDthrVec[i] * 1.05 && _acl->_ampli2 > _LEDthrVec[j] * 1.05){ // the maximum is at least 5% bigger than the threshold
	t1 = CalcTimeThrLinear2pt(_acl->_trace1, _acl->_time1, _acl->_npt, _acl->_pol1, _LEDthrVec[i], _acl->_bl1);
	t2 = CalcTimeThrLinear2pt(_acl->_trace2, _acl->_time2, _acl->_npt, _acl->_pol2, _LEDthrVec[j], _acl->_bl2);
	
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
  int xBin, yBin;
  double bestThr1, bestThr2;
  double minStdDev;
  char title[200];

  //====================== CFD thresholds
  minStdDev = 5e5;
  xBin = yBin = 0;
  nBins = _timeDiffMeanCFDfrac->GetNbinsX() + 2;
  for(int i = 0; i < nBins; ++i)
    for(int j = 0; j < nBins; ++j){
      if(_dtCFDfrac[i][j].size() == 0) continue;
      CalcMeanStdDev(_dtCFDfrac[i][j], mean, stdDev, Emean, EstdDev);
      _timeDiffMeanCFDfrac->SetBinContent(i, j, mean);
      _timeDiffStdDevCFDfrac->SetBinContent(i, j, stdDev);

      if(stdDev != 0 && stdDev < minStdDev){ // find best resolution excluding 0s
	minStdDev = stdDev;
	xBin = i;
	yBin = j;
      }
    }
  
  // fill distr of best threshold settings
  bestThr1 = _timeDiffStdDevCFDfrac->GetXaxis()->GetBinCenter(xBin);
  bestThr2 = _timeDiffStdDevCFDfrac->GetYaxis()->GetBinCenter(yBin);

  CalcMeanStdDev(_dtCFDfrac[xBin][yBin], mean, stdDev, Emean, EstdDev);
  sprintf(title, "CFD: Thr1 %.2F, Thr2 %.2F, #sigma = %.2F #pm %.2F ps, %i events", bestThr1, bestThr2, stdDev * 1e12, EstdDev * 1e12, (int) _dtCFDfrac[xBin][yBin].size());
  _bestCFDdistr->SetTitle(title);
  for(std::vector<double>::iterator it = _dtCFDfrac[xBin][yBin].begin(); it != _dtCFDfrac[xBin][yBin].end(); ++it)
    _bestCFDdistr->Fill(*it);

  //====================== LED thresholds
  minStdDev = 5e5;
  xBin = yBin = 0;
  nBins = _timeDiffMeanLEDthr->GetNbinsX() + 2;
  for(int i = 0; i < nBins; ++i)
    for(int j = 0; j < nBins; ++j){
      if(_dtLEDthr[i][j].size() == 0) continue;
      CalcMeanStdDev(_dtLEDthr[i][j], mean, stdDev, Emean, EstdDev);
      _neventsLEDthr->SetBinContent(i, j, _dtLEDthr[i][j].size());
      _timeDiffMeanLEDthr->SetBinContent(i, j, mean);
      _timeDiffStdDevLEDthr->SetBinContent(i, j, stdDev);

      if(stdDev != 0 && stdDev < minStdDev){ // find best resolution excluding 0s
	minStdDev = stdDev;
	xBin = i;
	yBin = j;
      }
    }

  // fill distr of best threshold settings
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

