// It can be compile with
// g++ -Wall -fpic  `root-config --cflags` SplitTwoPion.cpp -o ./bin/SplitTwoPion `root-config --glibs`
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

  TFile *simFile = new TFile(Form("/home/matias/proyecto/Omnifold/Data/OF_SIM_TWO_%s.root", targetArr), "READ");
  

  const char* VarList = "Gen:Q2_gen:Nu_gen:Zh_gen_1:Pt2_gen_1:PhiPQ_gen_1:Zh_gen_2:Pt2_gen_2:PhiPQ_gen_2:Zh_gen:Pt2_gen:PhiPQ_gen:Rec:Q2_rec:Nu_rec:Zh_rec_1:Pt2_rec_1:PhiPQ_rec_1:Zh_rec_2:Pt2_rec_2:PhiPQ_rec_2:Zh_rec:Pt2_rec:PhiPQ_rec";
  float *vars = new Float_t[24];

  TNtuple *simulTuple = (TNtuple*) simFile->Get("ntuple_sim");
  if(simulTuple==NULL){std::cout <<"la ptm, que paso ahora\n";}
  // Read the necesary variables
  //simulTuple->SetBranchAddress("evnt",&evnt);
  simulTuple->SetBranchAddress("Gen"         , &vars[0]);
  simulTuple->SetBranchAddress("Q2_gen"      , &vars[1]);
  simulTuple->SetBranchAddress("Nu_gen"      , &vars[2]);
  simulTuple->SetBranchAddress("Zh_gen_1"    , &vars[3]);
  simulTuple->SetBranchAddress("Pt2_gen_1"   , &vars[4]);
  simulTuple->SetBranchAddress("PhiPQ_gen_1" , &vars[5]);
  simulTuple->SetBranchAddress("Zh_gen_2"    , &vars[6]);
  simulTuple->SetBranchAddress("Pt2_gen_2"   , &vars[7]);
  simulTuple->SetBranchAddress("PhiPQ_gen_2" , &vars[8]);
  simulTuple->SetBranchAddress("Zh_gen"      , &vars[9]);
  simulTuple->SetBranchAddress("Pt2_gen"     , &vars[10]);
  simulTuple->SetBranchAddress("PhiPQ_gen"   , &vars[11]);
  simulTuple->SetBranchAddress("Rec"         , &vars[12]);
  simulTuple->SetBranchAddress("Q2_rec"      , &vars[13]);
  simulTuple->SetBranchAddress("Nu_rec"      , &vars[14]);
  simulTuple->SetBranchAddress("Zh_rec_1"    , &vars[15]);
  simulTuple->SetBranchAddress("Pt2_rec_1"   , &vars[16]);
  simulTuple->SetBranchAddress("PhiPQ_rec_1" , &vars[17]);
  simulTuple->SetBranchAddress("Zh_rec_2"    , &vars[18]);
  simulTuple->SetBranchAddress("Pt2_rec_2"   , &vars[19]);
  simulTuple->SetBranchAddress("PhiPQ_rec_2" , &vars[20]);
  simulTuple->SetBranchAddress("Zh_rec"      , &vars[21]);
  simulTuple->SetBranchAddress("Pt2_rec"     , &vars[22]);
  simulTuple->SetBranchAddress("PhiPQ_rec"   , &vars[23]);

  TFile *FHfile = new TFile(Form("/home/matias/proyecto/Omnifold/Data/OF_SIM_TWO_%s_1.root", targetArr), "RECREATE");
  gROOT->cd();
  TNtuple *FHtuple = new TNtuple("ntuple_sim", "", VarList);
  //int splitEvnt = simulTuple->GetEntries()/2 
  int splitEvnt = 800000 ;
  for(int i = 0; i < splitEvnt; i++) { 
    simulTuple->GetEntry(i);
    FHtuple->Fill(vars);
  }

  FHfile->cd();
  FHtuple->Write();
  gROOT->cd();
  
  FHfile->Close(); 
  delete FHtuple;

  TFile *SHfile = new TFile(Form("/home/matias/proyecto/Omnifold/Data/OF_SIM_TWO_%s_2.root", targetArr), "RECREATE");
  gROOT->cd();
  TNtuple *SHtuple = new TNtuple("ntuple_sim", "", VarList);
  for(int j = splitEvnt; j < /*simulTuple->GetEntries()*/ 2*splitEvnt; j++) { 
    simulTuple->GetEntry(j);
    SHtuple->Fill(vars); 
  }

  SHfile->cd();
  SHtuple->Write();
  gROOT->cd();
  
  SHfile->Close(); 
  delete SHtuple;
  
  simFile->Close();

  return 0;

}
