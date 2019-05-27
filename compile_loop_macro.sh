#!/bin/bash
g++ -std=c++11 -O3 `root-config --cflags` -o newLoopOverFiles.exe newLoopOverFiles.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
srun ./newLoopOverFiles.exe 0 0 10 25 185 1 false  #$@ >/dev/null 2>&1 &

#g++ -std=c++11 -O3 `root-config --cflags` -o peakHeightVsIncidenceAngle_analysis.exe peakHeightVsIncidenceAngle_analysis.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
#srun -p debug ./peakHeightVsIncidenceAngle_analysis.exe 215 220 1 205 225 1  /data/snoplus/home/joesingh/VUV/VUV_Geometry/angle_sweep/ /data/snoplus/home/joesingh/VUV/VUV_Geometry/peak_analysis_mirror_aperture_back_nominal.root #$@ >/dev/null 2>&1 &
