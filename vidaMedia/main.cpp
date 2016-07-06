#include "Escaramujo.h"

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

	/// 
	/// Inicio
	/// 
	// Placas en Arreglo A-ch0  B-ch1 C-ch2

	int ventana = 50; //Nanosegundos
	int contadorFiltro = 0; 
	int veto = 500; //Nanosegundos
	ofstream salida;
	salida.open("filtro.dat"); //archivo de salida

     
	for(int i=0; i<n_events; i++){  // loop over the number of events start
      
		dataTree->GetEntry(i);   // pido que la entrada del árbol llene las variables previamente declaradas
     
		///////////////////////////////////////////////////

		if (sizeTDC_0==1 && sizeTDC_1==1 && sizeTDC_2==1) //Exactamente un conteo en cada placa
		{
			if ( (TDC_LE_2[0]- TDC_LE_0[0]) <= ventana ) // A y C en coincidencia
			{
				if ( (TDC_LE_1[0]- TDC_LE_2[0]) >= veto ) // Sólo se toman en cuenta pulsos con mayor duración al veto
				{
					salida << TDC_LE_1[0] - TDC_LE_2[0] << endl ; //Salida al archivo
					contadorFiltro++;
				}
			}
		}

  } // End of indexing the TTree

	//Razón decaimientos muón / eventos totales

		double razon = static_cast <double> (contadorFiltro) / n_events ;

		cout << razon << endl;

		salida.close();
  }

  
  return 0;  // main end
}
