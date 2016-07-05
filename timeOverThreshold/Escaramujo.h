#ifndef ESCARAMUJO_H
#define ESCARAMUJO_H

#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map> 
#include <algorithm>
#include <functional>
#include <cmath>
#include <iostream>
#include <sstream>
#include <utility>
#include <limits>
#include <getopt.h>
using namespace std;

#include "TRootBrowser.h"
#include "TBrowser.h"
#include "TH1F.h"
#include "TF1.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLine.h"
#include "TProfile.h"
#include "TError.h"
#include "TNtuple.h"
#include "TVector.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TPad.h"
#include "TGaxis.h"
#include "TAttLine.h"
#include "TApplication.h"
#include "THStack.h"
#include <TGClient.h>
#include <TF1.h>
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TROOT.h>
#include "TStyle.h"
#include "TTreeViewer.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TPaveStats.h"

const int kMaxLineBarMap = 1024;
const int kHitsTDCmax  = 18;

const float kTMin = -100;
const float kTMax = 40000;
const int kTBins = 0.01*(kTMax-kTMin);

const int kNumCh = 3;
const int kNHitsMin = 0;
const int kNHitsMax = 20;
const int kHNitsBins = kNHitsMax - kNHitsMin;


bool fileExist(const char *fileName){
  ifstream in(fileName,ios::in);
  if(in.fail()){
    in.close();
    return false;
  }
  in.close();
  return true;
}



bool readListFile(const string &inListFile, vector<string> &inFileList){
  
  cout << "Reading input list file.\n";
  
  ifstream in(inListFile.c_str());
  
  if(!in.is_open()){
    cerr << "\nCan't open file: " << inListFile << endl << endl;
    return false;
  }
  
  while(!in.eof()){
    char line[kMaxLineBarMap];
    in.getline(line,kMaxLineBarMap);
    string lineS(line);
    
    size_t found = lineS.find_first_not_of(" \t\v\r\n#");
    if(found == string::npos) continue;
    
    unsigned int i=0;
    istringstream eventISS(&(lineS[found]));
    string aux;
    const unsigned int nCol = 1;
    while(eventISS >> aux){
      inFileList.push_back(aux);
      if(i>=nCol){
        i=-1;
        cout << aux << endl;
        return false;
      }
      ++i;
    }
    if(i!=nCol){
      cout << "Error: there is more than one column in the list file!\n\n";
      return false;
    }
  }
  
  in.close();
  return true;
}

int processCommandLineArgs(const int argc, char *argv[], vector<string> &inFileList){
  
  if(argc == 1) return 1;
  
  bool inListFileFlag = false;
  string inListFile = "";
  int opt=0;
  while ( (opt = getopt(argc, argv, "i:")) != -1) {
    switch (opt) {
    
    case 'i':
      if(!inListFileFlag){
        inListFile = optarg;
        inListFileFlag = true;
      }
      else{
        cerr << "\nError, can not set more than one input list file!\n\n";
        return 2;
      }
      break;

    default: /* '?' */
      return 1;
    }
  }

  inFileList.clear();
  if(inListFileFlag){
    cout << inListFile << endl;
    bool allOk = readListFile(inListFile, inFileList);
    if(!allOk){
      cerr << "\nError reading input list file!\n\n";
      return 1;
    }
  }
  
  for(int i=optind; i<argc; ++i){
    inFileList.push_back(argv[i]);
    if(!fileExist(argv[i])){
      cout << "\nError reading input file: " << argv[i] <<"\nThe file doesn't exist!\n\n";
      return 1;
    }
  }
  
  if(inFileList.size()==0){
    cerr << "Error: no input file(s) provided!\n\n";
    return 1;
  }
  
  return 0;
}

#endif
