#ifndef ANALYSISCLASS_H
#define ANALYSISCLASS_H

#include "TFile.h"
#include "TTree.h"

#include "string"
#include "vector"

#include "MeasurementConditions.h"

class AnalysisPrototype; // use this instead of #include "AnalysisPrototype.h" to avoid errors in compilation

class AnalysisClass
{
 public:
  AnalysisClass(char* fileName);
  ~AnalysisClass();
  
  void Analyze();
  void Save();
  
 private:
  TTree* _tree;
  TFile* _inFile;

  std::string _inFileName;
  std::vector<AnalysisPrototype*> _anaVector;

 public: // this stuff needs to be public to allow access from other classes

  TFile* _outFile; // here to save stuff after analysis

  MeasurementConditions* _measCond;
  
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

};

#endif //#ifndef ANALYSISCLASS_H
