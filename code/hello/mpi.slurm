#!/bin/bash

# 1 node
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=24
#SBATCH --job-name="hello-mpi"
#SBATCH -o hello-mpi.%J.out
#SBATCH -e hello-mpi.%J.err
#SBATCH -t 00:01:00
#SBATCH -p par7.q

source /etc/profile.d/modules.sh

module load intel/xe_2018.2
module load gcc/9.3.0
module load intelmpi/intel/2018.2

mpirun ./hello-mpi
