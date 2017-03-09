#include "iostream"

#include "ConversionClass.h"

int main(int argc, char** argv)
{
  if(argc != 2){
    std::cout << "\tUsage: convertTxt fileName" << std::endl;
    return -1;
  }
  
  ConversionClass* ccl = new ConversionClass(argv[1]);
  ccl->ReadFile();
  ccl->DumpConfMap();
  
  delete ccl;
  
  return 0;
}
