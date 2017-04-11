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
  void Save();

 private:
  //=================================== functions ===============================
  void CalcMeanStdDev(std::vector<double> vec, double& mean, double& stdDev, double& Emean, double& EstdDev);
  void FindMax(Double_t* tra, Double_t* tim, Int_t n, int pol, double& max, double& maxpos);
  double CalcBaseline(Double_t* tra, Double_t* tim, Int_t n, int pol, double start, double stop);
  double CalcTimeThrLinear2pt(Double_t* tra, Double_t* tim, Int_t n, int pol, double thr, double offset); // thr and offset must have already the right polarity
  void FillRiseTime_RiseTimeAmpli();
  
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

  //================================ analysis variables =========================
  // baseline
  double _bl1;
  double _bl2;

  //amplitude
  double _ampli1;
  double _ampli2;

  //position of maximum
  double _maxPos1;
  double _maxPos2;
  
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

  // rise time 10 90
  TH1D* _riseTime1090_1;
  TH1D* _riseTime1090_2;
  
  // rise time 10 90 vs amplitude
  TH2D* _riseTimeAmpli1;  
  TH2D* _riseTimeAmpli2;

  // study of CFD thresholds
  TH2D* _timeDiffMeanCFDfrac; 
  TH2D* _timeDiffStdDevCFDfrac;
  std::vector<double> _CFDfracVec; // threshold vector
  std::vector<double>** _dtCFDfrac; // time differences

};

#endif //#ifndef TIMINGSTUDY_H
