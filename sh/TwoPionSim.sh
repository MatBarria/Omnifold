TARNAME="$@"

# set dirs
MAINDIR=/work/mbarrial/Omnifold/              # dir of the program
JOBDIR=/work/mbarrial/Omnifold/Cpp/bin          # dir to store logs and job scripts
OUTDIR=/work/mbarrial/Omnifold/logs

mkdir -p ${JOBDIR} ${OUTDIR} # just in case

# setting jobname
jobname="TwoPionSim_${TARNAME}"
jobfile="${JOBDIR}/${jobname}.sh"

echo ${jobname}

echo "#!/bin/bash"                                                 > ${jobfile}
echo "#SBATCH -J ${jobname}"                                      >> ${jobfile}
echo "#SBATCH -o ${OUTDIR}/${jobname}.out"                        >> ${jobfile}
echo "#SBATCH -e ${OUTDIR}/${jobname}.err"                        >> ${jobfile}
echo "#SBATCH --time=4:00:00"                                     >> ${jobfile}
echo "#SBATCH --mem=1GB"                                          >> ${jobfile}
echo ""                                                           >> ${jobfile}
echo "source ${HOME}/.bashrc"                                     >> ${jobfile}
echo "cd ${JOBDIR}"                                               >> ${jobfile}
echo "./SimOnePion ${TARNAME}"                             >> ${jobfile}
echo "Submitting job: ${jobfile}"
sbatch ${jobfile} # submit job!
