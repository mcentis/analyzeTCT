#include "MeasurementConditions.h"

#include "iostream"

void MeasurementConditions::DumpCondMap()
{
  std::map<std::string, std::string>::iterator it;
  for(it = _condMap.begin(); it != _condMap.end(); ++it)
    std::cout << (*it).first << " => " << (*it).second << std::endl;

  return;
}

