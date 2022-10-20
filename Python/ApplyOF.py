from __future__ import absolute_import, division, print_function

import numpy as np
from matplotlib import pyplot as plt

from sklearn.model_selection import train_test_split

import tensorflow as tf

from tensorflow.keras.layers import Dense, Input
from tensorflow.keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping

import matplotlib as mpl
mpl.rcParams.update(mpl.rcParamsDefault)
mpl.rcParams["text.usetex"]=False 

import uproot

    
inputDirectory  = "/home/matias/proyecto/Pt2Broadening_multi-pion/Data/"
# file = uproot.open(inputDirectory + "VecSum_CdeltaZ.root")
def reweight(events):
    f = model.predict(events, batch_size=10000)
    weights = f / (1. - f)
    return np.squeeze(np.nan_to_num(weights))

# num_pion = 2

num_pion = 1
# sim_vars = ['Q2', 'Nu', 'Zh', 'Pt2', 'PhiPQ', 'VC_TM', 'YC']
# sim_gen_vars = ['Pt2_gen']
# sim_rec_vars = ['Pt2_sim']

sim_gen_vars = ['Pt2']
sim_rec_vars = ['Pt2']

dummyval = -10 #a value for examples that don't pass one of the measured/ideal selections

# "OF_SIM_C_1.root:ntuple_sim"
for i in range(num_pion):
    with uproot.open(inputDirectory + "VecSum_CdeltaZ.root:ntuple_" + str(i+1) + "_pion") as file:
        sim_gen = file.arrays(sim_gen_vars, library = "np")['Pt2']
        sim_rec = file.arrays(sim_rec_vars, library = "np")['Pt2']
    with uproot.open(inputDirectory + "VecSum_CdeltaZ.root:ntuple_" + str(i+1) + "_pion") as file:
        data_gen = file.arrays(sim_gen_vars, library = "np")['Pt2']
        data_rec = file.arrays(sim_rec_vars, library = "np")['Pt2']

sim = np.stack([sim_gen, sim_rec], axis = 1)
labels_sim = np.zeros(len(sim))

data = np.stack([data_gen, data_rec], axis = 1)
labels_data = np.ones(len(data))

inputs = Input((1, ))
hidden_layer_1 = Dense(50, activation='relu')(inputs)
hidden_layer_2 = Dense(50, activation='relu')(hidden_layer_1)
hidden_layer_3 = Dense(50, activation='relu')(hidden_layer_2)
outputs = Dense(1, activation='sigmoid')(hidden_layer_3)

print(inputs)

model = Model(inputs=inputs, outputs=outputs)

earlystopping = EarlyStopping(patience=10,
                              verbose=1,
                              restore_best_weights=True)


# initial iterative weights are ones
weights_pull = np.ones(len(sim_gen))
weights_push = np.ones(len(sim_gen))
weights_data = np.ones(len(sim_gen))

# detector level features/labels
xvals_rec = np.concatenate((sim_rec, data_rec[data_rec != dummyval])) 
yvals_rec = np.concatenate((labels_sim, np.ones(len(data_rec[data_rec != 1]))))

# detector level features/labels
xval_dec = np.concatenate((sim_gen, sim_gen)) # generated + generated
yvals_dec = np.concatenate((labels_sim, labels_data))

