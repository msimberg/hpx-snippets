#!/bin/bash

printf "threads, start [s], async [s], main [s], stop [s]"
printf "\n";

for i in `seq ${1} ${2}`; do
    printf "$i, "
    ./build/start_stop_benchmark --hpx:threads=$i --repetitions=1000
done

