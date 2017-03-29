#include "AnalysisPrototype.h"

#include "iostream"

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

void AnalysisPrototype::Save()
{
  std::cout << "Implement Save" << std::endl;
  
  return;
}
