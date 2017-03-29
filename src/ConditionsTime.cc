#include "ConditionsTime.h"

#include "TDirectory.h"
#include "TCanvas.h"
#include "TAxis.h"

ConditionsTime::ConditionsTime(AnalysisClass* acl, const char* dirName)
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
 _pcbTtime->SetName("pcbTtime");
 
 _copperTtime = new TGraph();
 _copperTtime->SetName("copperTtime");

 _xTime = new TGraph();
 _xTime->SetName("xTime");

 _yTime = new TGraph();
 _yTime->SetName("yTime");

 _zTime = new TGraph();
 _zTime->SetName("zTime");

 _ampliVtime = new TGraph();
 _ampliVtime->SetName("ampliVtime");

 _ampliItime = new TGraph();
 _ampliItime->SetName("ampliItime");

 return;
}

ConditionsTime::~ConditionsTime()
{
  delete _chillerSetTtime;
  delete _chillerInternTime;
  delete _chillerExternTime;
  delete _peltierSetTtime;
  delete _pcbTtime;
  delete _copperTtime;
    
  return;
}

void ConditionsTime::AnalysisAction()
{
  double time = _acl->_timeStamp;
  
  _chillerSetTtime->SetPoint(_acl->_event, time, _acl->_chillerSetT);
  _chillerInternTime->SetPoint(_acl->_event, time, _acl->_chillerIntern);
  _chillerExternTime->SetPoint(_acl->_event, time, _acl->_chillerExtern);
  _peltierSetTtime->SetPoint(_acl->_event, time, _acl->_peltierSetT);
  _pcbTtime->SetPoint(_acl->_event, time, _acl->_pcbT);
  _copperTtime->SetPoint(_acl->_event, time, _acl->_copperT);

  _xTime->SetPoint(_acl->_event, time, _acl->_x);
  _yTime->SetPoint(_acl->_event, time, _acl->_y);
  _zTime->SetPoint(_acl->_event, time, _acl->_z);

  _ampliVtime->SetPoint(_acl->_event, time, _acl->_ampliV);
  _ampliItime->SetPoint(_acl->_event, time, _acl->_ampliI);

  return;
}

void ConditionsTime::Save()
{
  const char xtitle[20] = "Time stamp";

  PutAxisLabels(_chillerSetTtime, xtitle, "Temperature [C]");
  PutAxisLabels(_chillerInternTime, xtitle, "Temperature [C]");
  PutAxisLabels(_chillerExternTime, xtitle, "Temperature [C]");
  PutAxisLabels(_peltierSetTtime, xtitle, "Temperature [C]");
  PutAxisLabels(_pcbTtime, xtitle, "Temperature [C]");
  PutAxisLabels(_copperTtime, xtitle, "Temperature [C]");

  PutAxisLabels(_xTime, xtitle, "Position [mm]");
  PutAxisLabels(_yTime, xtitle, "Position [mm]");
  PutAxisLabels(_zTime, xtitle, "Position [mm]");

  PutAxisLabels(_ampliVtime, xtitle, "Voltage [V]");
  PutAxisLabels(_ampliItime, xtitle, "Current [A]");
    
  TDirectory* dir = _acl->_outFile->mkdir(_dirName.c_str());
  dir->cd();

  _chillerSetTtime->Write();
  _chillerInternTime->Write();
  _chillerExternTime->Write();
  _peltierSetTtime->Write();
  _pcbTtime->Write();
  _copperTtime->Write();

  _xTime->Write();
  _yTime->Write();
  _zTime->Write();

  _ampliVtime->Write();
  _ampliItime->Write();
  
  return;
}

void ConditionsTime::PutAxisLabels(TGraph* gr, const char* xtitle, const char* ytitle)
{
  TCanvas* can = new TCanvas();
  gr->Draw("apl");
  gr->GetXaxis()->SetTitle(xtitle);
  gr->GetYaxis()->SetTitle(ytitle);
  delete can;
  
  return;
}
