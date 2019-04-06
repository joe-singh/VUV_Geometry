#!/bin/bash

if [ ! -d "./angle_sweep" ]; then 
    mkdir ./angle_sweep
fi

s="220"
while [ $s -lt 241 ]
do
    if [ ! -d "./angle_sweep/angle_$s" ]; then
        mkdir ./angle_sweep/angle_$s
    fi
    n=$[s-10]
    u=$[s+10]
    #n="90"
    while [ $n -lt $u ]
    do
        srun -p debug python geoGenMain.py $s $n 1.0
        srun -p debug python makePhotonMacro.py $s $n 400.0 10000 # wavelength (nm), n_photons
    echo ">>> Done $s $n"
    srun -p debug --comment="$@VUVSim_$s_$n" rat -o ./angle_sweep/angle_$s/no_cylinder_angle_$s.$n.root ./macros/photon_$s.$n.mac $@ >/dev/null 2>&1 &
    n=$[n+1]
    done 
s=$[s+1]
done
