# VUV_Geometry
VUV Geometry Simulation 

## Summary
This repo is used to run simulations of the VUV apparatus for both the 400 nm mirror calibration and the 128 nm TPB
measurements. Just like the actual setup, the simulation geometry has a sample and PMT that rotates. Shell scripts are 
also available to automate the process. An analysis script creates a plot of the photon distribution as a function of 
PMT angle. There is also a script to overlay data and MC plots, which has only currently been used for the mirror but can be 
extended to the TPB later.

## To Do
1. Make sure file paths in geoGenMain.py, makePhotonMacro.py, cluster_zero_sweep.sh, and newLoopOverFiles.cc are changed.
2. Remove hardcoded variables. 

## Files
1. geoGenMain.py - Contains details of the geometry. Python script that writes to an output .geo file, which is then used by
RAT to make the geometry. Run as follows:

$ geoGenMain.py sample_angle pmt_angle tpb_thickness config aperture distance reflectivity laser_axis rotation_axis pmt_parallel pmt_transverse 

sample_angle - numerical: the angle at which the sample is turned (dictates angle of incidence)
pmt_angle - numerical: the angle of the pmt
tpb_thickness - numerical: thickness of tpb layer, no effect if config is mirror
config - string: "tpb" if tpb simulation, "mirror" if mirror calibration simulation
aperture - numerical: flag to indicate aperture systematic, ±1 for ±1stdev and 0 for no effect
distance - numerical: flag to indicate pmt distance systematic, ±1 for ±1stdev and 0 for no effect
reflectivity - numerical: flag to indicate reflectivity systematic, 1 for on and 0 for no effect
laser_axis - numerical: flag to indicate laser misalignment systematic, 1 for on and 0 for no effect
rotation_axis - numerical: flag to indicate sample holder rotation misalignment, 1 for on and 0 for no effect
pmt_parallel - numerical: flag to indicate misalignment of pmt rotation parallel to beam, 1 for on and 0 for no effect
pmt_transverse - numerical: flag to indicate misalignment of pmt rotation transverse to beam, 1 for on and 0 for no effect

output: .geo file used by RAT.

2. makePhotonMacro.py - makes the RAT macro used to run the simulation. Run as follows:

$ makePhotonMacro.py sample_angle pmt_angle wavelength n_photon filepath

sample_angle and pmt_angle - same as for geoGenMain.py
wavelength - wavelength of photons in nm 
n_photon - number of photons in simulation, recommended 10000 for mirror and 1000000 for TPB
filepath - destination of output .mac files, stored in ./macros/filepath

output: .mac file to run RAT simulation. needs .geo file corresponding to sample_angle, pmt_angle.

3. cluster_zero_sweep.sh

Produces .geo/.mac files for a given range of angles and runs simulations on the cluster. Run as follows:

./cluster_zero_sweep.sh

NOTE: This file has the range of sample angles and pmt angles hardcoded into it currently. Line 25 loops over sample angles
(using an array) and lines 31/32 determine the min/max for the PMT angle sweep. The shell script can support angles with 1dp
but the increment in PMT angle is always +1 (there is no increment on the sample angle since it loops over an array which has
all the angles in it already). 

output: .root MC files in the directory specified on line 10. NOTE this directory is hardcoded into line 41 which actually 
determines where the output files go. The actual structure is directory/angle_x/\*.root where x is the sample angle. 

4. compile_loop_macro.sh

Shell script to run analysis script on MC .root files and produce photon distribution plots. Run as follows:

./compile_loop_macro.sh

NOTE: This file also has hardcoded parameters. Line 3 is of the form:

srun ./newLoopOverFiles.exe sample_start sample_finish sample_step pmt_start pmt_finish pmt_step false  #$@ >/dev/null 2>&1 &

sample_start/sample_finish indicate the first and last sample angles for which MC files exist. sample_step is the increment 
size with which the analysis should loop over the MC. 

pmt_start/pmt_finish indicate the first and last pmt angles for which MC files exist for the given range of sample angles.
pmt_step is the increment size with which the analysis should loop over these MC files. 

NOTE: The location where to look for the MC files is hardcoded into the script at line 221. 

output: a root file (name hardcoded on line 174) which contains the angular distributions. 



geoShapes.py is the class made by Chris that contains all the shape information about Tubes and Boxes, as well as
borders between shapes. OPTICS.ratdb is the file containing all the material information. 

raytrace.mac is the file used to visualise (rat raytrace.mac)
