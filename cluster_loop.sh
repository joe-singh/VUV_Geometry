#!/bin/bash
srun -p debug --comment="$@" root -q loopOverFiles.cc #$@ >/dev/null 2>&1 &
