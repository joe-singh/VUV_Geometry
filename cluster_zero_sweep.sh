#!/bin/bash

s="0"
while [ $s -lt 1 ]
do
    if [ ! -d "./angle_sweep/angle_$s" ]; then
        mkdir ./angle_sweep/angle_$s
    fi
    n="-50" 
    while [ $n -lt 51 ]
    do
        srun -p debug python geoGenMain.py $s $n
        srun -p debug python makePhotonMacro.py $s $n
    echo ">>> Done $s $n"
    srun -p debug --comment="$@VUVSim_$s_$n" rat -o ./angle_sweep/angle_$s/no_cylinder_angle_$s.$n.root ./macros/photon_$s.$n.mac $@ >/dev/null 2>&1 &
    n=$[n+1]
    done 
s=$[s+1]
done
