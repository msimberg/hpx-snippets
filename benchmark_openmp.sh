#!/bin/bash

printf "threads, "
printf "min [s], max [s], avg [s], result"
printf "\n";

for i in `seq ${1} ${2}`; do
    printf "$i, "
    OMP_NUM_THREADS=$i ./build/openmp_benchmark
done
