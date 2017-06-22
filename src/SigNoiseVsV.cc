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
  _noise1BiasGr->SetName("noise1bias");
  
  _noise2BiasGr = new TGraphErrors();
  _noise2BiasGr->SetName("noise2bias");

  _snr1BiasGr = new TGraphErrors();
  _snr1BiasGr->SetName("snr1bias");

  _snr2BiasGr = new TGraphErrors();
  _snr2BiasGr->SetName("snr2bias");

  return;
}

SigNoiseVsV::~SigNoiseVsV()
{
  delete _ivGr;
  delete _ampli1BiasGr;
  delete _ampli2BiasGr;
  delete _inte1BiasGr;
  delete _inte2BiasGr;
  delete _noise1BiasGr;
  delete _noise2BiasGr;
  delete _snr1BiasGr;
  delete _snr2BiasGr;
  
  return;
}

void SigNoiseVsV::AnalysisAction()
{
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
  
  PutAxisLabels(_ivGr, xtitle, "Current [mA]");
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

  PutAxisLabels(_snr1BiasGr, xtitle, "SNR");
  _snr1BiasGr->Write();

  PutAxisLabels(_snr2BiasGr, xtitle, "SNR");
  _snr2BiasGr->Write();

  return;
}

void SigNoiseVsV::Process()
{
  // get noise from last step
  _noise1Vec.push_back(_acl->_noise1);
  _noise2Vec.push_back(_acl->_noise2);

  double mean, Emean, sigma, Esigma;
  double bias, Ebias;
  
  for(unsigned int i = 0; i < _biasVec.size(); ++i){
    CalcMeanStdDev(*_biasVec.at(i), mean, sigma, Emean, Esigma);

    bias = mean;
    Ebias = Emean;

    SetPointGr(*_currVec.at(i), _ivGr, i, bias, Ebias);
    SetPointGr(*_ampli1Vec.at(i), _ampli1BiasGr, i, bias, Ebias);
    SetPointGr(*_ampli2Vec.at(i), _ampli2BiasGr, i, bias, Ebias);
    SetPointGr(*_inte1Vec.at(i), _inte1BiasGr, i, bias, Ebias);
    SetPointGr(*_inte2Vec.at(i), _inte2BiasGr, i, bias, Ebias);

    _noise1BiasGr->SetPoint(i, fabs(bias), _noise1Vec.at(i)[0] * 1e3);
    _noise1BiasGr->SetPointError(i, Ebias, _noise1Vec.at(i)[1] * 1e3);
    
    _noise2BiasGr->SetPoint(i, fabs(bias), _noise2Vec.at(i)[0] * 1e3);
    _noise2BiasGr->SetPointError(i, Ebias, _noise2Vec.at(i)[1] * 1e3);
  }

  // snr graphs
  FillSNRgraph(_snr1BiasGr, _ampli1BiasGr, _noise1BiasGr);
  FillSNRgraph(_snr2BiasGr, _ampli2BiasGr, _noise2BiasGr);

  return;
}

void SigNoiseVsV::FillSNRgraph(TGraphErrors* snrGr, TGraphErrors* ampliGr, TGraphErrors* noiseGr)
{
  double snr, Esnr;

  int npt = ampliGr->GetN();

  double *bias = ampliGr->GetX();
  double *Ebias = ampliGr->GetEX();
  
  double *sig = ampliGr->GetY();
  double *Esig = ampliGr->GetEY();

  double *noise = noiseGr->GetY();
  double *Enoise = noiseGr->GetEY();

  for(int i = 0; i < npt; i++){
    snr = sig[i]/(noise[i]/1e3); // noise is in mV
    Esnr = sqrt(pow(Esig[i]/sig[i], 2) + pow(Enoise[i]/noise[i], 2)) * snr;

    snrGr->SetPoint(i, bias[i], snr);
    snrGr->SetPointError(i, Ebias[i], Esnr);
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
  static bool firstCall = true;

  _biasVec.push_back(new std::vector<double>);
  _currVec.push_back(new std::vector<double>);
  _ampli1Vec.push_back(new std::vector<double>);
  _ampli2Vec.push_back(new std::vector<double>);
  _inte1Vec.push_back(new std::vector<double>);
  _inte2Vec.push_back(new std::vector<double>);
  
  if(firstCall){ // avoid looking for previous step if it is the first step
    firstCall = false;
    return;
  }
  // noise from previous step
  _noise1Vec.push_back(_acl->_noise1);
  _noise2Vec.push_back(_acl->_noise2);

  
  return;
}
