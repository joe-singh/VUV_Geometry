#!/bin/bash

front_nominal=(20.0 30.0 45.0 47.0 50.0 52.0 55.0 57.0 60.0 62.0 65.0 67.0 70.0 72.0 75.0)
front_data=(17.3 30.8 46.4 48.8 51.0 52.9 56.5 58.6 60.8 63.3 67.8 69.3 72.4 73.7 76.7)
back_nominal=(200.0 210.0 225.0 227.0 230.0 232.0 235.0 237.0 240.0 242.0 245.0 247.0 250.0 252.0 255.0)
back_data=(196.5 205.7 220.0 226.1 224.3 225.5 229.0 229.7 233.1 233.7 236.7 233.9 237.2 239.0 242.9)
tpb_systematic=(0.0) 
### Make sure output directory exists
if [ ! -d "MC_tpb/sqe_minus" ]; then 
    mkdir -p MC_tpb/sqe_minus
fi
#rm -r angle_sweep/* # Clean it

### Make sure directory with macros exists
if [ ! -d "macros/check/" ]; then 
    mkdir -p macros/check/
fi
#rm -r macros/check/* # Clean it

#s_min=$1
#s_max=$1
#s=$s_min
#while [ $s -le $s_max ]; do
#while (( $(echo "$s < $s_max+1" |bc -l) )); do
for s in "${tpb_systematic[@]}"; do 
    echo "$s"
    if [ ! -d "MC_tpb/sqe_minus/angle_$s" ]; then
        mkdir MC_tpb/sqe_minus/angle_$s
    fi

	n_min=$(echo "25.0"|bc -l)  #n_min=$(echo "$s - 10" |bc -l)     
	n_max=$(echo "30.0"|bc -l) #n_max=$(echo "$s + 10" |bc -l) 

    n=$n_min
    #while [ $n -le $n_max ]
    while (( $(echo "$n < $n_max+1" |bc -l) )); 
    do
        python geoGenMain.py $s $n 1.0 'tpb' 0 0 0 0 0 0 0 check 
        python makePhotonMacro.py $s $n 128.0 1000000 check # wavelength (nm), n_photons
    echo ">>> Done $s $n"
        sbatch -p debug --wait --comment="$@VUVSim_$s_$n" ratbatch.sh -o ./MC_tpb/sqe_minus/angle_$s/no_cylinder_angle_$n.root ./macros/check/photon_$s.$n.mac $@ >/dev/null 2>&1 &
    n=$(echo "$n+1"|bc -l)  #n=$[n+1]
    done 
#s=$[s+10]
s=$(echo "$s+1"|bc -l)
done
wait

#g++ -std=c++11 -O3 `root-config --cflags` -o newLoopOverFiles.exe newLoopOverFiles.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
#srun -w westley ./newLoopOverFiles.exe $1 $1 10 10 45 1  #$@ >/dev/null 2>&1 &


