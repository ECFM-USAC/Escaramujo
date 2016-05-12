#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <cmath>
#include <iostream>
#include <bitset>
#include <cstddef> 
using namespace std;

#include "TH1F.h"
#include "TF1.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLine.h"
#include "TError.h"
#include "TNtuple.h"
#include "TVector.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TApplication.h"
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TROOT.h>
#include "TStyle.h"
#include "TTreeViewer.h"
#include "TLatex.h"
#include "TPaveStats.h"
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

const int gVerbosity   = 1;
const int kMaxLine     = 1024;
const int kDataCharacters = 32;
const int kClockTSize = 8;
const int kNumTotalEdges = 8;
const int kNumChannels = 4;

const float kMaxTime = 30e3;
const float kMinTime = 0;

const int kMaxNHitsTDC = 16;

const double kClockRes = 40;    // ns/count
const double kBoardRes = kClockRes/32;  // ns/count (1.25 for 6000, 0.75 for 5000 series

struct event_t{
  vector<vector<Double_t> > tdcLE;
  vector<vector<Double_t> > tdcTE;  
};

struct run_t{
  vector<int> chHits;
//   map< int, int > chMap;
  vector<event_t> event;
};


void printHelp(){
  cout << "\nUsage:\n";
  cout << " * Save root file:\n";
  cout << "    ./qNet2root.exe <input qNet file> <output root file>\n";
}


std::bitset<4> hex2bitset(string hexStr){
  stringstream hexStrSS;
  hexStrSS << hex << hexStr;
  unsigned n;
  hexStrSS >> n;
  std::bitset<4> b(n);
  return b;
}

int hexData2Time(string edgeStrLSB, string edgeStrMSB){
  int intedHex;
  edgeStrMSB.append(edgeStrLSB);
  stringstream hexStrSS;
  hexStrSS << hex << edgeStrMSB;
  unsigned n;
  hexStrSS >> n;
  std::bitset<5> b(n);
  return intedHex = b.to_ulong();
}

bool evStart(string hexStr){  // decides if the event start maker is present. MSB in MSByte
  bool evStart = 0;
  std::bitset<4> b = hex2bitset(hexStr);
  if(b[3]==1) evStart = 1;
  return evStart;
}    

bool hit(string hexStr){ // decides if there is a hit or not in an hex number (if the bit 1 is on of MSByte)
  bool hit = 0;
  std::bitset<4> b = hex2bitset(hexStr);
  if(b[1]==1) hit = 1;
  return hit;
}

double clockStr2double(string clockStr){ // transform the clock string into a time in [ns]
  stringstream clockTSS;
// 	  cout << " >>" << clockStr;
  clockTSS << hex << clockStr;
  unsigned unsigCl;
  clockTSS >> unsigCl;
  std::bitset<32> bCl(unsigCl);
// 	  cout << " " << bCl.to_ulong() << " ";
  double clockTi = bCl.to_ulong()*kClockRes;
//   cout << " " << std::setprecision(0) << std::fixed << clockTi << "<< ";
  return clockTi;
}

bool saneLine(string lineS){
  bool saneLine = 1;
  { // check if there is trash in the line //
    size_t found = lineS.find_first_not_of(" 0123456789ABCDEF\t\v\r\n#"); 
    if(found != string::npos || lineS.empty()){
      saneLine = 0;              
//       if(gVerbosity>2) cout << "character other than hex, space, tab, newline\n";
    }
  } 
  
  {  // check if the clock tick is an hex number 
    string clockStr = lineS.substr(0,kClockTSize);
    size_t found = clockStr.find_first_not_of("0123456789ABCDEF");
    if(found != string::npos){
      saneLine = 0;
//       if(gVerbosity>2) cout << "Clock Tick with characters other than hex\n";
    } 
  }
  
  { // check if there are spaces between the data
    int kFirstSpace = 8;
    bool pars = 0;
    for(int i=0;i<8;++i){
      std::locale loc;
      char s = lineS[kFirstSpace+3*i];
      if(! std::isspace(s,loc) ) pars=1;
    }
    if(pars){
      saneLine = 0;
//       if(gVerbosity>2) cout << "incorrect spacing in data\n";
    } 
  }  
//   cout << "saneLine = " << saneLine << endl;
  if(saneLine) return true;
  else return false;
}
    
    

bool fileExist(const char *fileName){
  ifstream in(fileName,ios::in);
  
  if(in.fail()){
    cerr <<"\nError reading file: " << fileName <<"\nThe file doesn't exist!\n\n";
    in.close();
    return false;
  }
  
  in.close();
  return true;
}

