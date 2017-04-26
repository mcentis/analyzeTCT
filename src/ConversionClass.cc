#include "ConversionClass.h"

#include "iostream"
#include "sstream"

ConversionClass::ConversionClass(char* fileName)
{
  _fileName = std::string(fileName);
  _fileStr.open(_fileName, std::ifstream::in);

  if(_fileStr.is_open() == false){
    std::cout << "[Error] Could not open " << _fileName << std::endl;
    exit(1);
  }

  // open root file
  std::string outFileName = _fileName;
  std::string ext = std::string(".txt");
  std::string::size_type i = outFileName.find(ext);

  if(i != std::string::npos)
    outFileName.erase(i, ext.length());

  outFileName.append(".root");
  
  _outFile = TFile::Open(outFileName.c_str(), "RECREATE");

  // set the tree
  _tree = new TTree("TCTtree", "");

  _npt = 1e6;
  _trace1 = new Double_t[_npt];
  _trace2 = new Double_t[_npt];
  _time1 = new Double_t[_npt];
  _time2 = new Double_t[_npt];

  _tree->Branch("npt", &_npt, "npt/I");
  _tree->Branch("trace1", _trace1, "trace1[npt]/D");
  _tree->Branch("time1", _time1, "time1[npt]/D");
  _tree->Branch("trace2", _trace2, "trace2[npt]/D");
  _tree->Branch("time2", _time2, "time2[npt]/D");

  _tree->Branch("set", &_set, "set/i");
  _tree->Branch("timeStamp", &_timeStamp, "timeStamp/i");
  _tree->Branch("chillerSetT", &_chillerSetT, "chillerSetT/D");
  _tree->Branch("chillerIntern", &_chillerIntern, "chillerIntern/D");
  _tree->Branch("chillerExtern", &_chillerExtern, "chillerExtern/D");
  _tree->Branch("peltierSetT", &_peltierSetT, "peltierSetT/D");
  _tree->Branch("pcbT", &_pcbT, "pcbT/D");
  _tree->Branch("copperT", &_copperT, "copperT/D");
  _tree->Branch("biasSet", &_biasSet, "biasSet/D");
  _tree->Branch("current", &_current, "current/D");
  _tree->Branch("biasMeas", &_biasMeas, "biasMeas/D");
  _tree->Branch("x", &_x, "x/D");
  _tree->Branch("y", &_y, "y/D");
  _tree->Branch("z", &_z, "z/D");
  _tree->Branch("ampliV", &_ampliV, "ampliV/D");
  _tree->Branch("ampliI", &_ampliI, "ampliI/D");
  _tree->Branch("rep", &_rep, "rep/i");
  _tree->Branch("event", &_event, "event/i");

  _set = 0;
  _event = 0;

  return;
}

void ConversionClass::ReadFile()
{
  std::string line;
  std::stringstream sstr;
  std::string word;
  std::string::size_type delimPos;
  
  // first loop for configuration stuff
  while(_fileStr.eof() == false){
    line.clear();
    std::getline(_fileStr, line);
    if(line.size() == 0) continue; // skip empty lines

    // looking for configuration stuff
    delimPos = line.find(":");
    if(delimPos != std::string::npos){
      _measCond._condMap[line.substr(0, delimPos)] = line.substr(delimPos + 1, line.size());
      continue;
    }

    // if not config stuff looking for the lines before the data
    sstr.clear();
    sstr.str(line);
    sstr >> word;
    if(word.compare("timestamp") == 0){
      line.clear();
      std::getline(_fileStr, line); // get the line of ======
      break;
    }
      
  }//while(_fileStr.eof() == false)

  _tree->GetUserInfo()->AddAt(&_measCond, 0);
  
  double dTime;
  int iPoint;

  // loop for data
  while(_fileStr.eof() == false){

    // line with status
    line.clear();
    sstr.clear();
    std::getline(_fileStr, line);
    if(line.size() == 0) // already reached end of file, but the flag is not up yet
      break;
    sstr.str(line);
    sstr >> _timeStamp >> _chillerSetT >> _chillerIntern >> _chillerExtern >> _peltierSetT >> _pcbT >> _copperT >> _biasSet
	 >> _current >> _biasMeas >> _x >> _y >> _z >> _rep >> _ampliV >> _ampliI;

    if(_event == 0){
      if(_rep == 0) // check in first event that repetitions start from 0 or not (if not they should start from 1)
	_repStartFrom0 = true;
      else
	_repStartFrom0 = false;
    }
    
    if(_repStartFrom0 == false)
      _rep -= 1; // to have counter starting from 0
    
    if(_rep == 0 && _event != 0) // new set, condition on event to have set counter start from 0
      _set++;

    // first scope trace
    line.clear();
    sstr.clear();
    std::getline(_fileStr, line);
    sstr.str(line);

    sstr >> dTime;
    sstr >> _npt;

    iPoint = 0;
    while(sstr.eof() == false){
      sstr >> _trace1[iPoint];
      _time1[iPoint] = dTime * iPoint;
      iPoint++;
    }

    // second scope trace
    line.clear();
    sstr.clear();
    std::getline(_fileStr, line);
    sstr.str(line);

    iPoint = 0;
    while(sstr.eof() == false){
      sstr >> _trace2[iPoint];
      _time2[iPoint] = dTime * iPoint;
      iPoint++;
    }

    _tree->Fill();
    _event++;
    
  }//while(_fileStr.eof() == false)
    
  
  return;
}

void ConversionClass::DumpCondMap()
{
  _measCond.DumpCondMap();
  
  return;
}

ConversionClass::~ConversionClass()
{
  _tree->Write();
  _outFile->Close();
  _fileStr.close();
  
  return;
}