iterations = 4
for i in range(iterations):
    print("\nITERATION: {}\n".format(i + 1))

    # STEP 1: classify Sim. (which is reweighted by weights_push) to Data
    # weights reweighted Sim. --> Data
    print("STEP 1\n")

    weights_1 = np.concatenate((weights_push, weights_data))
    #QUESTION: concatenation here confuses me
    # actual weights for Sim., ones for Data (not MC weights)

    X_train_1, X_test_1, Y_train_1, Y_test_1, w_train_1, w_test_1 = train_test_split(
        xvals_rec, yvals_rec, weights_1) #REMINDER: made up of synthetic+measured

    model.compile(loss='binary_crossentropy',
                  optimizer='Adam',
                  metrics=['accuracy'])
    model.fit(X_train_1[X_train_1!=dummyval],
              Y_train_1[X_train_1!=dummyval],
              sample_weight=w_train_1[X_train_1!=dummyval],
              epochs=200,
              batch_size=10000,
              validation_data=(X_test_1[X_test_1!=dummyval], Y_test_1[X_test_1!=dummyval], w_test_1[X_test_1!=dummyval]),
              callbacks=[earlystopping],
              verbose=1)

    weights_pull = weights_push * reweight(sim_rec) 
    #QUESTION: above model used in reweight function (model.predict)?
    #QUESTION: Model trains until synthetic is indistinguishable from data? How does this work? 
    #How are weights then iteratively multiplied?
    
    ###
    #Need to do something with events that don't pass reco.
    
    #One option is to take the prior:
    #weights_pull[sim_rec==dummyval] = 1. 
    
    #Another option is to assign the average weight: <w|x_true>.  To do this, we need to estimate this quantity.
    xvals_1b = np.concatenate([sim_gen[sim_rec != dummyval], sim_gen[sim_rec != dummyval]])
    yvals_1b = np.concatenate([np.ones(len(sim_gen[sim_rec != dummyval])),np.zeros(len(sim_gen[sim_rec != dummyval]))])
    weights_1b = np.concatenate([weights_pull[sim_rec != dummyval], np.ones(len(sim_gen[sim_rec != dummyval]))])
    
    X_train_1b, X_test_1b, Y_train_1b, Y_test_1b, w_train_1b, w_test_1b = train_test_split(
        xvals_1b, yvals_1b, weights_1b)    
    
    model.compile(loss='binary_crossentropy',
                  optimizer='Adam',
                  metrics=['accuracy'])
    model.fit(X_train_1b,
              Y_train_1b,
              sample_weight=w_train_1b,
              epochs=200,
              batch_size=10000,
              validation_data=(X_test_1b, Y_test_1b, w_test_1b),
              callbacks=[earlystopping],
              verbose=1)
    
    average_vals = reweight(sim_gen[sim_rec == dummyval])
    weights_pull[sim_rec == dummyval] = average_vals
    ###
    
    weights[i, :1, :] = weights_pull

    # STEP 2: classify Gen. to reweighted Gen. (which is reweighted by weights_pull)
    # weights Gen. --> reweighted Gen.
    print("\nSTEP 2\n")

    weights_2 = np.concatenate((np.ones(len(sim_gen)), weights_pull))
    # ones for Gen. (not MC weights), actual weights for (reweighted) Gen.

    X_train_2, X_test_2, Y_train_2, Y_test_2, w_train_2, w_test_2 = train_test_split(
        xval_dec, yvals_dec, weights_2)

    model.compile(loss='binary_crossentropy',
                  optimizer='Adam',
                  metrics=['accuracy'])
    model.fit(X_train_2,
              Y_train_2,
              sample_weight=w_train_2,
              epochs=200,
              batch_size=2000,
              validation_data=(X_test_2, Y_test_2, w_test_2),
              callbacks=[earlystopping],
              verbose=1)

    weights_push = reweight(sim_gen)
    ###
    #Need to do something with events that don't pass truth    
    
    #One option is to take the prior:
    #weights_push[sim_gen==dummyval] = 1. 
    
    #Another option is to assign the average weight: <w|x_reco>.  To do this, we need to estimate this quantity.
    xvals_1b = np.concatenate([sim_rec[sim_gen != dummyval], sim_rec[sim_gen != dummyval]])
    yvals_1b = np.concatenate([np.ones(len(sim_rec[sim_gen != dummyval])), np.zeros(len(sim_rec[sim_gen != dummyval]))])
    weights_1b = np.concatenate([weights_push[sim_gen != dummyval], np.ones(len(sim_rec[sim_gen != dummyval]))])
    
    X_train_1b, X_test_1b, Y_train_1b, Y_test_1b, w_train_1b, w_test_1b = train_test_split(
        xvals_1b, yvals_1b, weights_1b)    
    
    model.compile(loss='binary_crossentropy',
                  optimizer='Adam',
                  metrics=['accuracy'])
    model.fit(X_train_1b,
              Y_train_1b,
              sample_weight=w_train_1b,
              epochs=200,
              batch_size=10000,
              validation_data=(X_test_1b, Y_test_1b, w_test_1b),
              callbacks=[earlystopping],
              verbose=1)
    
    average_vals = reweight(sim_rec[sim_geni == dummyval])
    weights_push[sim_gen == dummyval] = average_vals
    ###    
    
    weights[i, 1:2, :] = weights_push
# print(sim)
# print(data)
# print(labels_sim)
# print(labels_data)
print(xvals_rec)
print(yvals_rec)