bool readFile(const char *inFile, run_t &run){
  
  vector<event_t> &evV = run.event;
  run.chHits.resize(4);
  
  if(gVerbosity>0){
    cout << "Reading input file: " << inFile << endl;
  }
  
  ifstream in(inFile);
  
  if(!in.is_open()){
    cerr << "\nCan't open file: " << inFile << endl << endl;
    return false;
  }
  
  unsigned int n=0;
  unsigned int nErr=0;
  unsigned int ln=0;
  
  // aks the first line
  char line[kMaxLine];
  in.getline(line,kMaxLine);
  ++ln;
  string lineS(line,kDataCharacters);
  
  while(!saneLine(lineS)){
    in.getline(line,kMaxLine);
    ++ln;
    lineS.assign(line,kDataCharacters);
//     cout << saneLine(lineS) << endl;
  } 
  if(gVerbosity>2) cout << ln << ": " << lineS << "\tFirst line" << endl;
    
  while(!in.eof()){ // start to read file
    
//     cout << "evV.size()=" << evV.size() << endl;
//     if(evV.size()>20) break;

    string evStartString = lineS.substr(kClockTSize+1,1);
    bool bEvStart = evStart(evStartString);  // test the event start flag
    if(bEvStart){  // event start
      ++n;
      if(gVerbosity>2) cout << "\t\t * * * event Start n = " << n << "\t\t * * * " << endl;
      event_t ev;
      ev.tdcLE.resize(4);
      ev.tdcTE.resize(4);
      bEvStart = 0; // put the event start flag OFF
      bool evSane = 1;
      string clockStrEvStart = lineS.substr(0,kClockTSize);
      double clockTEvStart = clockStr2double(clockStrEvStart);
      do{
	
	string clockStr = lineS.substr(0,kClockTSize);
	double clockT = clockStr2double(clockStr) - clockTEvStart;
	if(gVerbosity>2) cout << ln << ": " << lineS << "\tIN of event" << endl;
	
	for(int i=0;i<kNumTotalEdges;++i){
	  string edgeStrMSB = lineS.substr(kClockTSize+1 + i*3,1);
	  string edgeStrLSB = lineS.substr(kClockTSize+2 + i*3,1);
	  bool bHit = hit(edgeStrMSB);
	  if(bHit){
	    Double_t t = hexData2Time(edgeStrLSB,edgeStrMSB) * kBoardRes + clockT; 
	    if(t<kMinTime || t>kMaxTime) evSane = 0;
	    unsigned int chTag = (int) ((i/2)%kNumChannels);
	    int edgeTypeInt = (i%2);
	    if     (edgeTypeInt==0) ev.tdcLE[(int)chTag].push_back(t);
	    else if(edgeTypeInt==1) ev.tdcTE[(int)chTag].push_back(t);
	    else evSane = 0;
	    if(gVerbosity>2){
	      cout << ln << ": "; 
	      cout << edgeStrMSB << edgeStrLSB << " ";
	      cout << "ch" << chTag << "_" << edgeTypeInt << " " << std::setprecision(2) << std::fixed << t << " " << clockStr << " " << clockStrEvStart;
	      cout << " " << clockStr2double(clockStr) << " " << clockStr2double(clockStrEvStart) << "\n";
	    }
	  }
	}
	char line[kMaxLine];
	in.getline(line,kMaxLine);
	++ln;
	lineS.assign(line,kDataCharacters);
	if(!saneLine(lineS)){
	  ++nErr;
	  break;
	}
	
	string evStartString = lineS.substr(kClockTSize+1,1);
	bEvStart = evStart(evStartString);  // test the event start flag
	if(bEvStart){
	  for(unsigned int c=0;c<kNumChannels;++c){
	    run.chHits[c] += ev.tdcLE[c].size();
	    if(ev.tdcLE[c].size()>kMaxNHitsTDC-2) evSane = 0;
	    if(ev.tdcLE[c].size() != ev.tdcTE[c].size() ) evSane = 0;
	  }
	  if(!evSane){
	    ++nErr;
	    continue;
	  } 
	  evV.push_back(ev);
	}
	
      } while(!bEvStart); // do this while there the new event flag is off
      
      
    } // event ends
    else{
        // aks the first line
      char line[kMaxLine];
      in.getline(line,kMaxLine);
      ++ln;
      lineS.assign(line,kDataCharacters);
      if(gVerbosity>2) cout << ln << ": " << lineS << "\tOUT of event" << endl;
      if(!saneLine(lineS)){
	continue;
      } 
    }
    
    
  } // end of while that reads the file until de eof
  
  in.close();
  
  if(gVerbosity>0){
    cout << "=============================\n";
    cout << "Events read:\t" << n
    << "\nParsing errors:\t" << nErr
    << "\nGood events:\t"    << evV.size() << endl;

    cout << "\nChannels present (TDC)\n";
    cout << " CH\tCounts\n";
    for(unsigned int c=0;c<kNumChannels;++c){
      cout << " " << c << "\t" << run.chHits[c] << endl;
    }
//     for(std::map<int, int>::iterator it = run.chMap.begin(); it != run.chMap.end(); it++){
//       cout << " " << it->first << "\t" << it->second << endl;
//     }
    cout << "=============================\n";
  }

  return true;
}  

