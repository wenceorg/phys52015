#!/bin/bash

# 1 core
#SBATCH -n 1
#SBATCH --job-name="hello-serial"
#SBATCH -o hello-serial.%J.out
#SBATCH -e hello-serial.%J.err
#SBATCH -t 00:01:00
#SBATCH -p test.q

source /etc/profile.d/modules.sh

module load intel/xe_2018.2
module load gcc/9.3.0

./hello-serial
