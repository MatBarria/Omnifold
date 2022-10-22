import numpy as np
import matplotlib.pyplot as plt
import energyflow as ef
import energyflow.archs

import omnifold
import modplot
import ibu
import uproot

plt.rcParams['figure.figsize'] = (4,4)
plt.rcParams['figure.dpi'] = 120
plt.rcParams['font.family'] = 'serif'

#Directory where the simulations is storeded
data_directory  = "/home/matias/proyecto/Omnifold/Data/"

#Target
target = "C"

# This are the variables thar you want to safe in the final tuple 
# you must have the Generated values(gen), the reconstructed/detected values(rec) 
vars = ['Gen', 'Q2_gen', 'Nu_gen', 'Pt2_gen', 'Zh_gen', 'Pt2_gen', 'PhiPQ_gen',
        'Rec', 'Q2_rec', 'Nu_rec', 'Pt2_rec', 'Zh_rec', 'Pt2_rec', 'PhiPQ_rec']

# Value of the variables that was not reconstructed or generated
dummyval = -999.

# Open the files and saves the variables in dictionaries 
with uproot.open(data_directory + "OF_SIM_" + target + "_1.root:ntuple_sim") as file:
    sim = file.arrays(vars, library = "np")
with uproot.open(data_directory + "OF_SIM_" + target + "_2.root:ntuple_sim") as file:
    data = file.arrays(vars, library = "np")

# A dictionary of the dictionaries 
datasets = {'simul':sim,'data':data}
# choose what is MC(MonteCarlo/simulations) and Data in this context
synthetic, nature = datasets['simul'], datasets['data']
# how many iterations of the unfolding process
itnum = 3
# Phase space of the correction
obs_multifold = ['Q2', 'Nu', 'Pt2', 'Zh', 'Pt2', 'PhiPQ']

# a dictionary to hold information about the observables
obs = {}

# the Q2 and histogram style information (the func is there to add the arrays latter)
obs.setdefault('Q2', {}).update({
    'func': lambda dset, ptype: dset['Q2_' + ptype],
    'nbins_det': 30, 'nbins_mc': 30,
    'xlim': (1, 4.2), 'ylim': (0, 1.5),
    'xlabel': r'Q2 [$GeV^2$]', 'symbol': r'$Q^2$',
    'ylabel': r'Normalized Cross Section ',
    'stamp_xy': (0.425, 0.65),
})

# the Nu and histogram style information
obs.setdefault('Nu', {}).update({
    'func': lambda dset, ptype: dset['Nu_'+ ptype],
    'nbins_det': 30, 'nbins_mc': 30,
    'xlim': (2., 5), 'ylim': (0, 1),
    'xlabel': r'$\nu$[$GeV$]', 'symbol': r'$\nu$',
    'ylabel': r'Normalized Cross Section',
    'stamp_xy': (0.42, 0.65),
})

# the Zh and histogram style information
obs.setdefault('Zh', {}).update({
    'func': lambda dset, ptype: dset['Zh_' + ptype],
    'nbins_det': 30, 'nbins_mc':30,
    'xlim': (0, 1), 'ylim': (0, 4),
    'xlabel': r'$Z_h$', 'symbol': r'$Z_h$',
    'ylabel': r'Normalized Cross Section',
    'stamp_xy': (0.425, 0.65),
})

# the Pt2 ratio and histogram style information
obs.setdefault('Pt2', {}).update({ 
    'func': lambda dset, ptype: dset['Pt2_' + ptype],
    'nbins_det': 90, 'nbins_mc': 30,
    'xlim': (-0.01, 3), 'ylim': (0, 7),
    'xlabel': r'$Pt^2$[GeV]', 'symbol': r'$Pt^2$',
    'ylabel': r'Normalized Cross Section',
    'stamp_xy': (0.41, 0.92),
    'legend_loc': 'upper left', 'legend_ncol': 1,
})

# the PhiPQ fraction and histogram style information
obs.setdefault('PhiPQ', {}).update({
    'func': lambda dset, ptype: dset['PhiPQ_'+ ptype],
    'nbins_det': 30, 'nbins_mc': 30,
    'xlim': (-180, 180), 'ylim': (0, 0.01),
    'xlabel': r'PhiPQ[Deg]', 'symbol': r'$z_g$',
    'ylabel': 'Normalized Cross Section',
    'stamp_xy': (0.425, 0.65),
})


# additional histogram and plot style information
hist_style = {'histtype': 'step', 'density': True, 'lw': 1, 'zorder': 2}
gen_style = {'linestyle': '-', 'color': 'blue', 'lw': 1.15, 'label': 'Gen.'}
truth_style = {'step': 'mid', 'edgecolor': 'green', 'facecolor': (0.75, 0.875, 0.75),
               'lw': 1.25, 'zorder': 0, 'label': '``Truth\"'}
