// The code run over the simulation generated with the code GetSimpleTuple
// https://github.com/utfsm-eg2-data-analysis/GetSimpleTuple
// It can be compile with
// g++ -Wall -fPIC  `root-config --cflags` SimOnePion.cpp -o ./bin/SimOnePion `root-config --glibs`
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
  int dummyval = -999;

  std::cout << "Start" << std::endl;

  TString inputName;

  // Set the variables that we want to save
  const char* VarList = "Gen:Q2_gen:Nu_gen:Zh_gen:Pt2_gen:PhiPQ_gen:Rec:Q2_rec:Nu_rec:Zh_rec:Pt2_rec:PhiPQ_rec";
  float *vars = new Float_t[12];
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
      
      //float evnt;
      float pidGen, pidRec, Q2Gen, NuGen, Pt2Gen, PhiGen, ZhGen, Q2Rec, NuRec, Pt2Rec, PhiRec, ZhRec;

      // Read the necesary variables
      //simulTuple->SetBranchAddress("evnt",&evnt);
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
      // Create the variables to use inside of the for loops
      //vars[0] = 0; // Count how many pions were generated in the event
      //vars[1] = 0; // Count how many pions were detected in the event
      //int tmpCounter = 0;
      //float tmpEvnt;
      //float tmpZh[5], tmpPt[5], tmpPhi[5] ;
      int isPion;
      for(int i = 0; i < simulTuple->GetEntries(); i++) { // Loops in every generated particle
        isPion = 0; // If is diferent than 0 there is a generated or reconstructed pion
	simulTuple->GetEntry(i);
        // Check if the generated paricle is a pion+
	if(pidGen == 211) {
          // Save the angle PhiPQ,Zh and Pt if it's a pion
	  vars[0] = 1;
	  vars[1] = Q2Gen;
	  vars[2] = NuGen;
	  vars[3] = ZhGen;
	  vars[4] = Pt2Gen;
	  vars[5] = PhiGen;
	  isPion++;
	} else{
	  vars[0] = 0;
	  for(int i = 1; i <= 5; i++) { vars[i] = dummyval; }
	}
	if(pidRec == 211) {
          // Save the angle PhiPQ,Zh and Pt if it's a pion
	  vars[6] = 1;
	  vars[7] = Q2Rec;
	  vars[8] = NuRec;
	  vars[9] = ZhRec;
	  vars[10] = Pt2Rec;
	  vars[11] = PhiRec;
	  isPion++;
	} else{
	  vars[6] = 0;
	  for(int i = 7; i <= 11; i++) { vars[i] = dummyval; }
	}

	if(isPion > 0) {
	  outputTuple->Fill(vars);
	}

      } // End particles loop
      delete simulTuple;
      fSource->Close();
    } // End sim loop
    std::cout << "Directory " << folder << " checked" << std::endl;
  } // End folder loop

  // Save the Ntuple
  TFile *fileOutput= new TFile(Form("/work/mbarrial/Omnifold/Data/OF_SIM_%s.root", targetArr), "RECREATE");
  //TFile *fileOutput= new TFile("/home/matias/proyecto/Omnifold/Data/OF_SIM_C.root", "RECREATE");
  fileOutput->cd();
  outputTuple->Write();
  gROOT->cd();
  fileOutput->Close();
  t.Print();

}

