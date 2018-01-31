#!/bin/bash

printf "threads, "
#printf "start min [s], start max [s], start avg [s], "
#printf "async min [s], async max [s], async avg [s], "
#printf "main min [s], main max [s], main avg [s], "
#printf "stop min [s], stop max [s], stop avg [s]"
printf "start [s], async [s], main [s], stop [s]"
printf "\n";

for i in `seq ${1} ${2}`; do
#for i in `yes ${1} | head -n${2}`; do
    #printf "$i, "
    ./build/suspension_benchmark --hpx:threads=$i --repetitions=1000
done

