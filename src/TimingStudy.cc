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
  _supPulse1 = new TH2D("supPulse1", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 1000, -0.5, 2);
  _supPulse2 = new TH2D("supPulse2", ";Time [s];Voltage [V];Entries", 1000, 0, 100e-9, 1000, -0.5, 2);

  _noise1 = new TH1D("noise1", ";Voltage [V];Entries", 1000, -0.5, 0.5);
  _noise2 = new TH1D("noise2", ";Voltage [V];Entries", 1000, -0.5, 0.5);

  _baseline1 = new TH1D("baseline1", ";Voltage [V];Entries", 1000, -0.05, 0.05);
  _baseline2 = new TH1D("baseline2", ";Voltage [V];Entries", 1000, -0.05, 0.05);

  return;
}

TimingStudy::~TimingStudy()
{
  delete _supPulse1;
  delete _supPulse2;

  delete _noise1;
  delete _noise2;

  delete _baseline1;
  delete _baseline2;
  
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
      _noise1->Fill(_acl->_trace1[i] * _pol1);
    
    if(_acl->_time2[i] > _nbStart2 && _acl->_time2[i] < _nbStop2)
      _noise2->Fill(_acl->_trace2[i] * _pol2);

  }

  _bl1 = calcBaseline(_acl->_trace1, _acl->_time1, _acl->_npt, _nbStart1, _nbStop1);
  _bl1 *= _pol1;
  _baseline1->Fill(_bl1);

  _bl2 = calcBaseline(_acl->_trace2, _acl->_time2, _acl->_npt, _nbStart2, _nbStop2);
  _bl2 *= _pol2;
  _baseline2->Fill(_bl2);
  
  return;
}

void TimingStudy::Save()
{
  TDirectory* dir = _acl->_outFile->mkdir(_dirName.c_str());
  dir->cd();

  _supPulse1->Write();
  _supPulse2->Write();

  _noise1->Write();
  _noise2->Write();

  _baseline1->Write();
  _baseline2->Write();  
  
  return;
}

double TimingStudy::calcBaseline(Double_t* tra, Double_t* tim, Int_t n, double start, double stop)
{
  double sum = 0;
  int count = 0;
  for(int i = 0; i < n; ++i)
    if(tim[i] > start && tim[i] < stop){
      sum += tra[i];
      count++;
    }
 
  return sum/count;
}
