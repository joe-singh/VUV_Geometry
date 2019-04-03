#!/bin/bash

### Make sure output directory exists
if [ ! -d "angle_sweep" ]; then 
    mkdir angle_sweep
fi
rm -r angle_sweep/* # Clean it

### Make sure directory with macros exists
if [ ! -d "macros" ]; then 
    mkdir macros
fi
rm -r macros/* # Clean it

s_min=215
s_max=240
s=$s_min
while [ $s -le $s_max ]; do
    if [ ! -d "angle_sweep/angle_$s" ]; then
        mkdir angle_sweep/angle_$s
    fi

	n_min=$((s - 5))
	n_max=$((s + 5))
    n=$n_min
    while [ $n -le $n_max ]
    do
		#if [ $n -ne 85 ]; then
		#	n=$[n+5]
		#	continue
		#fi
        srun -p debug python geoGenMain.py $s $n 1.0
        srun -p debug python makePhotonMacro.py $s $n 400.0
		echo ">>> Done $s $n"
    	srun -p debug --comment="$@VUVSim_$s_$n" rat -o ./angle_sweep/angle_$s/no_cylinder_angle_$s.$n.root ./macros/photon_$s.$n.mac $@ >/dev/null 2>&1 &
		n=$((n + 1))
    done 
	s=$((s + 1))
done
