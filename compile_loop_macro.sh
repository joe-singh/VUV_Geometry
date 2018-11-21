#!/bin/bash
g++ -std=c++11 -O3 `root-config --cflags` -o newLoopOverFiles.exe newLoopOverFiles.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
srun -p debug ./newLoopOverFiles.exe 0 0 1 -50 50 1 $@ >/dev/null 2>&1 &
