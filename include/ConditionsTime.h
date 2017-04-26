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
  void Save(TDirectory* parent);
  
 private:
  TGraph* _chillerSetTtime;
  TGraph* _chillerInternTime;
  TGraph* _chillerExternTime;
  TGraph* _peltierSetTtime;
  TGraph* _pcbTtime;
  TGraph* _copperTtime;

  TGraph* _xTime;
  TGraph* _yTime;
  TGraph* _zTime;

  TGraph* _ampliVtime;
  TGraph* _ampliItime;

  TGraph* _biasSetTime;
  TGraph* _biasMeasTime;
  TGraph* _currentTime;
  
};

#endif //#ifndef CONDITIONSTIME_H
