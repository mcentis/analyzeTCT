#include "TimingVsV.h"

#include "TDirectory.h"

TimingVsV::TimingVsV(AnalysisClass* acl, const char* dirName)
  : AnalysisPrototype(acl, dirName)
{
  _thr1CFDGr = new TGraphErrors();
  _thr1CFDGr->SetName("thr1CFD");

  _thr2CFDGr = new TGraphErrors();
  _thr2CFDGr->SetName("thr2CFD");

  _meanCFDGr = new TGraphErrors();
  _meanCFDGr->SetName("meanCFD");

  _stdDevCFDGr = new TGraphErrors();
  _stdDevCFDGr->SetName("stdDevCFD");

  _thr1LEDGr = new TGraphErrors();
  _thr1LEDGr->SetName("thr1LED");

  _thr2LEDGr = new TGraphErrors();
  _thr2LEDGr->SetName("thr2LED");

  _meanLEDGr = new TGraphErrors();
  _meanLEDGr->SetName("meanLED");

  _stdDevLEDGr = new TGraphErrors();
  _stdDevLEDGr->SetName("stdDevLED");

  _riseTime1Gr = new TGraphErrors();
  _riseTime1Gr->SetName("riseTime1");
  
  _riseTime2Gr = new TGraphErrors();
  _riseTime2Gr->SetName("riseTime2");
  
  return;
}

TimingVsV::~TimingVsV()
{
  delete _thr1CFDGr;
  delete _thr2CFDGr;
  delete _meanCFDGr;
  delete _stdDevCFDGr;

  delete _thr1LEDGr;
  delete _thr2LEDGr;
  delete _meanLEDGr;
  delete _stdDevLEDGr;

  delete _riseTime1Gr;
  delete _riseTime2Gr;

  return;
}

void TimingVsV::AnalysisAction()
{
  static bool firstCall = true;

  if(firstCall){
    NewSet();
    firstCall = false;
  }

  if(_stepVec.back() != _acl->_set){ // new set, store noise data
    NewSet();

    // data from previous step
    _CFDVec.push_back(_acl->_bestResCFD);
    _LEDVec.push_back(_acl->_bestResLED);
    _riseTimeVec.push_back(_acl->_riseTimeStep);
  }

  _biasVec.back()->push_back(_acl->_biasMeas);

  return;
}

void TimingVsV::Save(TDirectory* parent)
{
  TDirectory* dir = parent->mkdir(_dirName.c_str());
  dir->cd();

  const char* xtitle = "Bias [V]";
  
  PutAxisLabels(_thr1CFDGr, xtitle, "Frac 1");
  _thr1CFDGr->Write();

  PutAxisLabels(_thr2CFDGr, xtitle, "Frac 1");
  _thr2CFDGr->Write();

  PutAxisLabels(_meanCFDGr, xtitle, "Mean [s]");
  _meanCFDGr->Write();

  PutAxisLabels(_stdDevCFDGr, xtitle, "Std. Dev. [s]");
  _stdDevCFDGr->Write();

  PutAxisLabels(_thr1LEDGr, xtitle, "Thr. 1 [mV]");
  _thr1LEDGr->Write();

  PutAxisLabels(_thr2LEDGr, xtitle, "Thr. 2 [mV]");
  _thr2LEDGr->Write();

  PutAxisLabels(_meanLEDGr, xtitle, "Mean [s]");
  _meanLEDGr->Write();

  PutAxisLabels(_stdDevLEDGr, xtitle, "Std. Dev. [s]");
  _stdDevLEDGr->Write();

  PutAxisLabels(_riseTime1Gr, xtitle, "Rise Time 20% 80% [s]");
  _riseTime1Gr->Write();

  PutAxisLabels(_riseTime2Gr, xtitle, "Rise Time 20% 80% [s]");
  _riseTime2Gr->Write();

  return;
}

void TimingVsV::Process()
{
  // get data from last step
  _CFDVec.push_back(_acl->_bestResCFD);
  _LEDVec.push_back(_acl->_bestResLED);
  _riseTimeVec.push_back(_acl->_riseTimeStep);
    
  double mean, Emean, sigma, Esigma;
  double bias, Ebias;
  
  for(unsigned int i = 0; i < _stepVec.size(); ++i){
    CalcMeanStdDev(*_biasVec.at(i), mean, sigma, Emean, Esigma);

    bias = mean;
    Ebias = Emean;

    //=================== vectors with data: thr1, thr2, mean, err on mean, stdDev, err on stdDev
    _thr1CFDGr->SetPoint(i, fabs(bias), _CFDVec.at(i)[0]);
    _thr2CFDGr->SetPoint(i, fabs(bias), _CFDVec.at(i)[1]);

    _meanCFDGr->SetPoint(i, fabs(bias), _CFDVec.at(i)[2]);
    _meanCFDGr->SetPointError(i, Ebias, _CFDVec.at(i)[3]);

    _stdDevCFDGr->SetPoint(i, fabs(bias), _CFDVec.at(i)[4]);
    _stdDevCFDGr->SetPointError(i, Ebias, _CFDVec.at(i)[5]);

    _thr1LEDGr->SetPoint(i, fabs(bias), _LEDVec.at(i)[0] * 1e3);
    _thr2LEDGr->SetPoint(i, fabs(bias), _LEDVec.at(i)[1] * 1e3);

    _meanLEDGr->SetPoint(i, fabs(bias), _LEDVec.at(i)[2]);
    _meanLEDGr->SetPointError(i, Ebias, _LEDVec.at(i)[3]);

    _stdDevLEDGr->SetPoint(i, fabs(bias), _LEDVec.at(i)[4]);
    _stdDevLEDGr->SetPointError(i, Ebias, _LEDVec.at(i)[5]);

    // =============== rise time: rise time 1, error 1, rise time 1, error 2
    _riseTime1Gr->SetPoint(i, fabs(bias), _riseTimeVec.at(i)[0]);
    _riseTime1Gr->SetPointError(i, Ebias, _riseTimeVec.at(i)[1]);

    _riseTime2Gr->SetPoint(i, fabs(bias), _riseTimeVec.at(i)[2]);
    _riseTime2Gr->SetPointError(i, Ebias, _riseTimeVec.at(i)[3]);
    
  }

  return;
}

void TimingVsV::NewSet()
{
  _stepVec.push_back(_acl->_set);

  _biasVec.push_back(new std::vector<double>);
  
  return;
}
