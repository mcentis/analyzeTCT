#ifndef MEASUREMENTCONDITIONS_H
#define MEASUREMENTCONDITIONS_H

#include "map"
#include "string"

#include "TObject.h"

//#include "ConversionClass.h"

class MeasurementConditions : public TObject
{
  friend class ConversionClass;
  friend class AnalysisClass;
  
 private:
    std::map<std::string, std::string> _condMap;
    
 public:
    void DumpCondMap();
  
};

#endif //#ifndef MEASUREMENTCONDITIONS_H
