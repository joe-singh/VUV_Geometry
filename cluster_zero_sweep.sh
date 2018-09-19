#!/bin/bash

n="-90"
while [ $n -lt 95 ]
do
    srun -p debug python geoGenMain.py 0.0 $n
    echo ">>> Done $n"
    if [ ! -d "./zero_angle_sweep_highstats/angle_$n" ]; then 
        mkdir ./zero_angle_sweep_highstats/angle_$n
    fi 
    srun -p debug --comment="$@VUVSim_0.0_$n" rat -o ./zero_angle_sweep_highstats/angle_$n/no_cylinder_angle_0_$n.root photon.mac $@ >/dev/null 2>&1 
n=$[n+5]
done
