#ifndef CONDITIONSTIME_H
#define CONDITIONSTIME_H

#include "AnalysisPrototype.h"

#include "TGraph.h"

class AnalysisClass;

class ConditionsTime : public AnalysisPrototype
{
 public:
  ConditionsTime(AnalysisClass* acl, const char* dirName);
  ~ConditionsTime();
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

#endif //#ifndef CONDITIONSTIME_H
