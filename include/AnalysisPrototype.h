#ifndef ANALYSISPROTOTYPE_H
#define ANALYSISPROTOTYPE_H

#include "AnalysisClass.h"

#include "string"

#include "TGraph.h"

class AnalysisPrototype
{
 public:
  AnalysisPrototype(AnalysisClass* acl, const char* dirName);
  virtual ~AnalysisPrototype();
  virtual void AnalysisAction();
  virtual void Save(TDirectory* parent);
  virtual void Process();
  virtual void NewSet();
  
 protected:
  AnalysisClass* _acl;
  std::string _dirName;

  void CalcMeanStdDev(std::vector<double> vec, double& mean, double& stdDev, double& Emean, double& EstdDev);
  double CalcTimeThrLinear2pt(Double_t* tra, Double_t* tim, Int_t n, int pol, double thr, double offset); // thr and offset must have already the right polarity
  
  void PutAxisLabels(TGraph* gr, const char* xtitle, const char* ytitle);

};


#endif //#ifndef ANALYSISPROTOTYPE_H
