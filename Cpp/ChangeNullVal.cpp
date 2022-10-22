// It can be compile with
// g++ -Wall -fPIC  `root-config --cflags` ChangeNullVal.cpp -o ./bin/ChangeNullVal `root-config --glibs`
// For the target name use (D,C,Fe,Pb)

#include <iostream>
#include <string>
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
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

  TFile *simFile = new TFile(Form("/home/matias/proyecto/Omnifold/Data/OF_SIM_%s.root", targetArr), "READ");
  

  const char* VarList = "Gen:Q2_gen:Nu_gen:Zh_gen:Pt2_gen:PhiPQ_gen:Rec:Q2_rec:Nu_rec:Zh_rec:Pt2_rec:PhiPQ_rec";
  float *vars = new Float_t[12];

  TNtuple *simulTuple = (TNtuple*) simFile->Get("ntuple_sim");
  if(simulTuple==NULL){std::cout <<"la ptm, que paso ahora\n";}
  // Read the necesary variables
  //simulTuple->SetBranchAddress("evnt",&evnt);
  simulTuple->SetBranchAddress("Gen"       , &vars[0]);
  simulTuple->SetBranchAddress("Q2_gen"    , &vars[1]);
  simulTuple->SetBranchAddress("Nu_gen"    , &vars[2]);
  simulTuple->SetBranchAddress("Zh_gen"    , &vars[3]);
  simulTuple->SetBranchAddress("Pt2_gen"   , &vars[4]);
  simulTuple->SetBranchAddress("PhiPQ_gen" , &vars[5]);
  simulTuple->SetBranchAddress("Rec"       , &vars[6]);
  simulTuple->SetBranchAddress("Q2_rec"    , &vars[7]);
  simulTuple->SetBranchAddress("Nu_rec"    , &vars[8]);
  simulTuple->SetBranchAddress("Zh_rec"    , &vars[9]);
  simulTuple->SetBranchAddress("Pt2_rec"   , &vars[10]);
  simulTuple->SetBranchAddress("PhiPQ_rec" , &vars[11]);

  int newDummyVal = 9999;

  TFile *outfile = new TFile(Form("/home/matias/proyecto/Omnifold/Data/OF_SIM_C-2.root", targetArr), "RECREATE");
  gROOT->cd();
  TNtuple *outTuple = new TNtuple("ntuple_sim", "", VarList);
  for(int i = 0; i < simulTuple->GetEntries(); i++) { 
    simulTuple->GetEntry(i);
    if (vars[0] == 0) {
      vars[1] = newDummyVal; 
      vars[2] = newDummyVal; 
      vars[3] = newDummyVal; 
      vars[4] = newDummyVal; 
      vars[5] = newDummyVal; 
    }

    if (vars[6] == 0) {
      vars[7]  = newDummyVal; 
      vars[8]  = newDummyVal; 
      vars[9]  = newDummyVal; 
      vars[10] = newDummyVal; 
      vars[11] = newDummyVal; 
    }
    outTuple->Fill(vars);
  }

  outfile->cd();
  outTuple->Write();
  gROOT->cd();
  
  outfile->Close(); 
  delete FHtuple;
  simFile->Close();

  return 0;

}
