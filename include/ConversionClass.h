#ifndef CONVERSIONCLASS_H
#define CONVERSIONCLASS_H

#include "string"
#include "map"
#include "fstream"

#include "TTree.h"
#include "TFile.h"

class ConversionClass
{
 public:
  ConversionClass(char* fileName);
  ~ConversionClass();
  void ReadFile();
  void DumpConfMap();
  
 private:
  std::string _fileName;
  std::ifstream _fileStr;

  std::map<std::string, std::string> _confMap;
  
  TTree* _tree;
  TFile* _outFile;

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

#endif //#ifndef CONVERSIONCLASS_H
