#!/bin/bash
  
n="-90"
while [ $n -lt 90 ]
do
python geoGenMain.py 0.0 $n
rat photon.mac -o ./zero_angle_sweep/no_cylinder_angle_0.$n.root
n=$[n+5]
done
