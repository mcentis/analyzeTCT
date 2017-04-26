#ifndef SIGNOISEVSV_H
#define SIGNOISEVSV_H

#include "AnalysisPrototype.h"

#include "TGraphErrors.h"

class AnalysisClass;

class SigNoiseVsV : public AnalysisPrototype
{
 public:
  SigNoiseVsV(AnalysisClass* acl, const char* dirName);
  ~SigNoiseVsV();
  void AnalysisAction();
  void Save(TDirectory* parent);
  void Process();

 private:
  std::vector<unsigned int> _stepVec; // vector containing the steps
  std::vector<std::vector<double>*> _biasVec; // vector with the bias value for each repetition and step
  std::vector<std::vector<double>*> _currVec; // vector with the current value for each repetition and step
  std::vector<std::vector<double>*> _ampli1Vec; // amplitudes
  std::vector<std::vector<double>*> _ampli2Vec;
  std::vector<std::vector<double>*> _inte1Vec; // integrals
  std::vector<std::vector<double>*> _inte2Vec;
  
  TGraphErrors* _ivGr;
  TGraphErrors* _ampli1BiasGr;
  TGraphErrors* _ampli2BiasGr;
  TGraphErrors* _inte1BiasGr;
  TGraphErrors* _inte2BiasGr;

  void NewSet();
  void SetPointGr(std::vector<double> vec, TGraphErrors* gr, unsigned int npt, double bias, double Ebias);
  
};


#endif //#ifndef SIGNOISEVSV_H
