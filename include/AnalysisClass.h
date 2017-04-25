#ifndef ANALYSISCLASS_H
#define ANALYSISCLASS_H

#include "TFile.h"
#include "TTree.h"

#include "string"
#include "vector"
#include "stdlib.h"

#include "MeasurementConditions.h"
#include "ConfigFileReader.h"

class AnalysisPrototype; // use this instead of #include "AnalysisPrototype.h" to avoid errors in compilation

class AnalysisClass
{
 public:
  AnalysisClass(const char* fileName, const char* cfgFile);
  ~AnalysisClass();
  
  void Analyze();
  void Save();
  
 private:
  TTree* _tree;
  TFile* _inFile;

  std::string _inFileName;
  std::vector<AnalysisPrototype*> _anaVector;

  void RootBeautySettings();
  void CheckPositionCut();
  void CalcPulseProperties();
  void GetValuesFromCfgMap();

  void InitSet();
  void SaveSet();
  std::vector<AnalysisPrototype*> _setAnaVector;
  unsigned int _currentSet;
  char _setDirName[200];
  
  void FindMax(Double_t* tra, Double_t* tim, Int_t n, int pol, double& max, double& maxpos); // waveform amplitude
  double CalcBaseline(Double_t* tra, Double_t* tim, Int_t n, int pol, double start, double stop); // waveform baseline
  double IntegrateSignal(Double_t* tra, Double_t* tim, Int_t n, int pol, double start, double stop, double offset);
  
 public: // this stuff needs to be public to allow access from other classes

  TFile* _outFile; // here to save stuff after analysis

  MeasurementConditions* _measCond;
  ConfigFileReader* _cfgAnalysis;
  
  // ======================== variables for the tree

  // traces
  Double_t* _trace1;
  Double_t* _trace2;
  Double_t* _time1;
  Double_t* _time2;
  Int_t _npt;

  // conditions
  UInt_t _set; // counter of the set conditions (same conditions are kept for different repetitions)
  UInt_t _timeStamp;
  Double_t _chillerSetT;
  Double_t _chillerIntern;
  Double_t _chillerExtern;
  Double_t _peltierSetT;
  Double_t _pcbT;
  Double_t _copperT;
  Double_t _biasSet;
  Double_t _current;
  Double_t _biasMeas;
  Double_t _x;
  Double_t _y;
  Double_t _z;
  Double_t _ampliV;
  Double_t _ampliI;
  UInt_t _rep; // repetition counter
  UInt_t _event;

  //====================== analysis variables from cfg file ======================  
  // cuts in space
  double _xcutLo;
  double _xcutHi;

  double _ycutLo;
  double _ycutHi;

  double _zcutLo;
  double _zcutHi;

  bool _applyXcut;
  bool _applyYcut;
  bool _applyZcut;
  
  // pulse polarity for the traces
  int _pol1;
  int _pol2;

  // noise, baseline region
  double _nbStart1;
  double _nbStop1;

  double _nbStart2;
  double _nbStop2;

  // integration region for the pulses
  double _intStart1;
  double _intStop1;

  double _intStart2;
  double _intStop2;
  
  // ================================ general analysis variables

  // pos cut flag
  bool _posCutPassed;

  // baseline
  double _bl1;
  double _bl2;

  // amplitude
  double _ampli1;
  double _ampli2;

  // position of maximum
  double _maxPos1;
  double _maxPos2;

  // signal integral
  double _inte1;
  double _inte2;
  
};

#endif //#ifndef ANALYSISCLASS_H
