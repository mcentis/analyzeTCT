#include "SigNoiseVsV.h"

#include "TDirectory.h"

SigNoiseVsV::SigNoiseVsV(AnalysisClass* acl, const char* dirName)
  : AnalysisPrototype(acl, dirName)
{
  _ivGr = new TGraphErrors();
  _ivGr->SetName("IV");

  _ampli1BiasGr = new TGraphErrors();
  _ampli1BiasGr->SetName("ampli1bias");

  _ampli2BiasGr = new TGraphErrors();
  _ampli2BiasGr->SetName("ampli2bias");

  _inte1BiasGr = new TGraphErrors();
  _inte1BiasGr->SetName("inte1bias");

  _inte2BiasGr = new TGraphErrors();
  _inte2BiasGr->SetName("inte2bias");

  _noise1BiasGr = new TGraphErrors();
  _noise1BiasGr->SetName("noise1Bias");
  
  _noise2BiasGr = new TGraphErrors();
  _noise2BiasGr->SetName("noise2Bias");
  
  return;
}

SigNoiseVsV::~SigNoiseVsV()
{
  delete _ivGr;
  delete _ampli1BiasGr;
  delete _ampli2BiasGr;
  delete _inte1BiasGr;
  delete _inte2BiasGr;
  
  return;
}

void SigNoiseVsV::AnalysisAction()
{
  static bool firstCall = true;

  if(firstCall){
    NewSet();
    firstCall = false;
  }

  if(_stepVec.back() != _acl->_set){ // new set, store noise data
    NewSet();

    // noise from previous step
    _noise1Vec.push_back(_acl->_noise1);
    _noise2Vec.push_back(_acl->_noise2);
  }

  _biasVec.back()->push_back(_acl->_biasMeas);
  _currVec.back()->push_back(_acl->_current);
  _ampli1Vec.back()->push_back(_acl->_ampli1);
  _ampli2Vec.back()->push_back(_acl->_ampli2);
  _inte1Vec.back()->push_back(_acl->_inte1);
  _inte2Vec.back()->push_back(_acl->_inte2);
  
  return;
}

void SigNoiseVsV::Save(TDirectory* parent)
{
  TDirectory* dir = parent->mkdir(_dirName.c_str());
  dir->cd();

  const char* xtitle = "Bias [V]";
  
  PutAxisLabels(_ivGr, xtitle, "Current [A]");
  _ivGr->Write();

  PutAxisLabels(_ampli1BiasGr, xtitle, "Amplitude [V]");
  _ampli1BiasGr->Write();

  PutAxisLabels(_ampli2BiasGr, xtitle, "Amplitude [V]");
  _ampli2BiasGr->Write();

  PutAxisLabels(_inte1BiasGr, xtitle, "Integral [Vs]");
  _inte1BiasGr->Write();

  PutAxisLabels(_inte2BiasGr, xtitle, "Integral [Vs]");
  _inte2BiasGr->Write();

  PutAxisLabels(_noise1BiasGr, xtitle, "Noise [mV]");
  _noise1BiasGr->Write();

  PutAxisLabels(_noise2BiasGr, xtitle, "Noise [mV]");
  _noise2BiasGr->Write();

  return;
}

void SigNoiseVsV::Process()
{
  // get noise from last step
  _noise1Vec.push_back(_acl->_noise1);
  _noise2Vec.push_back(_acl->_noise2);

  double mean, Emean, sigma, Esigma;
  double bias, Ebias;
  
  for(unsigned int i = 0; i < _stepVec.size(); ++i){
    CalcMeanStdDev(*_biasVec.at(i), mean, sigma, Emean, Esigma);

    bias = mean;
    Ebias = Emean;

    SetPointGr(*_currVec.at(i), _ivGr, i, bias, Ebias);
    SetPointGr(*_ampli1Vec.at(i), _ampli1BiasGr, i, bias, Ebias);
    SetPointGr(*_ampli2Vec.at(i), _ampli2BiasGr, i, bias, Ebias);
    SetPointGr(*_inte1Vec.at(i), _inte1BiasGr, i, bias, Ebias);
    SetPointGr(*_inte2Vec.at(i), _inte2BiasGr, i, bias, Ebias);

    _noise1BiasGr->SetPoint(i, fabs(bias), _noise1Vec.at(i) * 1e3);
    _noise2BiasGr->SetPoint(i, fabs(bias), _noise2Vec.at(i) * 1e3);
  }

  return;
}

void SigNoiseVsV::SetPointGr(std::vector<double> vec, TGraphErrors* gr, unsigned int npt, double bias, double Ebias)
{
  double mean, sigma, Emean, Esigma;
  CalcMeanStdDev(vec, mean, sigma, Emean, Esigma);
  gr->SetPoint(npt, fabs(bias), fabs(mean));
  gr->SetPointError(npt, Ebias, Emean);

  return;
}

void SigNoiseVsV::NewSet()
{
  _stepVec.push_back(_acl->_set);

  _biasVec.push_back(new std::vector<double>);
  _currVec.push_back(new std::vector<double>);
  _ampli1Vec.push_back(new std::vector<double>);
  _ampli2Vec.push_back(new std::vector<double>);
  _inte1Vec.push_back(new std::vector<double>);
  _inte2Vec.push_back(new std::vector<double>);
  
  return;
}
