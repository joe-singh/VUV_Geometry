import sys

angle = sys.argv[2]
sample_angle = sys.argv[1]
fname = './macros/photon_'+str(int(float(sample_angle)))+'.'+str(int(float(angle)))+'.mac'
f = open(fname, 'w')


f.write('/glg4debug/glg4param omit_muon_processes  1.0\n')
f.write('/glg4debug/glg4param omit_hadronic_processes  1.0\n')

f.write('/rat/db/set DETECTOR geo_file "./VUV_'+sample_angle+'_'+angle+'_.geo"\n')

f.write('/run/initialize\n')

f.write('/rat/proc count\n')
f.write('/rat/procset update 1000\n')

f.write('/rat/proclast outroot\n')

f.write('/tracking/storeTrajectory 1\n')

#/generator/add combo gun2:regexfill
#/generator/pos/set -0.0 -0.0 0.0 
f.write('/generator/add combo gun2:point\n')
# For 20 degrees
f.write('/generator/vtx/set opticalphoton 0.0 1.0 0.0 0.0 0.0000071 0.0000071\n') # Wavelength 175 nm, Energy in MeV 
# For debug 
#/generator/pos/set 0.0 0.0 0.0
f.write('/generator/pos/set -101.6 -300.0 0.0\n') 
#/generator/pos/set slitGenPlane #-101.6 -310.0 0.0 

#/generator/pos/set -101.6 -310.0 0.0 

f.write('/run/beamOn 1000000\n')
