#include "iostream"

#include "AnalysisClass.h"

int main(int argc, char** argv)
{
  if(argc != 3){
    std::cout << "\tUsage: analyzeData fileName configFile" << std::endl;
    return -1;
  }

  AnalysisClass* ancl = new AnalysisClass(argv[1], argv[2]);
  ancl->Analyze();
  ancl->Save();
  
  delete ancl;
  
  return 0;
}
