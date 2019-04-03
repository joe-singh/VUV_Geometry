#!/bin/bash
g++ -std=c++11 -O3 `root-config --cflags` -o newLoopOverFiles.exe newLoopOverFiles.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
srun -p debug ./newLoopOverFiles.exe 85 85 10 0 90 5  #$@ >/dev/null 2>&1 &

#g++ -std=c++11 -O3 `root-config --cflags` -o peakHeightVsIncidenceAngle_analysis.exe peakHeightVsIncidenceAngle_analysis.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
#srun -p debug ./peakHeightVsIncidenceAngle_analysis.exe 45 85 1 40 90 1  #$@ >/dev/null 2>&1 &