ibu_style = {'ls': '-', 'marker': 'o', 'ms': 2.5, 'color': 'gray', 'zorder': 1}
omnifold_style = {'ls': 'dashed', 'marker': 's', 'ms': 2.5, 'color': 'tab:red', 'zorder': 3}

# calculate quantities to be stored in obs
for obkey,ob in obs.items():
    
    # Add the array with the data to the las dictionary
    # calculate observable for GEN, (REC)SIM, DATA, and TRUE
    ob['genobs'], ob['simobs'] = ob['func'](synthetic, 'gen'), ob['func'](synthetic, 'rec')
    ob['truthobs'], ob['dataobs'] = ob['func'](nature, 'gen'), ob['func'](nature, 'rec')
    
    # setup bins
    # ob['rec/det or gen/mc'] = np.linspace(min val, max val, nbins + 1)
    ob['bins_det'] = np.linspace(ob['xlim'][0], ob['xlim'][1], ob['nbins_det']+1)
    ob['bins_mc'] = np.linspace(ob['xlim'][0], ob['xlim'][1], ob['nbins_mc']+1)
    # = np.linspace(bin-array except the last number, bin-array except the first number) all array vals divided by 2
    ob['midbins_det'] = (ob['bins_det'][:-1] + ob['bins_det'][1:])/2
    ob['midbins_mc'] = (ob['bins_mc'][:-1] + ob['bins_mc'][1:])/2
    # Second val - first val = width
    ob['binwidth_det'] = ob['bins_det'][1] - ob['bins_det'][0]
    ob['binwidth_mc'] = ob['bins_mc'][1] - ob['bins_mc'][0]
    
    # get the histograms of GEN, DATA, and TRUTH level observables
    #  np.histogram(data Array , binning array, density=True(this normalize the histogram))
    # the [0] is to select the first return object of the method,which is the number of event per bin
    # in this case normalized beacause density=True
    ob['genobs_hist'] = np.histogram(ob['genobs'][ob['genobs'] != dummyval], bins=ob['bins_mc'], density=True)[0]
    ob['data_hist'] = np.histogram(ob['dataobs'], bins=ob['bins_det'], density=True)[0]
    # modplot.calc_hist(Data array, binning array,  this just select if the bigger or equal of the binning selection 
    # goes in the right or in the left)
    # [:2] this returns the first 2 object created by the method that  is a histgram and the errors 
    ob['truth_hist'], ob['truth_hist_unc'] = modplot.calc_hist(ob['truthobs'][ob['truthobs'] != dummyval],
                                                               bins=ob['bins_mc'], density=True)[:2]

    
    # I dont check this jet but is think is some kind of one dimensional acceptance correction to 
    # compare with the other
    # compute (and normalize) the response matrix between GEN and SIM
    ob['response'] = np.histogram2d(ob['simobs'], ob['genobs'], bins=(ob['bins_det'], ob['bins_mc']))[0]
    ob['response'] /= (ob['response'].sum(axis=0) + 10**-50)
    
    # perform iterative Bayesian unfolding
    ob['ibu_phis'] = ibu.ibu(ob['data_hist'], ob['response'], ob['genobs_hist'], 
                         ob['binwidth_det'], ob['binwidth_mc'], it=itnum)
    ob['ibu_phi_unc'] = ibu.ibu_unc(ob, it=itnum, nresamples=25)
    
    print('Done with', obkey)


# machine learning model and inputs
model_layer_sizes = [100, 100]
# model_layer_sizes = [100, 100, 100] # use this for the full network size

# set up the array of data/simulation detector-level observables
X_det = np.asarray([np.concatenate((obs[obkey]['dataobs'], obs[obkey]['simobs'])) for obkey in obs_multifold]).T
Y_det = ef.utils.to_categorical(np.concatenate((np.ones(len(obs['Q2']['dataobs'])), 
                                                np.zeros(len(obs['Q2']['simobs'])))))

# set up the array of generation particle-level observables
X_gen = np.asarray([np.concatenate((obs[obkey]['genobs'], obs[obkey]['genobs'])) for obkey in obs_multifold]).T
Y_gen = ef.utils.to_categorical(np.concatenate((np.ones(len(obs['Q2']['genobs'])), 
                                                np.zeros(len(obs['Q2']['genobs'])))))

# standardize the inputs
X_det = (X_det - np.mean(X_det, axis=0))/np.std(X_det, axis=0)
X_gen = (X_gen - np.mean(X_gen, axis=0))/np.std(X_gen, axis=0)

# Specify the training parameters
# model parameters for the Step 1 network
det_args = {'input_dim': len(obs_multifold), 'dense_sizes': model_layer_sizes,
            'patience': 10, 'filepath': 'Step1_{}', 'save_weights_only': False, 
            'modelcheck_opts': {'save_best_only': True, 'verbose': 1}}

# model parameters for the Step 2 network
mc_args = {'input_dim': len(obs_multifold), 'dense_sizes': model_layer_sizes, 
           'patience': 10, 'filepath': 'Step2_{}', 'save_weights_only': False, 
           'modelcheck_opts': {'save_best_only': True, 'verbose': 1}}

