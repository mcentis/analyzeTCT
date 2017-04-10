#ifndef TIMINGSTUDY_H
#define TIMINGSTUDY_H

#include "AnalysisPrototype.h"

class AnalysisClass;

class TimingStudy : public AnalysisPrototype
{
 public:
  TimingStudy(AnalysisClass* acl, const char* dirName);
  ~TimingStudy();
  void AnalysisAction();
  void Save();

  //private:
  
};

#endif //#ifndef TIMINGSTUDY_H
