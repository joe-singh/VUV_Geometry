import sys
import math

def wavelengthToEnergy(wavelength):
  l = wavelength * (10**-9) # wavelength in m
  h = 4.135667662 * (10 ** -15) # h in evS
  c = 299792458 # c in ms-1
  E = str(h*c*1e-6 / l) # E in MeV
  return "{:.10f}".format(float(E))

energy = wavelengthToEnergy(float(sys.argv[3]))

angle = sys.argv[2]
sample_angle = sys.argv[1]
fname = './macros/photon_'+str(int(float(sample_angle)))+'.'+str(int(float(angle)))+'.mac'
f = open(fname, 'w')

# [10.0d1, 10.0d1, 2.75d-3, 2.75d-3] = WLSABSLENGTH
f.write('/glg4debug/glg4param omit_muon_processes  1.0\n')
f.write('/glg4debug/glg4param omit_hadronic_processes  1.0\n')
f.write('/rat/db/set OPTICS[tpb] WLSMEANNUMBERPHOTONS 0.9\n')
f.write('/rat/db/set DETECTOR geo_file "./VUV_'+sample_angle+'_'+angle+'_.geo"\n')
f.write('/run/initialize\n')

### Visualization commands
#f.write('/vis/open HepRepFile\n') # command to use HepRep visualization
##f.write('/vis/open OGL\n') # option to use OGL visualization exists too if you want
#f.write('/vis/drawVolume\n')
#f.write('/vis/viewer/flush\n')
#f.write('/vis/scene/add/trajectories\n')
#f.write('/vis/scene/add/hits\n')

f.write('/rat/proc count\n')
f.write('/rat/procset update 1000\n')

f.write('/rat/proclast outroot\n')

f.write('/tracking/storeTrajectory 1\n')
#f.write('/tracking/verbose 1\n')
f.write('/generator/add combo gun2:regexfill\n')
#/generator/pos/set -0.0 -0.0 0.0 
#f.write('/generator/add combo gun2:point\n')
# For 20 degrees
f.write('/generator/vtx/set opticalphoton 0.0 1.0 0.0 0.0 %s %s\n' %(energy, energy)) # Wavelength 128 nm, Energy in MeV 
# For debug 
#/generator/pos/set 0.0 0.0 0.0
#f.write('/generator/pos/set -101.6 -300.0 0.0\n') 
#f.write('/generator/pos/set slitGenPlane\n') #-101.6 -310.0 0.0 

### Fill slitGenPlane volume with specified distribution
fwhm_beam = 0.1535 # inch
sigma_beam = fwhm_beam/(2*math.sqrt(2*math.log(2))) # inch
sigma_beam_mm = 25.4*sigma_beam # mm
### The following option "slitGenPlane" with no additional following options
### will fill uniformly. The first option following "slitGenPlane" can be
### either empty or "uniform", "normal", "normal2d". In the case of "normal" or
### "normal2d", you need to specify the axis ("x", "y", or "z") in which the
### distribution is in. In the case of "normal2d" you need two axes. The first
### number following the axis label is the mean, the second is the sigma.
f.write('/generator/pos/set slitGenPlane normal2d x 0 ' + str(sigma_beam_mm) + ' y 0 ' + str(sigma_beam_mm) + '\n') # Fill slitGenPlane volume using 2-D Gaussian with sigma_beam_mm sigma along both axes x, y

#/generator/pos/set -101.6 -310.0 0.0 

### Settings for visualization
f.write('/tracking/FillPointCont true\n')
f.write('/tracking/storeTrajectory 3\n')
f.write('/vis/scene/endOfEventAction accumulate\n') # accumulate photon tracks

### Add aesthetic x-y-z axis coordinates in visualization
f.write('/vis/scene/add/axes 0 0 0 10 cm\n')

### Run simulation
#f.write('/run/beamOn 1000000\n')
