#!/bin/bash
srun -p debug --comment="$@" root -q newLoopOverFiles.cc(0, 5, 5, -95, 95, 5) #$@ >/dev/null 2>&1 &
