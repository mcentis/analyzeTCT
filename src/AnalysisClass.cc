#include "AnalysisClass.h"

#include "ConditionsTime.h"

#include "iostream"

AnalysisClass::AnalysisClass(const char* fileName, const char* cfgFile)
{
  _inFileName = std::string(fileName);
  _inFile = TFile::Open(_inFileName.c_str());
  if(_inFile == NULL){
    std::cout << "[Error] Could not open " << _inFileName << std::endl;
    exit(1);
  }

  _cfgAnalysis = new ConfigFileReader(cfgFile);
  
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

  _measCond = (MeasurementConditions*) _tree->GetUserInfo()->At(0);
  //_measCond->DumpCondMap();

  // open root file
  std::string outFileName = _inFileName;
  std::string rep = std::string("/"); // to be replaced
  std::string add = std::string("/analyzed"); // replacement
  std::string::size_type i = outFileName.find_last_of(rep);

  if(i != std::string::npos)
    outFileName.replace(i, rep.length(), add);
  else
    outFileName.insert(0, add);
  
  _outFile = TFile::Open(outFileName.c_str(), "RECREATE");

  // create the analysis objects
  _anaVector.push_back(new ConditionsTime(this, "conditionsTime"));
  
  return;
}

AnalysisClass::~AnalysisClass()
{
  _outFile->Close();
  
  return;
}

void AnalysisClass::Analyze()
{
  long int entries = _tree->GetEntries();

  std::vector<AnalysisPrototype*>::iterator it;
  
  for(long int i = 0; i < entries; i++)
    {
      _tree->GetEntry(i);
      for(it = _anaVector.begin(); it != _anaVector.end(); it++)
	(*it)->AnalysisAction();
    }
  
  return;
}

void AnalysisClass::Save()
{
  std::vector<AnalysisPrototype*>::iterator it;
  for(it = _anaVector.begin(); it != _anaVector.end(); it++)
    (*it)->Save();
  
  return;
}
