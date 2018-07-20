#!/bin/bash

s="10" 

while [ $s -lt 90 ]
do
  n="0"
  while [ $n -lt 90 ]
  do
  python geoGenMain.py $s $n
  rat photon.mac -o ./no_tpb_files/no_cylinder_angle_$s.$n.root 
  n=$[n+5]
  done
  s=$[s+5]
done
