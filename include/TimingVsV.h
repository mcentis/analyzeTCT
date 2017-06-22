#ifndef TIMINGVSV_H
#define TIMINGVSV_H

#include "AnalysisPrototype.h"

#include "TGraphErrors.h"

class AnalysisClass;

class TimingVsV : public AnalysisPrototype
{
 public:
  TimingVsV(AnalysisClass* acl, const char* dirName);
  ~TimingVsV();
  void AnalysisAction();
  void Save(TDirectory* parent);
  void Process();
  void NewSet();
  
 private:
  std::vector<std::vector<double>*> _biasVec; // vector with the bias value for each repetition and step

  std::vector<std::vector<double>> _CFDVec;
  
  TGraphErrors* _thr1CFDGr;
  TGraphErrors* _thr2CFDGr;
  TGraphErrors* _meanCFDGr;
  TGraphErrors* _stdDevCFDGr;

  std::vector<std::vector<double>> _LEDVec;
  
  TGraphErrors* _thr1LEDGr;
  TGraphErrors* _thr2LEDGr;
  TGraphErrors* _meanLEDGr;
  TGraphErrors* _stdDevLEDGr;

  std::vector<std::vector<double>> _riseTimeVec;
  TGraphErrors* _riseTime1Gr;
  TGraphErrors* _riseTime2Gr;

};

#endif //#ifndef TIMINGVSV_H
