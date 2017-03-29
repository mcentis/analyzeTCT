#ifndef ANALYSISPROTOTYPE_H
#define ANALYSISPROTOTYPE_H

#include "AnalysisClass.h"

#include "string"

class AnalysisPrototype
{
 public:
  AnalysisPrototype(AnalysisClass* acl, const char* dirName);
  ~AnalysisPrototype();
  virtual void AnalysisAction();
  virtual void Save();
  
 protected:
  AnalysisClass* _acl;
  std::string _dirName;
};


#endif //#ifndef ANALYSISPROTOTYPE_H
