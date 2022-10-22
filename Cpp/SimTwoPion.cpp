// The code run over the simulation generated with the code GetSimpleTuple
// https://github.com/utfsm-eg2-data-analysis/GetSimpleTuple
// It can be compile with
// g++ -Wall -fPIC  `root-config --cflags` SimTwoPion.cpp -o ./bin/SimTwoPion `root-config --glibs`
// For the target name use (D,C,Fe,Pb)

#include <iostream>
#include <string>
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TVector2.h"
#include "TStopwatch.h"
#include "TROOT.h"

int main(int argc, char* argv[]) {

  if(argc != 2) {
    std::cout << "Insert (just) the target name as a parameter" << std::endl;
    return 0;
  }

  TStopwatch t;

  // For the Target name use (D,C,Fe,Pb)
  std::string target = argv[1];
  // Creating a array of chars instead of a string to use Form method
  int n = target.length();
  char targetArr[n + 1];
  strcpy(targetArr, target.c_str());
  int dummyval = -999; // Value for the variables of pions that was not Gen/Rec  

  std::cout << "Start" << std::endl;

  TString inputName;

  // Set the variables that we want to save
  const char* VarList = "Gen:Q2_gen:Nu_gen:Zh_gen_1:Pt2_gen_1:PhiPQ_gen_1:Zh_gen_2:Pt2_gen_2:PhiPQ_gen_2:Zh_gen:Pt2_gen:PhiPQ_gen:Rec:Q2_rec:Nu_rec:Zh_rec_1:Pt2_rec_1:PhiPQ_rec_1:Zh_rec_2:Pt2_rec_2:PhiPQ_rec_2:Zh_rec:Pt2_rec:PhiPQ_rec";
  float *vars = new Float_t[24];
  TNtuple* outputTuple = new TNtuple("ntuple_sim", "", VarList);
  for(int folder = 1; folder < 10; folder++) { // Loops in every directory
    for(int sim = 1; sim < 500; sim++) { // Loops in every simulation of the directory
      // Set the name of the file where is the data depends on the target and the folder
      if(targetArr[0] == 'D' ){
        if(folder < 4) {
          inputName = Form("/work/mbarrial/out/GetSimpleTuple_HSim/D2_pb%i/prunedD_%i.root", folder, sim);
        } else {
          inputName = Form("/work/mbarrial/out/GetSimpleTuple_HSim/D2_pb%i_yshiftm03/prunedD_%i.root", folder, sim);
        }
      } else {
        if(folder < 4) {
          inputName = Form("/work/mbarrial/out/GetSimpleTuple_HSim/%s%i/pruned%s_%i.root", targetArr,folder,targetArr,sim);
        } else {
          inputName = Form("/work/mbarrial/out/GetSimpleTuple_HSim/%s%i_yshiftm03/pruned%s_%i.root", targetArr,folder,targetArr,sim);
        }
      }
      //std::cout << "Checking directory " << folder << "  " << sim << std::endl;
      //inputName = Form("/home/matias/proyecto/Piones/Data/Simul/pruned%s_%i.root",targetArr ,sim);
      // Open the file and check if it's exist
      TFile* fSource = new TFile(inputName,"READ");
      if (fSource->IsZombie()) {
        fSource->Close();
        continue;
      }
      // Open the tuple and check if it's exist
      TNtuple* simulTuple = (TNtuple*)fSource->Get("ntuple_sim");
      if(simulTuple == NULL) {
        delete simulTuple;
        fSource->Close();
        continue;
      }

      gROOT->cd();
      
      float tmpEvnt, evnt;
      float pidGen, pidRec, Q2Gen, NuGen, Pt2Gen, PhiGen, ZhGen;
      float Q2Rec, NuRec, Pt2Rec, PhiRec, ZhRec, deltaZ;
      // Read the necesary variables
      simulTuple->SetBranchAddress("evnt",&evnt);
      simulTuple->SetBranchAddress("mc_pid",&pidGen);
      simulTuple->SetBranchAddress("pid",&pidRec);
      simulTuple->SetBranchAddress("mc_Q2",&Q2Gen);
      simulTuple->SetBranchAddress("mc_Nu",&NuGen);
      simulTuple->SetBranchAddress("mc_Zh",&ZhGen);
      simulTuple->SetBranchAddress("mc_Pt2",&Pt2Gen);
      simulTuple->SetBranchAddress("mc_PhiPQ",&PhiGen);
      simulTuple->SetBranchAddress("Q2",&Q2Rec);
      simulTuple->SetBranchAddress("Nu",&NuRec);
      simulTuple->SetBranchAddress("Zh",&ZhRec);
      simulTuple->SetBranchAddress("Pt2",&Pt2Rec);
      simulTuple->SetBranchAddress("PhiPQ",&PhiRec);
      simulTuple->SetBranchAddress("deltaZ",&deltaZ);
      // Create the variables to use inside of the for loops
      //vars[0] = 0; // Count how many pions were generated in the event
      //vars[1] = 0; // Count how many pions were detected in the event
      int tmpCounter = 0;
      //float tmpEvnt;
      //float tmpZh[5], tmpPt[5], tmpPhi[5] ;
      int isPion;
  


      for(int i = 0; i < simulTuple->GetEntries(); i++) { // Loops in every generated particle
	isPion = 0; // If is diferent than 0 there is a generated or reconstructed pion
	simulTuple->GetEntry(i);

	// Set all varibles at dummyVal at start to avoid errors
	for(int j = 0; j < 24; j++) {
	  vars[j] = dummyval;
	}

	vars[0]  = 0; // number of Gen Pion 
	vars[12] = 0; // number of Rec Pion
	
      

        // Check if the generated paricle is a pion+
	if(pidGen == 211 || (pidRec == 211 && TMath::Abs(deltaZ) < 3)) {
	  isPion++;
	  if(pidGen == 211) {
	    // Save the angle PhiPQ,Zh and Pt if it's a pion
	    vars[0] = 1;
	    vars[1] = Q2Gen;
	    vars[2] = NuGen;
	    vars[3] = ZhGen;
	    vars[4] = Pt2Gen;
	    vars[5] = PhiGen;
	  } else{
	    for(int i = 1; i <= 5; i++) { vars[i] = dummyval; }
	  }
	  if(pidRec == 211) {
	    // Save the angle PhiPQ,Zh and Pt if it's a pion
	    vars[12] = 1;
	    vars[13] = Q2Rec;
	    vars[14] = NuRec;
	    vars[15] = ZhRec;
	    vars[16] = Pt2Rec;
	    vars[17] = PhiRec;
	  } else {
	    for(int i = 7; i <= 11; i++) { vars[i] = dummyval; }
	  }
	}

        tmpEvnt = evnt;
        simulTuple->GetEntry(i + 1);
        // Check if the next particle cames from the same event
        while(tmpEvnt == evnt) { // Check all the paricles in the event
	  if((pidGen == 211 || (pidRec == 211 && TMath::Abs(deltaZ) < 3)) && (isPion < 2)) {
	    if(pidGen == 211) {
	      // Save the angle PhiPQ,Zh and Pt if it's a pion
	      vars[0]++;
	      vars[1] = Q2Gen;
	      vars[2] = NuGen;
	      vars[isPion*3+3] = ZhGen;
	      vars[isPion*3+4] = Pt2Gen;
	      vars[isPion*3+5] = PhiGen;
	    } else{
	      vars[isPion*3+3] = dummyval;
	      vars[isPion*3+4] = dummyval;
	      vars[isPion*3+5] = dummyval;
	    }
	    if(pidRec == 211) {
	      // Save the angle PhiPQ,Zh and Pt if it's a pion
	      vars[12]++;
	      vars[13] = Q2Rec;
	      vars[14] = NuRec;
	      vars[isPion*3+15] = ZhRec;
	      vars[isPion*3+16] = Pt2Rec;
	      vars[isPion*3+17] = PhiRec;
	    } else {
	      vars[isPion*3+15] = dummyval;
	      vars[isPion*3+16] = dummyval;
	      vars[isPion*3+17] = dummyval;
	    }
	    isPion++;
	  }
	    tmpCounter++;
	    tmpEvnt = evnt;
	    // Go to the next particle
	    if(i + 1 + tmpCounter > simulTuple->GetEntries() ){ break; }
	    simulTuple->GetEntry(i + 1 + tmpCounter);
	  
	}
	// If there is just one detected pion put it first
	float tmpZh, tmpPt2, tmpPhi;
	if(int(vars[0]) == 1) {
	  if(vars[3] == dummyval) {
	    vars[3] = vars[6];
	    vars[4] = vars[7];
	    vars[5] = vars[8];
	    vars[6] = dummyval;
	    vars[7] = dummyval;
	    vars[8] = dummyval;
	    tmpZh   = vars[18];
	    tmpPt2  = vars[19];
	    tmpPhi  = vars[20];
	    vars[18] = vars[15];
	    vars[19] = vars[16];
	    vars[20] = vars[17];
	    vars[15] = tmpZh;
	    vars[16] = tmpPt2;
	    vars[17] = tmpPhi;
	  } 
	}

	// If there is not Gen pion set sum variables = dummyval
	if(vars[0] == 0){
	  vars[9]  = dummyval;
	  vars[10] = dummyval;
	  vars[11] = dummyval;
	}

	// If there is not Rec pion set sum variables = dummyval
	if(vars[12] == 0){
	  vars[21] = dummyval;
	  vars[22] = dummyval;
	  vars[23] = dummyval;
	}
	
	// If there is one Gen pion set sum variables = val first pion
	if(vars[0] == 1){
	  vars[9]  = vars[3];
	  vars[10] = vars[4];
	  vars[11] = vars[5];
	}
        

	// If there is one Rec pion set sum variables = val non null pion
	if(vars[0] == 1) {
	  if(vars[15] != dummyval) {
	    vars[21] = vars[15];
	    vars[22] = vars[16];
	    vars[23] = vars[17];  
	  } else {
	    vars[21] = vars[18];
	    vars[22] = vars[19];
	    vars[23] = vars[20];  
	  }
	
	}
	
	
        if(vars[2] == 2) {
	  TVector2* vec = new TVector2(0,0);
	  for(int k = 0; k < 2; k++) {
	    // Calculate de tranvers momentum vector
	    TVector2 *tmpVec = new TVector2(TMath::Sqrt(vars[k*3+4])*TMath::Cos((vars[k*3+5] + 180)*TMath::DegToRad()), TMath::Sqrt(vars[k*3+4])*TMath::Sin((vars[k*3+5] + 180)*TMath::DegToRad()));
	    // Sum the vector and save the sum of Zh
	    *vec += *tmpVec;
	    delete tmpVec;
	  }
	  vars[9]  = vars[3]+vars[6];
	  vars[10] = std::pow(vec->Mod(),2);
	  vars[11] = vec->Phi()*TMath::RadToDeg()-180;
	  delete vec;
	}
	if(vars[2] == 2) {
	  TVector2* vec = new TVector2(0,0);
	  for(int k = 0; k < 2; k++) {
	    // Calculate de tranvers momentum vector
	    TVector2 *tmpVec = new TVector2(TMath::Sqrt(vars[k*3+16])*TMath::Cos((vars[k*3+17] + 180)*TMath::DegToRad()), TMath::Sqrt(vars[k*3+16])*TMath::Sin((vars[k*3+17]+ 180)*TMath::DegToRad()));
	    // Sum the vector and save the sum of Zh
	    *vec += *tmpVec;
	    delete tmpVec;
	  }
	  vars[21] = vars[15]+vars[18];
	  vars[22] = std::pow(vec->Mod(),2);
	  vars[23] = vec->Phi()*TMath::RadToDeg()-180;
	  delete vec;
	}

        // Add the variables to the tuple
        // Jump to the next event
        i += tmpCounter;
        tmpCounter = 0;
	if(isPion > 0) {
	  outputTuple->Fill(vars);
	}

      } // End part icles loop
      delete simulTuple;
      fSource->Close();
    } // End sim loop
    std::cout << "Directory " << folder << " checked" << std::endl;
  } // End folder loop

  // Save the Ntuple
  TFile *fileOutput= new TFile(Form("/work/mbarrial/Omnifold/Data/OF_SIM_TWO_%s.root", targetArr), "RECREATE");
  //TFile *fileOutput= new TFile("/home/matias/proyecto/Omnifold/Data/OF_SIM_TWO_C.root", "RECREATE");
  fileOutput->cd();
  outputTuple->Write();
  gROOT->cd();
  fileOutput->Close();
  t.Print();

}

