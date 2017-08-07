#include "iostream"
#include "string"

#include "ConversionClass.h"

int main(int argc, char** argv)
{
  if(argc < 2){
    std::cout << "\tUsage: convertTxt [option] fileName" << std::endl;
    std::cout << "\t[option]: -tp chNum -> two pulses on which channel (either 1 or 2)" << std::endl;
    return -1;
  }

  std::string fileName;
  int twoPlsCh = 0; // in case nothing specified, treat both channels
  
  for(int i = 1; i < argc; ++i){
    std::string arg = argv[i];
    if(arg == "-tp")
      twoPlsCh = atoi(argv[++i]);
    else
      fileName = arg;
  }
  
  ConversionClass* ccl = new ConversionClass((char*) fileName.c_str(), twoPlsCh);
  ccl->ReadFile();
  ccl->DumpCondMap();
  
  delete ccl;
  
  return 0;
}
