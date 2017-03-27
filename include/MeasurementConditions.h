#ifndef MEASUREMENTCONDITIONS_H
#define MEASUREMENTCONDITIONS_H

#include "map"
#include "string"

#include "TObject.h"

class MeasurementConditions : public TObject
{
  friend class ConversionClass;
  friend class AnalysisClass;
  
 private:
  std::map<std::string, std::string> _condMap;
    
 public:
  MeasurementConditions();
  ~MeasurementConditions();
  void DumpCondMap();
    
  ClassDef(MeasurementConditions, 1);
};

#endif //#ifndef MEASUREMENTCONDITIONS_H
