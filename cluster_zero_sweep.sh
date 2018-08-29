#!/bin/bash

n="-95"
while [ $n -lt 100 ]
do
    srun -p debug python geoGenMain.py 0.0 $n
    echo ">>> Done $n"
    srun -p debug --comment="$@VUVSim_0.0_$n" rat -o ./zero_angle_sweep_highstats/no_cylinder_angle_0_$n.root photon.mac $@ >/dev/null 2>&1 &
    sleep 0.5
    #if [ -e ./zero_angle_sweep_highstats/no_cylinder_angle_0_$n.root ] 
    #then     
    #    echo ">>> File Created"
    #else  
    #    echo ">>> File Not Created at Angle $n"
    #    exit 1
    #fi
n=$[n+5]
done
