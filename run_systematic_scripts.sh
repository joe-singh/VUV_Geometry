#!/bin/bash
if [ -d "angle_sweep" ]; then
    echo "Cleaning angle_sweep"
    rm -rf angle_sweep
fi

if [ -d "macros" ]; then
    echo "Cleaning macros"
    rm -rf macros
fi

echo "Compiling analysis macro"
g++ -std=c++11 -O3 `root-config --cflags` -o peakHeightVsIncidenceAngle_analysis.exe peakHeightVsIncidenceAngle_analysis.cc `root-config --glibs` -I $RATROOT/include -L $RATROOT/lib -l RATEvent -l rat
echo "Compiled"
python make_systematic_scripts.py pmt_y
echo "Made shell scripts"
echo "Running loop"

#JOBID=""
for f in shellscripts/*.sh; do
    chmod +x "$f"
    $f
    #   JOBID=${JOBID}:`sbatch "$f" | awk '{print $4'}`
done  

make clean

srun ./peakHeightVsIncidenceAngle_analysis.exe 200 250 1 190 260 1  /data/snoplus/home/joesingh/VUV/VUV_Geometry/angle_sweep/mirror/pmt_y/back/nominal/ /data/snoplus/home/joesingh/VUV/VUV_Geometry/peak_analysis_mirror_pmt_y_back_nominal.root #:$@ >/dev/null 2>&1 &
srun ./peakHeightVsIncidenceAngle_analysis.exe 200 250 1 190 260 1  /data/snoplus/home/joesingh/VUV/VUV_Geometry/angle_sweep/mirror/pmt_y/back/plus/ /data/snoplus/home/joesingh/VUV/VUV_Geometry/peak_analysis_mirror_pmt_y_back_plus.root #$@ >/dev/null 2>&1 &
srun ./peakHeightVsIncidenceAngle_analysis.exe 200 250 1 190 260 1  /data/snoplus/home/joesingh/VUV/VUV_Geometry/angle_sweep/mirror/pmt_y/back/minus/ /data/snoplus/home/joesingh/VUV/VUV_Geometry/peak_analysis_mirror_pmt_y_back_minus.root #$@ >/dev/null 2>&1 &



