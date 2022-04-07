#!/bin/bash

# 1 node
#SBATCH --nodes=1
#SBATCH --job-name="hello-openmp"
#SBATCH -o hello-openmp.%J.out
#SBATCH -e hello-openmp.%J.err
#SBATCH -t 00:01:00
#SBATCH -p test.q

source /etc/profile.d/modules.sh

module load intel/xe_2018.2
module load gcc/9.3.0

export OMP_NUM_THREADS=2
./hello-openmp
