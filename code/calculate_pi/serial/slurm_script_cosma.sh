#!/bin/bash -l
#
# Batch script for bash users
#
#SBATCH -n 2 #the number of cores we are using
#SBATCH -J cal_pi  #a name for our job
#SBATCH -o cal_pi.%J.dump #the logfile
#SBATCH -e cal_pi.%J.err  #the error file
#SBATCH -p cosma #the queue we want to use
#SBATCH -A durham
#SBATCH -t 00:01:00 ## the maximum run time
# Need to ensure the right MPI module is loaded -
# i.e. the same module which the program was compiled with.

# specify the modules you compiled the code with below
module purge #unload all modules


module list #write a list of used modules to the outputfile

# Run the program
#add the correct command to run the program below


#print some info at the end
echo "Job done, info follows..."
sacct -j $SLURM_JOBID --format=JobID,JobName,Partition,MaxRSS,Elapsed,ExitCode
exit
