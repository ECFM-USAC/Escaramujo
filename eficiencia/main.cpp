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


int main(int argc,char *argv[]) {   // code starts // main
  
  TApplication myApp("myApp", 0, 0 ,0,-1);   
  
  vector<string> inFileList;   // process the file input file list (in case it is desired to run multiple root files as input
  int returnCode = processCommandLineArgs( argc, argv, inFileList);
  if(returnCode!=0){
    cerr << "Error\n";
    return returnCode;
  }
  
  TChain *dataTree = new TChain("dataTree"); // open a TChain and add the opened root files to it
  for (unsigned int i = 0; i < inFileList.size(); ++i)
    dataTree->Add(inFileList[i].c_str());
  cout << "number of events: " << dataTree->GetEntries() << endl;  // check the number of entries in the TChain
  
  int sizeTDC_0,sizeTDC_1,sizeTDC_2;   // cuántos hits hubieron en un evento
  double TDC_LE_0[kHitsTDCmax];   // todos los flancos iniciales de los pulsos de un evento, canal 0 (leading edge)
  double TDC_LE_1[kHitsTDCmax];   // todos los flancos iniciales de los pulsos de un evento, canal 1
  double TDC_LE_2[kHitsTDCmax];
  
  double TDC_TE_0[kHitsTDCmax];   // todos los flancos finales de los pulsos de un evento, canal 0 (trailing edge)
  double TDC_TE_1[kHitsTDCmax];
  double TDC_TE_2[kHitsTDCmax];
  
  // determina el estado de las ramas del árbol de root //
  dataTree->SetBranchStatus("*",0);          // apaga todas las ramas
  dataTree->SetBranchStatus("sizeTDC_0",1);  // enciente la rama sizeTDC_0 
  dataTree->SetBranchStatus("sizeTDC_1",1);  // ...
  dataTree->SetBranchStatus("sizeTDC_2",1);
  dataTree->SetBranchStatus("TDC_LE_0",1);
  dataTree->SetBranchStatus("TDC_LE_1",1);
  dataTree->SetBranchStatus("TDC_LE_2",1);
  dataTree->SetBranchStatus("TDC_TE_0",1);
  dataTree->SetBranchStatus("TDC_TE_1",1);
  dataTree->SetBranchStatus("TDC_TE_2",1);
  
  // vincula las ramas del árbol de root con las variables declaradas más arriba
  dataTree->SetBranchAddress("sizeTDC_0",&sizeTDC_0);   // vincula la rama entre comillas llamada sizeTDC_0 con la direccón de memoria donde vive la variable previamente declarada (que hemos llamado igual por simplicidad)
  dataTree->SetBranchAddress("sizeTDC_1",&sizeTDC_1);
  dataTree->SetBranchAddress("sizeTDC_2",&sizeTDC_2);
  dataTree->SetBranchAddress("TDC_LE_0",&TDC_LE_0);
  dataTree->SetBranchAddress("TDC_LE_1",&TDC_LE_1);
  dataTree->SetBranchAddress("TDC_LE_2",&TDC_LE_2);
  dataTree->SetBranchAddress("TDC_TE_0",&TDC_TE_0);
  dataTree->SetBranchAddress("TDC_TE_1",&TDC_TE_1);
  dataTree->SetBranchAddress("TDC_TE_2",&TDC_TE_2);  
  
  { // Open the Ttree and index it event-by-event
    
	int n_events = (int) dataTree->GetEntries();

	/// Aqui empecé yo

	int nHits_chABC = 0;  
	int nHits_chAC = 0; 
	int ventana = 50; //Nanosegundos
	double efficiency = 0;   
     
	for(int i=0; i<n_events; i++){  // loop over the number of events start
      
		dataTree->GetEntry(i);   // pido que la entrada del árbol llene las variables previamente declaradas
     
		///////////////////////////////////////////////////

		if (sizeTDC_2 > 0 && sizeTDC_1 > 0 )
		{
			if ( fabs(TDC_LE_2[0]- TDC_LE_1[0]) <= ventana )
			{
				nHits_chAC++;

				if ( sizeTDC_0 > 0 )
				{
					if ( fabs(TDC_LE_0[0]- TDC_LE_1[0]) <= ventana ) nHits_chABC++ ;
				}

			}
		}		

		//////////////////////////////////////////////////////
  } // End of indexing the TTree
		efficiency = static_cast <double> (nHits_chABC) / nHits_chAC ;

		cout << "A^C" << "\t" << nHits_chAC << endl ;
		cout << "A^B^C" << "\t" << nHits_chABC << endl ;
		cout << "Eficiencia:" << "\t" << efficiency << endl ;
  }
  
  //   myApp.Run();
  return 0;  // main end
}
