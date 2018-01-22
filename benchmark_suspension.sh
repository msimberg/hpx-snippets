#!/bin/bash

printf "threads, "
printf "first start [s], first stop [s], first main [s], first finalize [s], "
printf "start min [s], start max [s], start avg [s], "
printf "stop min [s], stop max [s], stop avg [s], "
printf "main min [s], main max [s], main avg [s], "
printf "finalize min [s], finalize max [s], finalize avg [s]"
printf "\n";

for i in `seq ${1} ${2}`; do
    printf $i, 
    ./build/suspension_benchmark --hpx:threads=$i
done

