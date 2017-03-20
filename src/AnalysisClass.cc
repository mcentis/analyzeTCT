#include "AnalysisClass.h"

#include "iostream"

AnalysisClass::AnalysisClass(char* fileName)
{
  _inFileName = std::string(fileName);
  _inFile = TFile::Open(_inFileName.c_str());
  if(_inFile == NULL){
    std::cout << "[Error] Could not open " << _inFileName << std::endl;
    exit(1);
  }

  std::cout << "1" << std::endl;
  
  _tree = (TTree*) _inFile->Get("TCTtree");

  _npt = 1e6;
  _trace1 = new Double_t[_npt];
  _trace2 = new Double_t[_npt];
  _time1 = new Double_t[_npt];
  _time2 = new Double_t[_npt];

  _tree->SetBranchAddress("npt", &_npt);
  _tree->SetBranchAddress("trace1", _trace1);
  _tree->SetBranchAddress("time1", _time1);
  _tree->SetBranchAddress("trace2", _trace2);
  _tree->SetBranchAddress("time2", _time2);

  _tree->SetBranchAddress("set", &_set);
  _tree->SetBranchAddress("timeStamp", &_timeStamp);
  _tree->SetBranchAddress("chillerSetT", &_chillerSetT);
  _tree->SetBranchAddress("chillerIntern", &_chillerIntern);
  _tree->SetBranchAddress("chillerExtern", &_chillerExtern);
  _tree->SetBranchAddress("peltierSetT", &_peltierSetT);
  _tree->SetBranchAddress("pcbT", &_pcbT);
  _tree->SetBranchAddress("copperT", &_copperT);
  _tree->SetBranchAddress("biasSet", &_biasSet);
  _tree->SetBranchAddress("current", &_current);
  _tree->SetBranchAddress("biasMeas", &_biasMeas);
  _tree->SetBranchAddress("x", &_x);
  _tree->SetBranchAddress("y", &_y);
  _tree->SetBranchAddress("z", &_z);
  _tree->SetBranchAddress("ampliV", &_ampliV);
  _tree->SetBranchAddress("ampliI", &_ampliI);
  _tree->SetBranchAddress("rep", &_rep);
  _tree->SetBranchAddress("event", &_event);

  std::cout << "2" << std::endl;

  _measCond = (MeasurementConditions*) _tree->GetUserInfo()->At(0);
  std::cout << "3" << std::endl;

  _measCond->DumpCondMap();

  std::cout << "4" << std::endl;

  return;
}

AnalysisClass::~AnalysisClass()
{
  return;
}
