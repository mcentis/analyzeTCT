#include "TemperatureTime.h"

#include "TDirectory.h"
#include "TCanvas.h"
#include "TAxis.h"

TemperatureTime::TemperatureTime(AnalysisClass* acl, const char* dirName)
  : AnalysisPrototype(acl, dirName)
{
  _chillerSetTtime = new TGraph();
  _chillerSetTtime->SetName("chillerSetTtime");
  
 _chillerInternTime = new TGraph();
 _chillerInternTime->SetName("chillerInternTime");
 
 _chillerExternTime = new TGraph();
 _chillerExternTime->SetName("chillerExternTime");
 
 _peltierSetTtime = new TGraph();
 _peltierSetTtime->SetName("peltierSetTtime");
 
 _pcbTtime = new TGraph();
 _pcbTtime->SetName("_pcbTtime");
 
 _copperTtime = new TGraph();
 _copperTtime->SetName("_copperTtime");

  return;
}

TemperatureTime::~TemperatureTime()
{
  delete _chillerSetTtime;
  delete _chillerInternTime;
  delete _chillerExternTime;
  delete _peltierSetTtime;
  delete _pcbTtime;
  delete _copperTtime;
    
  return;
}

void TemperatureTime::AnalysisAction()
{
  _chillerSetTtime->SetPoint(_acl->_event, _acl->_timeStamp, _acl->_chillerSetT);
  _chillerInternTime->SetPoint(_acl->_event, _acl->_timeStamp, _acl->_chillerIntern);
  _chillerExternTime->SetPoint(_acl->_event, _acl->_timeStamp, _acl->_chillerExtern);
  _peltierSetTtime->SetPoint(_acl->_event, _acl->_timeStamp, _acl->_peltierSetT);
  _pcbTtime->SetPoint(_acl->_event, _acl->_timeStamp, _acl->_pcbT);
  _copperTtime->SetPoint(_acl->_event, _acl->_timeStamp, _acl->_copperT);
  
  return;
}

void TemperatureTime::Save()
{
  PutAxisLabels(_chillerSetTtime);
  PutAxisLabels(_chillerInternTime);
  PutAxisLabels(_chillerExternTime);
  PutAxisLabels(_peltierSetTtime);
  PutAxisLabels(_pcbTtime);
  PutAxisLabels(_copperTtime);
  
  TDirectory* dir = _acl->_outFile->mkdir(_dirName.c_str());
  dir->cd();

  _chillerSetTtime->Write();
  _chillerInternTime->Write();
  _chillerExternTime->Write();
  _peltierSetTtime->Write();
  _pcbTtime->Write();
  _copperTtime->Write();
  
  
  return;
}

void TemperatureTime::PutAxisLabels(TGraph* gr)
{
  TCanvas* can = new TCanvas();
  gr->Draw("apl");
  gr->GetXaxis()->SetTitle("Time stamp");
  gr->GetYaxis()->SetTitle("Temperature [C]");
  delete can;
  
  return;
}
