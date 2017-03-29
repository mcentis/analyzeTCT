#ifndef TEMPERATURETIME_H
#define TEMPERATURETIME_H

#include "AnalysisPrototype.h"

#include "TGraph.h"

class AnalysisClass;

class TemperatureTime : public AnalysisPrototype
{
 public:
  TemperatureTime(AnalysisClass* acl, const char* dirName);
  ~TemperatureTime();
  void AnalysisAction();
  void Save();
  
 private:
  TGraph* _chillerSetTtime;
  TGraph* _chillerInternTime;
  TGraph* _chillerExternTime;
  TGraph* _peltierSetTtime;
  TGraph* _pcbTtime;
  TGraph* _copperTtime;

  void PutAxisLabels(TGraph* gr);
};

#endif //#ifndef TEMPERATURETIME_H
