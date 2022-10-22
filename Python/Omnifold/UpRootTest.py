import numpy as np
import ROOT
import uproot
    
inputDirectory  = "/home/matias/proyecto/Pt2Broadening_multi-pion/Data/"
# file = uproot.open(inputDirectory + "VecSum_CdeltaZ.root")


num_pion = 2

variables = ['Q2', 'Nu', 'Zh', 'Pt2', 'PhiPQ', 'VC_TM', 'YC']

pions_dic = [{}, {}, {}]

for i in range(num_pion):
    with uproot.open(inputDirectory + "VecSum_CdeltaZ.root:ntuple_" + str(i+1) + "_pion") as file:
        pions_dic[i] = file.arrays(variables, library = "np")



print(pions_dic)


