#include "TimingStudy.h"

#include "iostream"

#include "TDirectory.h"

TimingStudy::TimingStudy(AnalysisClass* acl, const char* dirName)
  : AnalysisPrototype(acl, dirName)
{
  //======================================== histos =============================================
  _supPulse1 = new TH2D("supPulse1", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 2000, -0.15, 1.15);
  _supPulse2 = new TH2D("supPulse2", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 2000, -0.15, 1.15);

  _rawNoiseDistr1 = new TH1D("rawNoise1", ";Voltage [V];Entries", 1000, -0.15, 0.15);
  _rawNoiseDistr2 = new TH1D("rawNoise2", ";Voltage [V];Entries", 1000, -0.15, 0.15);

  _noiseDistr1 = new TH1D("noise1", ";Voltage [V];Entries", 1000, -0.15, 0.15);
  _noiseDistr2 = new TH1D("noise2", ";Voltage [V];Entries", 1000, -0.15, 0.15);

  _baselineDistr1 = new TH1D("baseline1", ";Voltage [V];Entries", 1000, -0.015, 0.015);
  _baselineDistr2 = new TH1D("baseline2", ";Voltage [V];Entries", 1000, -0.015, 0.015);

  _ampliDistr1 = new TH1D("amplitude1", ";Voltage [V];Entries", 1000, -0.1, 1.15);
  _ampliDistr2 = new TH1D("amplitude2", ";Voltage [V];Entries", 1000, -0.1, 1.15);

  _riseTime2080_1 = new TH1D("riseTime2080_1", ";Rise time 20% 80% [s];Entries", 5000, 0, 10e-9);
  _riseTime2080_2 = new TH1D("riseTime2080_2", ";Rise time 20% 80% [s];Entries", 5000, 0, 10e-9);

  _riseTimeAmpli1 = new TH2D("riseTimeAmpli1", ";Amplitude [V];Rise time 20% 80% [s];Entries", 1000, -0.1, 1.15, 1000, 0, 2e-9);
  _riseTimeAmpli2 = new TH2D("riseTimeAmpli2", ";Amplitude [V];Rise time 20% 80% [s];Entries", 1000, -0.1, 1.15, 1000, 0, 2e-9);

  InitThrStudy();
  
  return;
}

TimingStudy::~TimingStudy()
{
  delete _supPulse1;
  delete _supPulse2;

  delete _rawNoiseDistr1;
  delete _rawNoiseDistr2;
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

    if(_acl->_time1[i] > _acl->_nbStart1 && _acl->_time1[i] < _acl->_nbStop1){
      _rawNoiseDistr1->Fill(_acl->_trace1[i] * _acl->_pol1);
      _noiseDistr1->Fill(_acl->_trace1[i] * _acl->_pol1 - _acl->_bl1);
    }
    
    if(_acl->_time2[i] > _acl->_nbStart2 && _acl->_time2[i] < _acl->_nbStop2){
      _rawNoiseDistr2->Fill(_acl->_trace2[i] * _acl->_pol2);
      _noiseDistr2->Fill(_acl->_trace2[i] * _acl->_pol2 - _acl->_bl2);
    }
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

  _rawNoiseDistr1->Write();
  _rawNoiseDistr2->Write();

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

  _bestCFDdistr = new TH1D("bestCFDdistr", ";#Delta t [s];Events", 16000, -10e-9, 10e-9);

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

  _bestLEDdistr = new TH1D("bestLEDdistr", ";#Delta t [s];Events", 16000, -10e-9, 10e-9);
  
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
      if(_dtCFDfrac[i][j].size() <= 4) continue; // avoid warnings from CalcMeanStdDev. 4 is the minimum to calculate all quantities
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
  
  // fill variables
  _bestThr1CFD = bestThr1;
  _bestThr2CFD = bestThr2;
  _bestMeanCFD = mean;
  _errBestMeanCFD = Emean;
  _bestStdDevCFD = stdDev;
  _errBestStdDevCFD = EstdDev;
  
  //====================== LED thresholds
  minStdDev = 5e5;
  xBin = yBin = 0;
  nBins = _timeDiffMeanLEDthr->GetNbinsX() + 2;
  for(int i = 0; i < nBins; ++i)
    for(int j = 0; j < nBins; ++j){
      if(_dtLEDthr[i][j].size() <= 4) continue; // avoid warnings from CalcMeanStdDev. 4 is the minimum to calculate all quantities
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

  // fill variables
  _bestThr1LED = bestThr1;
  _bestThr2LED = bestThr2;
  _bestMeanLED = mean;
  _errBestMeanLED = Emean;
  _bestStdDevLED = stdDev;
  _errBestStdDevLED = EstdDev;
  
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

std::vector<double> TimingStudy::GetBestCFDvalues() // in order: thr1, thr2, mean, err on mean, stdDev, err on stdDev
{
  std::vector<double> ret;
  ret.push_back(_bestThr1CFD);
  ret.push_back(_bestThr2CFD);
  ret.push_back(_bestMeanCFD);
  ret.push_back(_errBestMeanCFD);
  ret.push_back(_bestStdDevCFD);
  ret.push_back(_errBestStdDevCFD);

  return ret;
}

std::vector<double> TimingStudy::GetBestLEDvalues()
{
  std::vector<double> ret;
  ret.push_back(_bestThr1LED);
  ret.push_back(_bestThr2LED);
  ret.push_back(_bestMeanLED);
  ret.push_back(_errBestMeanLED);
  ret.push_back(_bestStdDevLED);
  ret.push_back(_errBestStdDevLED);

  return ret;
}

std::vector<double> TimingStudy::GetRiseTimes() // rise time 20% 80% risetime1, error1, risetime2, error2
{
  std::vector<double> ret;
  ret.push_back(_riseTime2080_1->GetMean());
  ret.push_back(_riseTime2080_1->GetMeanError());
  ret.push_back(_riseTime2080_2->GetMean());
  ret.push_back(_riseTime2080_2->GetMeanError());

  return ret;
}

std::vector<double> TimingStudy::GetNoise1() // noise and noise error (error on sigma assuming gaussian distribution)
{
  std::vector<double> ret;
  ret.push_back(_noiseDistr1->GetStdDev());
  ret.push_back(_noiseDistr1->GetStdDevError());
  
  return ret;
}

std::vector<double> TimingStudy::GetNoise2()
{
  std::vector<double> ret;
  ret.push_back(_noiseDistr2->GetStdDev());
  ret.push_back(_noiseDistr2->GetStdDevError());
  
  return ret;
}
