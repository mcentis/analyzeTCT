#ifndef TIMINGSTUDY_H
#define TIMINGSTUDY_H

#include "AnalysisPrototype.h"

#include "TH2D.h"

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
  //void calcMeanStdDev(std::vector<double> vec, double& mean, double& stdDev, double& Emean, double& EstdDev);
  //double integrateSignal(Double_t* tra, Double_t* tim, Int_t n, double start, double stop, double offset);
  //void findMax(Double_t* tra, Double_t* tim, Int_t n, double& max, double& maxpos);
  double calcBaseline(Double_t* tra, Double_t* tim, Int_t n, double start, double stop);
  //double calcTimeThrLinear2pt(Double_t* tra, Double_t* tim, Int_t n, double thr, double offset);

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
  
  //========================= histograms ===============================
  // superimposition of pulses
  TH2D* _supPulse1;
  TH2D* _supPulse2;

  // noise
  TH1D* _noise1;
  TH1D* _noise2;
  
  // baseline
  TH1D* _baseline1;
  TH1D* _baseline2;
  
};

#endif //#ifndef TIMINGSTUDY_H
