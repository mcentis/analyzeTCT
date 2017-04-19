#include "AnalysisClass.h"

#include "ConditionsTime.h"
#include "TimingStudy.h"

#include "iostream"

#include "TStyle.h"

AnalysisClass::AnalysisClass(const char* fileName, const char* cfgFile)
{
  _inFileName = std::string(fileName);
  _inFile = TFile::Open(_inFileName.c_str());
  if(_inFile == NULL){
    std::cout << "[Error] Could not open " << _inFileName << std::endl;
    exit(1);
  }

  _cfgAnalysis = new ConfigFileReader(cfgFile); // read config file

  RootBeautySettings();
  
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
  GetValuesFromCfgMap();
  
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
  _anaVector.push_back(new TimingStudy(this, "timingStudy"));
  
  return;
}

AnalysisClass::~AnalysisClass()
{
  _outFile->Close();
  
  return;
}

void AnalysisClass::GetValuesFromCfgMap()
{
  //================================================= get stuff from config file =====================
  _xcutLo = atof(_cfgAnalysis->GetValue("xcutLo").c_str());
  _xcutHi = atof(_cfgAnalysis->GetValue("xcutHi").c_str());
  if(_xcutLo >= _xcutHi)
    _applyXcut = false;
  else
    _applyXcut = true;

  _ycutLo = atof(_cfgAnalysis->GetValue("ycutLo").c_str());
  _ycutHi = atof(_cfgAnalysis->GetValue("ycutHi").c_str());
  if(_ycutLo >= _ycutHi)
    _applyYcut = false;
  else
    _applyYcut = true;

  _zcutLo = atof(_cfgAnalysis->GetValue("zcutLo").c_str());
  _zcutHi = atof(_cfgAnalysis->GetValue("zcutHi").c_str());
  if(_zcutLo >= _zcutHi)
    _applyZcut = false;
  else
    _applyZcut = true;

  _pol1 = atoi(_cfgAnalysis->GetValue("polarity1").c_str());
  _pol1 /= abs(_pol1); // normalize polarity

  _pol2 = atoi(_cfgAnalysis->GetValue("polarity2").c_str());
  _pol2 /= abs(_pol2); // normalize polarity

  _nbStart1 = atof(_cfgAnalysis->GetValue("nbStart1").c_str());
  _nbStop1 = atof(_cfgAnalysis->GetValue("nbStop1").c_str());

  _nbStart2 = atof(_cfgAnalysis->GetValue("nbStart2").c_str());
  _nbStop2 = atof(_cfgAnalysis->GetValue("nbStop2").c_str());

  _intStart1 = atof(_cfgAnalysis->GetValue("intStart1").c_str());
  _intStop1 = atof(_cfgAnalysis->GetValue("intStop1").c_str());
  
  _intStart2 = atof(_cfgAnalysis->GetValue("intStart1").c_str());
  _intStop2 = atof(_cfgAnalysis->GetValue("intStop1").c_str());

  return;
}

void AnalysisClass::Analyze()
{
  long int entries = _tree->GetEntries();

  std::vector<AnalysisPrototype*>::iterator it;
  
  for(long int i = 0; i < entries; i++)
    {
      _tree->GetEntry(i);

      CheckPositionCut(); // set flag for xyz cut
      CalcPulseProperties(); // baseline, amplitude, max pos, integral
      
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

void AnalysisClass::CheckPositionCut()
{
  if(_applyXcut && (_x <= _xcutLo || _x >= _xcutHi))
    _posCutPassed = false;
  else if(_applyYcut && (_y <= _ycutLo || _y >= _ycutHi))
    _posCutPassed = false;
  else if(_applyZcut && (_z <= _zcutLo || _z >= _zcutHi))
    _posCutPassed = false;
  else
    _posCutPassed = true;
  
  return;
}

void AnalysisClass::CalcPulseProperties()
{
  _bl1 = CalcBaseline(_trace1, _time1, _npt, _pol1, _nbStart1, _nbStop1);
  
  _bl2 = CalcBaseline(_trace2, _time2, _npt, _pol2, _nbStart2, _nbStop2);
  
  FindMax(_trace1, _time1, _npt, _pol1, _ampli1, _maxPos1);
  _ampli1 -= _bl1;
  
  FindMax(_trace2, _time2, _npt, _pol2, _ampli2, _maxPos2);
  _ampli2 -= _bl2;
  
  return;
}

double AnalysisClass::CalcBaseline(Double_t* tra, Double_t* tim, Int_t n, int pol, double start, double stop)
{
  double sum = 0;
  int count = 0;
  for(int i = 0; i < n; ++i)
    if(tim[i] > start && tim[i] < stop){
      sum += tra[i];
      count++;
    }
 
  return pol * sum/count;
}

void AnalysisClass::FindMax(Double_t* tra, Double_t* tim, Int_t n, int pol, double& max, double& maxpos)
{
  int ptPos = -1;
  double maximum = -1e6;
  for(int i = 0; i < n; ++i)
    if(pol * tra[i] > maximum){
      maximum = pol * tra[i];
      ptPos = i;
    }
  
  // interpolation using points near maximum
  // function y = a x**2 + b x + c
  double y1 = tra[ptPos - 1];
  double y2 = tra[ptPos];
  double y3 = tra[ptPos + 1];
  double x1 = tim[ptPos - 1];
  double x2 = tim[ptPos];
  double x3 = tim[ptPos + 1];

  double a = ( y3-y1 - (y2-y1)*(x3-x1)/(x2-x1) ) / ( pow(x3,2)-pow(x1,2) - (pow(x2,2)-pow(x1,2))*(x3-x1)/(x2-x1) );
  double b = ( y2-y1 - a*(pow(x2,2)-pow(x1,2)) ) / (x2-x1);
  double c = y1 - a*pow(x1,2) - b*x1;
  
  max = - pow(b,2)/(4*a) + c;
  max *= pol;
  maxpos = -b/(2*a);
    
  return;
}

void AnalysisClass::RootBeautySettings()
{
  gStyle->SetOptFit();
  gStyle->SetOptStat(111110);
  //gStyle->SetOptFit(0);
  //gStyle->SetOptStat(0);
  //gStyle->SetOptTitle(0);

  // big axis labels
  gStyle->SetLabelSize(0.05, "x");
  gStyle->SetLabelSize(0.05, "y");
  gStyle->SetLabelSize(0.05, "z");

  gStyle->SetTitleSize(0.05, "x");
  gStyle->SetTitleSize(0.05, "y");
  gStyle->SetTitleSize(0.05, "z");

  gStyle->SetTitleOffset(0.9, "x");
  // gStyle->SetTitleOffset(0.95, "x");
  gStyle->SetTitleOffset(0.95, "y");

  gStyle->SetHistFillColor(kBlue);
  gStyle->SetHistFillStyle(3005);
  
  return;
}
