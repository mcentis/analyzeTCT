#include "iostream"

#include "AnalysisClass.h"

int main(int argc, char** argv)
{
  if(argc != 2){
    std::cout << "\tUsage: analyzeData fileName" << std::endl;
    return -1;
  }

  AnalysisClass* ancl = new AnalysisClass(argv[1]);
  ancl->Analyze();
  ancl->Save();
  
  delete ancl;
  
  return 0;
}
