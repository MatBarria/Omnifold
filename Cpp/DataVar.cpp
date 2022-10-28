// This code generate a tuple with all the events of experimental data
// Saves the electron variables and calculate for the hadrons variables
// calculate vectorial momentum and use it as the hadron momentum for the event
// The code require that you have the number of the event saved in the data tupleName
// if you don't have it you can check by for the paricle has the same Q2 and Nu instead
// It can be compiled with
// g++ -Wall -fPIC -I./include `root-config --cflags` DataVar.cpp -o ./bin/DataVar  `root-config --glibs`
// For the target name use (C,Fe,Pb)

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
  std::cout << "Start" << std::endl;

  std::string target = argv[1];
  // Creating a array of chars instead of a string to use Form method
  int n = target.length();
  char targetArr[n + 1];
  strcpy(targetArr, target.c_str());

  // TFile* file = new TFile(Form("~/proyecto/Pt2Broadening_multi-pion/Data/PiPlusData_%s.root", targetArr), "READ");
  TFile* file = new TFile(Form("~/proyecto/Pt2Broadening_multi-pion/Data/PiPlusData_%sdeltaZ.root", targetArr), "READ");
  TNtuple* tuple = (TNtuple*)file->Get("ntuple_data");

  int tmpCounter = 0; // Counts how many partivles there is in the event
  float tmpEvnt, evnt, Q2Evnt, NuEvnt, ZhEvnt, Pt2Evnt, PhiEvnt, YCEvnt, VCEvnt, Vc;

  int dummyval = -999;
  const char* VarList = "Q2:Nu:Zh_1:Zh_2:Pt2_1:Pt2_2:PhiPQ_1:PhiPQ_2";
  // Variables to fill the tuple
  float *vars         = new Float_t[8];
  // Read the necesary variables
  tuple->SetBranchAddress("Q2",&Q2Evnt);
  tuple->SetBranchAddress("Nu",&NuEvnt);
  tuple->SetBranchAddress("Zh",&ZhEvnt);
  tuple->SetBranchAddress("Pt2",&Pt2Evnt);
  tuple->SetBranchAddress("PhiPQ",&PhiEvnt);
  tuple->SetBranchAddress("YC",&YCEvnt);
  tuple->SetBranchAddress("VC_TM",&VCEvnt);
  tuple->SetBranchAddress("NEvnt",&evnt);

  gROOT->cd();

  TNtuple* ntuplePionSol = new TNtuple("ntuple_pion" , "", VarList);
  TNtuple* ntuplePionLiq = new TNtuple("ntuple_pion" , "", VarList);
 
  for(int i = 0; i < tuple->GetEntries() ; i++) { // Loops in every detected paricle
    tuple->GetEntry(i);
    if(TMath::Abs(YCEvnt) > 1.4) { continue; }
    vars[0] = Q2Evnt;
    vars[1] = NuEvnt;
    vars[2] = ZhEvnt;
    vars[4] = Pt2Evnt;
    vars[6] = PhiEvnt;
    Vc = VCEvnt;
    tmpEvnt = evnt;
    tuple->GetEntry(i + 1);
    while(tmpEvnt == evnt) { // Check all the paricles in the event
      tmpCounter++;
      if(tmpCounter == 1) {
	vars[3] = ZhEvnt;
	vars[5] = Pt2Evnt;
	vars[7] = PhiEvnt;
      }
      if(i + 1 + tmpCounter >= tuple->GetEntries() ){ break; }
      tuple->GetEntry(i + 1 + tmpCounter);
    }
    if(tmpCounter == 0) {
	vars[3] = dummyval;
	vars[5] = dummyval;
	vars[7] = dummyval;
    }
    if(Vc == 2) { ntuplePionSol->Fill(vars);  }
    if(Vc == 1) { ntuplePionLiq->Fill(vars);  }
    // Jump to the next event
    i += tmpCounter;
    tmpCounter = 0;
  } // End paricle loop

  // Save the tuples
  TFile* fOutputSol = new TFile(Form("~/proyecto/Omnifold/Data/OF_Data_%s.root", targetArr), "RECREATE");
  fOutputSol->cd();
  
  ntuplePionSol->Write();

  gROOT->cd();
  fOutputSol->Close();
  TFile* fOutputLiq = new TFile(Form("~/proyecto/Omnifold/Data/OF_Data_D%s.root", targetArr), "RECREATE");
  fOutputLiq->cd();
  
  ntuplePionLiq->Write();

  gROOT->cd();
  fOutputLiq->Close();
  std::cout << "Done." << std::endl;
  file->Close();
  t.Print();

}
