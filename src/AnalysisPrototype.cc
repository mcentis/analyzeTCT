#include "AnalysisPrototype.h"

#include "iostream"

#include "TCanvas.h"
#include "TAxis.h"

AnalysisPrototype::AnalysisPrototype(AnalysisClass* acl, const char* dirName)
{
  _acl = acl;
  _dirName = std::string(dirName);
  
  return;
}

AnalysisPrototype::~AnalysisPrototype()
{
  return;
}

void AnalysisPrototype::AnalysisAction()
{
  std::cout << "Implement AnalysisAction" << std::endl;
  
  return;
}

void AnalysisPrototype::Save(TDirectory* parent)
{
  std::cout << "Implement Save" << std::endl;
  
  return;
}

void AnalysisPrototype::Process()
{
 return;
}

double AnalysisPrototype::CalcTimeThrLinear2pt(Double_t* tra, Double_t* tim, Int_t n, int pol, double thr, double offset)
{
  int i = 0;
  for(; i < n; ++i)
    if(pol * tra[i] - offset > thr) break;

  // linear interpolation between points below and above thr
  // y = a x + b
  double y1 = pol * tra[i] - offset;
  double y2 = pol * tra[i - 1] - offset;
  double x1 = tim[i];
  double x2 = tim[i - 1];

  double a = (y2-y1)/(x2-x1);
  double b = y1 -a*x1;

  return (thr - b)/a;
}

void AnalysisPrototype::CalcMeanStdDev(std::vector<double> vec, double& mean, double& stdDev, double& Emean, double& EstdDev)
{
  int N = vec.size();

  if(N == 0){
    mean = 0;
    stdDev = 0;
    Emean = 0;
    EstdDev = 0;
    return;
  }

  if(N < 4){ // to avoid strange results in error calculation
    mean = vec[0];
    stdDev = 0;
    Emean = 0;
    EstdDev = 0;
    return;
  }

  double sum = 0;
  double sumD2 = 0;
  double sumD4 = 0;
  
  for(std::vector<double>::iterator it = vec.begin(); it != vec.end(); ++it)
    sum += *it;

  mean = sum / N; // mean

  for(std::vector<double>::iterator it = vec.begin(); it != vec.end(); ++it){
    sumD2 += pow(*it - mean, 2);
    sumD4 += pow(*it - mean, 4);
  }

  double mu2 = sumD2 / (N - 1); // second central moment (variance)
  
  stdDev = sqrt(mu2); // std dev

  Emean = sqrt(mu2/N);

  double mu4 = ( pow(N, 2)*sumD4/(N-1) - 3*(2*N-3)*pow(mu2, 2) ) / (pow(N, 2)-3*N+3); // fourth central moment from http://mathworld.wolfram.com/SampleCentralMoment.html

  double Emu2 = sqrt((pow(N-1, 2) * mu4 - (N-1) * (N-3) * pow(mu2, 2)) / pow(N, 3)); // std dev of mu2 distribution from http://mathworld.wolfram.com/SampleVarianceDistribution.html
    
  EstdDev = 0.5 * Emu2 / sqrt(mu2);

  return;
}

void AnalysisPrototype::PutAxisLabels(TGraph* gr, const char* xtitle, const char* ytitle)
{
  TCanvas* can = new TCanvas();
  gr->Draw("apl");
  gr->GetXaxis()->SetTitle(xtitle);
  gr->GetYaxis()->SetTitle(ytitle);
  delete can;
  
  return;
}