# general training parameters
fitargs = {'batch_size': 500, 'epochs': 2, 'verbose': 1}
#fitargs = {'batch_size': 500, 'epochs': 100, 'verbose': 1} # use this for a full training

# reweight the sim and data to have the same total weight to begin with
ndata, nsim = np.count_nonzero(Y_det[:,1]), np.count_nonzero(Y_det[:,0])
wdata = np.ones(ndata)
winit = ndata/nsim*np.ones(nsim)

# apply the OmniFold procedure to get weights for the generation
multifold_ws = omnifold.omnifold(X_gen, Y_gen, X_det, Y_det, wdata, winit,
                                (ef.archs.DNN, det_args), (ef.archs.DNN, mc_args),
                                fitargs, val=0.2, it=itnum, trw_ind=-2, weights_filename='Test')

plot_directory  = "/home/matias/proyecto/Omnifold/Plots/"

for i,(obkey,ob) in enumerate(obs.items()):
    
    # get the styled axes on which to plot
    fig, [ax0, ax1] = modplot.axes(**ob)
    if ob.get('yscale') is not None:
        ax0.set_yscale(ob['yscale'])

        
    # Plot the Different Distributions of the Observable
    # plot the "data" histogram of the observable
    
    ax0.hist(ob['dataobs'][ob['dataobs'] != dummyval], bins=ob['bins_det'], color='black', label='\"Data\"', **hist_style)


    # plot the "sim" histogram of the observable
    ax0.hist(ob['simobs'], bins=ob['bins_det'], color='orange', label='Sim.', **hist_style, linestyle = 'dotted')

    # plot the "gen" histogram of the observable
    ax0.plot(ob['midbins_mc'], ob['genobs_hist'], **gen_style)

    # plot the "truth" histogram of the observable
    ax0.fill_between(ob['midbins_mc'], ob['truth_hist'], **truth_style)

    
    # Plot the Unfolded Distributions of the Observable
    # plot the OmniFold distribution
    of_histgen, of_histgen_unc = modplot.calc_hist(ob['genobs'][ob['genobs'] != dummyval], 
                                                   weights=multifold_ws[2*itnum][ob['genobs'] != dummyval], 
                                                   bins=ob['bins_mc'], density=True)[:2]
    
    ax0.plot(ob['midbins_mc'], of_histgen, **omnifold_style, label='MultiFold')

    # plot the IBU distribution
    #ax0.plot(ob['midbins_mc'], ob['ibu_phis'][itnum], **ibu_style, label='IBU ' + ob['symbol'])

    # Plot the Ratios of the OmniFold and IBU distributions to truth (with statistical uncertainties)
    # ibu_ratio = ob['ibu_phis'][itnum]/(ob['truth_hist'] + 10**-50)
    of_ratio = of_histgen/(ob['truth_hist'] + 10**-50)
    ax1.plot([np.min(ob['midbins_mc']), np.max(ob['midbins_mc'])], [1, 1], '-', color='green', lw=0.75)
    
    # ratio uncertainties
    truth_unc_ratio = ob['truth_hist_unc']/(ob['truth_hist'] + 10**-50)
    #ibu_unc_ratio = ob['ibu_phi_unc']/(ob['truth_hist'] + 10**-50)
    of_unc_ratio = of_histgen_unc/(ob['truth_hist'] + 10**-50)
    
    ax1.fill_between(ob['midbins_mc'], 1 - truth_unc_ratio, 1 + truth_unc_ratio, 
                     facecolor=truth_style['facecolor'], zorder=-2)
    #ax1.errorbar(ob['midbins_mc'], ibu_ratio, xerr=ob['binwidth_mc']/2, yerr=ibu_unc_ratio, 
                                              #color=ibu_style['color'], **modplot.style('errorbar'))
    ax1.errorbar(ob['midbins_mc'], of_ratio, xerr=ob['binwidth_mc']/2, yerr=of_unc_ratio, 
                                              color=omnifold_style['color'], **modplot.style('errorbar'))

    # legend style and ordering
    loc, ncol = ob.get('legend_loc', 'upper right'), ob.get('legend_ncol', 2)
    order = [3, 4, 2, 0, 1] if ncol==2 else [3, 4, 0, 2, 1]
    modplot.legend(ax=ax0, frameon=False, order=order, loc=loc, ncol=ncol)


    # save plot.
    fig.savefig(plot_directory + 'MultiFold_{}.pdf'.format(obkey), bbox_inches='tight')  
print("que wea")
# Add the weights to the dictionary/Ttree and save it in a root    
weight_dic = {'weight': multifold_ws[2*itnum]}
data.update(weight_dic)
with uproot.recreate("/home/matias/proyecto/Omnifold/Data/data_weights_" + target + ".root") as output_file:
    output_file['ntuple_pion'] = data
    output_file['ntuple_pion'].show()
    output_file
