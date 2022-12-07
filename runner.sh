#!/bin/bash

#SBATCH --account=m22oc-SECRET
#SBATCH --job-name=automaton
#SBATCH --time=00:10:00
#SBATCH --qos=standard
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --nodes=1

#SBATCH --output=%x-%j.out
#SBATCH --error=%x-%j.err
#SBATCH --partition=standard

# Very basic slurm batch script. For larger jobs, change --nodes and add --exclusive flag.
# Can be modified on-the-fly using sed.


module load intel-compilers-19
module load mpt

shopt -s -o nounset

declare ntasks=1
declare length=256

declare N=10

for ((i=0;i<N;i++))
do
        mpirun -n $ntasks ./automaton -l $length -e 0 -w 0 1
done

