#ifndef TIMINGSTUDY_H
#define TIMINGSTUDY_H

#include "AnalysisPrototype.h"

#include "TH2D.h"

#include "vector"

class AnalysisClass;

class TimingStudy : public AnalysisPrototype
{
 public:
  TimingStudy(AnalysisClass* acl, const char* dirName);
  ~TimingStudy();
  void AnalysisAction();
  void Save(TDirectory* parent);
  void Process();
  
 private:
  //=================================== functions ===============================
  void CalcMeanStdDev(std::vector<double> vec, double& mean, double& stdDev, double& Emean, double& EstdDev);
  double CalcTimeThrLinear2pt(Double_t* tra, Double_t* tim, Int_t n, int pol, double thr, double offset); // thr and offset must have already the right polarity

  void FillRiseTime_RiseTimeAmpli();

  void InitThrStudy(); // study of thresholds to optimize them
  void FillThrStudy();
  void ProcessThrStudy();
  void WriteThrStudy();
  
  //========================= histograms ===============================
  // superimposition of pulses
  TH2D* _supPulse1;
  TH2D* _supPulse2;

  // noise
  TH1D* _noiseDistr1;
  TH1D* _noiseDistr2;
  
  // baseline
  TH1D* _baselineDistr1;
  TH1D* _baselineDistr2;

  // amplitude
  TH1D* _ampliDistr1;
  TH1D* _ampliDistr2;

  // rise time 20 80
  TH1D* _riseTime2080_1;
  TH1D* _riseTime2080_2;
  
  // rise time 20 80 vs amplitude
  TH2D* _riseTimeAmpli1;  
  TH2D* _riseTimeAmpli2;

  //============================= threshold study CFD =============
  TH2D* _timeDiffMeanCFDfrac; 
  TH2D* _timeDiffStdDevCFDfrac;
  std::vector<double> _CFDfracVec; // threshold vector
  std::vector<double>** _dtCFDfrac; // time differences
  TH1D* _bestCFDdistr;

  //============================= threshold study LED =============
  TH2D* _neventsLEDthr;
  TH2D* _timeDiffMeanLEDthr;
  TH2D* _timeDiffStdDevLEDthr;
  std::vector<double> _LEDthrVec;
  std::vector<double>** _dtLEDthr;
  TH1D* _bestLEDdistr;
  
};

#endif //#ifndef TIMINGSTUDY_H