void data2tree(const run_t &run, TTree *dataTree){

  const vector<event_t> &evV = run.event;
  const unsigned int nCh = kNumChannels;
  
  Double_t **tdcLEV = new Double_t *[nCh];
  Double_t **tdcTEV = new Double_t *[nCh];
  Int_t *tdcSizeV = new Int_t[nCh];
  
  //Channel name map
  map<string, string> channel2chName;
  vector<string> tdcLEName;
  vector<string> tdcTEName;
  vector<string> tdcSizeName;
  
  for(unsigned int c=0;c<nCh;++c){

    ostringstream varName;
    varName << "TDC_LE_" << c;
    tdcLEName.push_back(varName.str());
    
    varName.str("");
    varName << "TDC_TE_" << c;
    tdcTEName.push_back(varName.str());
    
    ostringstream varSizeName;
    varSizeName << "sizeTDC_" << c;
    tdcSizeName.push_back(varSizeName.str());
    
  }
  
  cout << "\n\n Initilize TTree structure \n\n";
  //Init tree structure
  for(unsigned int c=0;c<nCh;++c){
    tdcLEV[c] = new  Double_t[kMaxNHitsTDC];
    tdcTEV[c] = new  Double_t[kMaxNHitsTDC];
    dataTree->Branch(tdcSizeName[c].c_str(), &(tdcSizeV[c]),(tdcSizeName[c]+"/I").c_str() );
    dataTree->Branch(tdcLEName[c].c_str(),tdcLEV[c],(tdcLEName[c]+"["+tdcSizeName[c]+"]/D").c_str() );
    dataTree->Branch(tdcTEName[c].c_str(),tdcTEV[c],(tdcTEName[c]+"["+tdcSizeName[c]+"]/D").c_str() );
  }
  

  //Fill tree
  cout << "\n\n Filling TTree \n\n";
  for(unsigned int i=0;i<evV.size();++i){ // loop on run events
    
//     cout << "i=" << i << endl;
//     if(i>4000 && i<5000) continue;

    for(unsigned int c=0;c<nCh;++c){// loop on tdc channels
      
      const unsigned int nEntr = evV[i].tdcLE[c].size();
      tdcSizeV[c] = nEntr;
//       cout << "ch=" << c << "\tnEntr=" << nEntr << endl;
      for(unsigned int e=0;e<nEntr;++e){
	
	tdcLEV[c][e]         = evV[i].tdcLE[c][e];
// 	cout << "llega\n";
	tdcTEV[c][e]         = evV[i].tdcTE[c][e];
	
// 	cout << i << "," << c << "," << e << " = " << tdcV[c][e] << " " << tdcEdgeTypeV[c][e] << endl;
      }
        
      
//       std::map<int, vector<double> >::const_iterator tdcIt=evV[i].tdc.find(tdcCh[c]);
//       std::map<int, vector<int> >::const_iterator tdcEdgeTypeIt=evV[i].tdcEdgeType.find(tdcCh[c]);
//       if(tdcIt==evV[i].tdc.end() || tdcEdgeTypeIt==evV[i].tdcEdgeType.end()){
//         tdcSizeV[c]=0;
//       }
//       else{
//         const unsigned int nEntr = tdcIt->second.size();
//         tdcSizeV[c] = nEntr;
//         for(unsigned int e=0;e<nEntr;++e){
//           tdcV[c][e]=(tdcIt->second)[e];
//           tdcEdgeTypeV[c][e]=(tdcEdgeTypeIt->second)[e];
//         }
//       }
      
    }

    dataTree->Fill();
  }

  //dataTree->Print(); 
}

int main(int argc,char *argv[]){
  
  if( argc!=3 ){
    printHelp();
    return 0;
  }
  
  if(!fileExist(argv[1])){
    return 1;
  }

  run_t run;
  readFile(argv[1], run);

  TFile *outF = new TFile(argv[2],"RECREATE");
  TTree *dataTree = new TTree("dataTree","dataTree");
  data2tree(run, dataTree);
  cout << "\n\n Writing TTree on disc \n\n";
  dataTree->Write();
  cout << "\n\n Closing ROOT file \n\n";
//   outF->Close();  
  delete outF;
  cout << "\n\n * * * All done * * * \n\n";
  
 return 0;
}