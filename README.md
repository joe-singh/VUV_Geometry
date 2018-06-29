# VUV_Geometry
VUV Geometry 

## Use:

At the command line:
1. python Main.py 
2. rat photon.mac -o output.root
3. python photonTracking.py output.root

Main.py controls all the geometry and creates a file called VUV.geo, which the photon.mac macro needs to 
simulate. This macro simulates (100,000) photons and stores their track information and writes it to output.root. 
photonTracking.py reads this output file and does a simple 3d scatterplot of the start and eventual endpoints of photons. 

geoFactory.py is the class made by Chris that contains all the shape information about Tubes and Boxes, as well as
borders between shapes. OPTICS.ratdb is the file containing all the material information. 
