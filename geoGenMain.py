import geoShapes as GS
import numpy as np 
import sys

masterString = ''
file_name = './VUV_'+str(sys.argv[2])+'_.geo'

# NOTE 12inches in 1 foot

# True if including all cylinders. False if only quartz.
CYLINDER_FLAG = True 
TPB_THICKNESS = 2.2  # NOTE Micrometers!!
SAMPLE_CYLINDER_RADIUS = 1.5
SAMPLE_CYLINDER_HEIGHT = 3.0
BASE_PLATE_HEIGHT = 0.75
LIGHT_HOLE_HEIGHT_FROM_BOTTOM = 8.0
LIGHT_HOLE_DIAMETER = 1.5
PHOTODIODE_BOTTOM_ARM_THICKNESS = 0.5
PHOTODIODE_DISTANCE_FROM_SAMPLE = 8.5

slit_width = 0.07
slit_height = 0.192

# NOTE For photodiode, theta = 0.0 puts it on the right side (y, z > 0). 
# For samples, theta = 0.0 means TPB is facing the left side where the light comes 
# in. Both rotations occur in the same direction though, there is just a 180 degree
# offset
SAMPLE_HOUSING_ANGLE = float(sys.argv[1])
#PHOTODIODE_ANGLE_NORMAL = float(sys.argv[2]) 
 
SAMPLE_NORMAL = 180.0 + SAMPLE_HOUSING_ANGLE
# Measure angles relative to normal of tpb sample
PHOTODIODE_THETA = float(sys.argv[2]) 
 
PHOTODIODE_HEIGHT = 1.25
NUM_SAMPLE_HOLES = 4
HOLE_IN_FRONT_OF_LIGHT = 2
assert 0 < HOLE_IN_FRONT_OF_LIGHT <= NUM_SAMPLE_HOLES

def deg_to_rad(deg):
    return np.pi * (deg / 180.0)

def trig_distances(r, theta):
    y = r * np.cos(deg_to_rad(theta))
    z = r * np.sin(deg_to_rad(theta))
    return y, z

borders = [] 

# Create World
world = GS.BoxVolume('world',400, 400, 400)
world.material = 'acrylic_black'
world.mother = ''
world.invisible = 1
masterString = world.writeToString(masterString)

# Create outside cube
cube = GS.BoxVolume('cube', 24, 24, 24)
cube.material = 'pmt_vacuum'
cube.mother = 'world'
cube.colorVect[3] = 0.1

cube_negative_x = -cube.height/2.0
cube_positive_x = cube.height/2.0

cube_negative_y = -cube.width/2.0
cube_positive_y = cube.width/2.0

cube_negative_z = -cube.depth/2.0
cube_positive_z = cube.depth/2.0

masterString = cube.writeToString(masterString)

# Create base plate
plate = GS.BoxVolume('baseplate', BASE_PLATE_HEIGHT, 18, 12)
plate.material = 'aluminum'
plate.mother = 'cube'
plate.colorVect[3] = 0.5
plate.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT/2.0, 'y': cube_negative_y + 10, 'z': 0.0}
masterString = plate.writeToString(masterString)

# Plate-cube border
#border_plate_cube = GS.border('borderplatecube', plate.name, cube.name)
#borders.append(border_plate_cube) 

BOTTOM_HEIGHT = LIGHT_HOLE_HEIGHT_FROM_BOTTOM - plate.height - PHOTODIODE_BOTTOM_ARM_THICKNESS - SAMPLE_CYLINDER_HEIGHT/2.0

# Create cylinder which will house the samples
sample_housing = GS.TubeVolume('samplehousing', SAMPLE_CYLINDER_RADIUS, SAMPLE_CYLINDER_HEIGHT, SAMPLE_CYLINDER_RADIUS - 0.11811)
sample_housing.material = 'quartz_suprasil'
sample_housing.mother = 'cube'
sample_housing.colorVect[3] = 0.5
x_sample_housing = cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS +  SAMPLE_CYLINDER_HEIGHT/2.0
sample_housing.center = {'x': x_sample_housing, 'y': cube_negative_y + 10, 'z': 0}
sample_housing.rotation[1] = 90.0

sample_center_y, sample_center_z = sample_housing.center['y'], sample_housing.center['z']
LIGHT_HOLE_CENTRE_HEIGHT = cube_negative_x + LIGHT_HOLE_HEIGHT_FROM_BOTTOM

# Create Shape to Simulate Liquid ArgonLAR = GS.TubeVolume('LAr', 6.24- 1e-6, cube.height, 0.0) 
""" 
LAR = GS.TubeVolume('LAr', sample_housing.rMin - 1e-6, cube.height - BASE_PLATE_HEIGHT - PHOTODIODE_BOTTOM_ARM_THICKNESS - 1e-5, 0.0) 
LAR.mother = 'cube'
LAR.material = 'pmt_vacuum'
LAR.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS + LAR.height/2.0, 'y': sample_housing.center['y'], 'z': 0.0} 
assert cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS == LAR.center['x'] - LAR.height/2.0
LAR.colorVect[3] = 0.2
LAR.rotation[1] = -90.0 
"""
LAR = GS.TubeVolume('LAr', sample_housing.rMin - 1e-6, cube.height - BASE_PLATE_HEIGHT- PHOTODIODE_BOTTOM_ARM_THICKNESS -1e-5, 0.0) 
LAR.mother = 'cube' 
LAR.material = 'liquid_Ar'
LAR.rotation[1] = -90.0
LAR.rotation[0] = 90.0 - SAMPLE_HOUSING_ANGLE
LAR.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS + LAR.height/2.0, 'y': sample_housing.center['y'], 'z': 0.0} 
LAR.colorVect[3] = 0.4

"""
LAR = GS.BoxVolume('LAr', cube.height - BASE_PLATE_HEIGHT - PHOTODIODE_BOTTOM_ARM_THICKNESS - 1e-5, 2.0, 2.0) 
LAR.mother = 'cube' 
LAR.material = 'pmt_vacuum' 
LAR.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS + LAR.height/2.0, 'y': sample_housing.center['y'], 'z': 0.0} 
LAR.colorVect[3] = 0.2 
masterString = LAR.writeToString(masterString) 
"""
# Top and bottom cylinders
if CYLINDER_FLAG or not CYLINDER_FLAG:
    BOTTOM_HEIGHT = LIGHT_HOLE_HEIGHT_FROM_BOTTOM - plate.height - PHOTODIODE_BOTTOM_ARM_THICKNESS - SAMPLE_CYLINDER_HEIGHT/2.0 
    bottom_cylinder = GS.TubeVolume('bottomcylinder', SAMPLE_CYLINDER_RADIUS, BOTTOM_HEIGHT, SAMPLE_CYLINDER_RADIUS - 0.11811)
    bottom_cylinder.material = 'aluminum'
    bottom_cylinder.mother = 'cube'
    bottom_cylinder.colorVect[3] = 0.5
    bottom_cylinder.center = {'x': cube_negative_x + plate.height + PHOTODIODE_BOTTOM_ARM_THICKNESS + BOTTOM_HEIGHT/2.0, 'y': sample_center_y, 'z': sample_center_z}
    bottom_cylinder.rotation[1] = 90.0
    sample_housing.center['x'] += BOTTOM_HEIGHT
    masterString = bottom_cylinder.writeToString(masterString)
   
    top_height = cube.height - bottom_cylinder.height - sample_housing.height - plate.height - PHOTODIODE_BOTTOM_ARM_THICKNESS
    top_cylinder = GS.TubeVolume('topcylinder', SAMPLE_CYLINDER_RADIUS, top_height, SAMPLE_CYLINDER_RADIUS - 0.11811)
    top_cylinder.mother = 'cube'
    top_cylinder.material = 'aluminum'
    top_cylinder.colorVect[3] = 0.5
    top_cylinder.center = {'x': sample_housing.center['x'] + sample_housing.height/2.0 + top_cylinder.height/2.0, 'y': sample_center_y, 'z': sample_center_z}
    top_cylinder.rotation[1] = 90.0
    masterString = top_cylinder.writeToString(masterString)
    assert sample_housing.center['x'] == LIGHT_HOLE_CENTRE_HEIGHT
    assert bottom_cylinder.center['x'] + bottom_cylinder.height/2.0 == sample_housing.center['x'] - sample_housing.height/2.0 and \
           sample_housing.center['x'] + sample_housing.height/2.0 == top_cylinder.center['x'] - top_cylinder.height/2.0 and \
           top_cylinder.center['x'] + top_cylinder.height/2.0 == cube_positive_x

    masterString = sample_housing.writeToString(masterString)

if CYLINDER_FLAG:    
    masterString = LAR.writeToString(masterString) 

# Add hole for light to come in from
light_hole = GS.TubeVolume('light_hole', LIGHT_HOLE_DIAMETER/2.0, 10, 0)
light_hole.material = 'pmt_vacuum'
light_hole.mother = 'world'
#light_hole.colorVect[3] = 0.2
light_hole.invisible = 0
light_hole.colorVect[3] = 0.1
light_hole.rotation[0] = 90.0
light_hole.center = {'x': LIGHT_HOLE_CENTRE_HEIGHT,'y': cube_negative_y - light_hole.height/2.0, 'z': 0}
masterString = light_hole.writeToString(masterString)
#light_cube_border = GS.border('lightcubeborder', light_hole.name, cube.name) 
#borders.append(light_cube_border)

# Add test volume to see if photons going through this as needed
testDisk = GS.TubeVolume('testdisk', light_hole.rMax, 0.05, 0) 
testDisk.material = 'pmt_vacuum'
testDisk.mother = 'cube' 
testDisk.colorVect[3] = 1.0
testDisk.rotation[0] = 90.0 
old_y_center = cube_negative_y + testDisk.height/2.0 + 5.0 
testDisk.center = {'x': light_hole.center['x'], 'y': old_y_center, 'z': light_hole.center['z']}
masterString = testDisk.writeToString(masterString) 

testDisk_border = GS.border('testdiskborder', testDisk.name, cube.name)

# Add lever joining sample housing to photodiode 
# Assert statement ensures that rod doesn't break cube boundaries
assert PHOTODIODE_DISTANCE_FROM_SAMPLE < min(sample_housing.center['y'] - cube_negative_y, sample_housing.center['z'] - cube_negative_z)
bottom_arm = GS.BoxVolume('bottomarm', 0.5, PHOTODIODE_DISTANCE_FROM_SAMPLE, 2) 
bottom_arm.material = 'aluminum'
bottom_arm.mother = 'cube'
bottom_arm.colorVect[3] = 0.5
THETA_RAD = deg_to_rad(PHOTODIODE_THETA)
y_arm_offset, z_arm_offset = trig_distances(bottom_arm.width/2, PHOTODIODE_THETA)
bottom_arm.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + bottom_arm.height/2.0, 'y': sample_housing.center['y'] + y_arm_offset, 'z': sample_housing.center['z'] + z_arm_offset}
bottom_arm.rotation[0] = -PHOTODIODE_THETA
masterString = bottom_arm.writeToString(masterString) 

#sample_cube_border = GS.border('samplehousingcubeborder', sample_housing.name, cube.name)
#borders.append(sample_cube_border)
arm_cube_border = GS.border('armcubeborder', bottom_arm.name, cube.name)

if CYLINDER_FLAG:
    sample_base_border = GS.border('samplebaseborder', sample_housing.name, bottom_arm.name)
    borders.append(sample_base_border)
    sample_base_border = GS.border('samplebaseborder', bottom_cylinder.name, bottom_arm.name)
    borders.append(sample_base_border) 
    sample_bottom_border = GS.border('samplebottomborder', sample_housing.name, bottom_cylinder.name)
    borders.append(sample_bottom_border)
    sample_top_border = GS.border('sampletopborder', top_cylinder.name, sample_housing.name)
    borders.append(sample_top_border)
    bottom_cube_border = GS.border('bottomcubeborder', bottom_cylinder.name, cube.name)
    borders.append(bottom_cube_border)
    top_cube_border = GS.border('bottomcubeborder', top_cylinder.name, cube.name)
    borders.append(top_cube_border)  

# Photodiode support
PHOTODIODE_SUPPORT_HEIGHT = LIGHT_HOLE_HEIGHT_FROM_BOTTOM - bottom_arm.height - BASE_PLATE_HEIGHT - PHOTODIODE_HEIGHT/2.0
assert PHOTODIODE_SUPPORT_HEIGHT >= 0
photodiode_support = GS.TubeVolume('photodiodesupport',  bottom_arm.depth/4.0, PHOTODIODE_SUPPORT_HEIGHT, 0.0) 
photodiode_support.material = 'aluminum'
photodiode_support.mother = 'cube'
photodiode_support.colorVect[3] = 1.0
y_support_offset, z_support_offset = trig_distances(0.4 * PHOTODIODE_DISTANCE_FROM_SAMPLE, PHOTODIODE_THETA)
photodiode_support.center = {'x': bottom_arm.center['x'] + bottom_arm.height/2.0 + photodiode_support.height/2.0, 'y': bottom_arm.center['y'] + y_support_offset, 'z': bottom_arm.center['z'] + z_support_offset} 
photodiode_support.rotation[0] = -PHOTODIODE_THETA
photodiode_support.rotation[1] = 90.0
masterString = photodiode_support.writeToString(masterString)

support_arm_border = GS.border('supportarmborder', photodiode_support.name, bottom_arm.name)
borders.append(support_arm_border)
support_cube_border = GS.border('supportcubeborder', photodiode_support.name, cube.name)
borders.append(support_cube_border)

# Photodiode 
photodiode = GS.TubeVolume('photodiode', PHOTODIODE_HEIGHT/2.0, 3.5, 0.0)
PD_X = photodiode_support.center['x'] + photodiode_support.height/2 + photodiode.rMax
photodiode.material = 'aluminum'
photodiode.mother = 'cube'
photodiode.colorVect[3] = 1.0
y_pd_offset, z_pd_offset = trig_distances(photodiode_support.rMax, PHOTODIODE_THETA)
photodiode.center = {'x': PD_X, 'y': photodiode_support.center['y'] - y_pd_offset, 'z': photodiode_support.center['z'] - z_pd_offset}
photodiode.rotation[0] = 90.0 - PHOTODIODE_THETA

assert photodiode.center['y'] - photodiode.height/2.0 > cube_negative_y
apparatusHeight = cube_negative_x + plate.height + bottom_arm.height + photodiode_support.height + photodiode.rMax
lightHeight = light_hole.center['x']
assert apparatusHeight == photodiode.center['x'] and lightHeight == apparatusHeight
masterString = photodiode.writeToString(masterString)
photodiode_support_border = GS.border('photodiodesupportborder', photodiode.name, photodiode_support.name) 
borders.append(photodiode_support_border)
photodiode_cube_border = GS.border('photodiodecubeborder', photodiode.name, cube.name) 

# Photodiode test volume
photodiodetest = GS.TubeVolume('photodiodetest', photodiode.rMax, 0.01, 0.0) 
photodiodetest.material = 'pmt_vacuum' 
photodiodetest.mother = 'cube'
photodiodetest.colorVect[3] = 1.0
y_pdt_offset, z_pdt_offset = trig_distances(1e-1 + photodiode.height/2.0 + photodiodetest.height/2.0, PHOTODIODE_THETA)
photodiodetest.center = {'x': photodiode.center['x'], 'y': photodiode.center['y'] - y_pdt_offset, 'z': photodiode.center['z'] - z_pdt_offset} 
photodiodetest.rotation[0] = 90.0 - PHOTODIODE_THETA
masterString = photodiodetest.writeToString(masterString)

if CYLINDER_FLAG:
    sample_mother = LAR
else: 
    sample_mother = cube

perf = GS.HoledBox('sampleholder', 6.25, 1.25, 0.33, NUM_SAMPLE_HOLES, 0.5, 0.1)
perf.material = 'aluminum'
perf.mother = sample_mother.name
perf.colorVect[3] = 0.3

if not CYLINDER_FLAG:
    perf.center = {'x': light_hole.center['x'], 'y': sample_housing.center['y'],  'z': sample_housing.center['z']}
    perf.rotation[0] = 90.0 - SAMPLE_HOUSING_ANGLE
    height_offset = perf.center['x'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1] - light_hole.center['x']
    perf.center['x'] -= height_offset
    hole_locations = perf.x

if CYLINDER_FLAG:
    perf.rotation[1] = 90.0
    perf.center['z'] = -LAR.center['x'] + light_hole.center['x'] 
    height_offset = perf.center['z'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1] - light_hole.center['x'] + LAR.center['x']
    perf.center['z'] -= height_offset
    perf_z = perf.center['z']
    hole_locations = perf.x 
masterString = perf.writeToString(masterString)

sample_holder_border = GS.border('perfborder', perf.name, cube.name) 
#masterString = sample_holder_border.writeToString(masterString); 

hole_locations = perf.x

for i in range(1, len(perf.x) + 1):
    height = float(perf.x[i-1])
    hole = GS.TubeVolume('hole_%d' % i, 0.5, 0.33, 0.0)
    hole.material = 'acrylic_suvt'
    hole.mother = sample_mother.name
    hole.colorVect[3] = 0.9
    if not CYLINDER_FLAG:
        hole.rotation[0] = perf.rotation[0]
        hole.center = {'x': perf.center['x'] + height, 'y': sample_housing.center['y'], 'z': sample_housing.center['z']} 
    if CYLINDER_FLAG:
        hole.rotation[1] = 90.0
        hole.center = {'x': 0.0, 'y': 0.0, 'z': perf_z - height}  
        
    masterString = hole.writeToString(masterString)
   
    hole_border = GS.border('hole_border_%d' % i, perf.name, hole.name) 
    hole_border.mother = sample_mother
    #masterString = hole_border.writeToString(masterString) 

    tpb = GS.TubeVolume('tpb_%d' % i, hole.rMax, TPB_THICKNESS * 3.93701*10**(-5), 0.0)
    tpb.material = 'tpb'
    tpb.mother = sample_mother.name
    tpb.colorVect[3] = 1.0
    tpb.rotation[0] = perf.rotation[0]
    y_tpb, z_tpb = trig_distances(hole.height/2.0 + tpb.height/2.0 + 1e-6, 90 - tpb.rotation[0])
    
    if not CYLINDER_FLAG:
        tpb.center['x'] = hole.center['x']
        tpb.center['y'] = hole.center['y'] - y_tpb # hole.height/2.0 - tpb.height/2.0  #y_tpb
        tpb.center['z'] = hole.center['z'] - z_tpb 
        print("Tpb z: %f, tpb y: %f" % (z_tpb, y_tpb)) 
    else: 
        tpb.rotation[1] = 90.0
        tpb.center = {'y': 0.0, 'x': hole.height/2.0 + tpb.height/2.0 + 1e-6, 'z': hole.center['z']}
    masterString = tpb.writeToString(masterString)

    tpb_surface = GS.border('tpb_surface_%d' % i, sample_mother.name, tpb.name)
    tpb_surface.mother = sample_mother.name
    tpb_surface.surface = 'tpb_surface_border'
    masterString = tpb_surface.writeToString(masterString)

    sample_test_hole = GS.TubeVolume("test%d" % i, hole.rMax, 0.05, 0.0)
    sample_test_hole.material = 'pmt_vacuum'
    sample_test_hole.mother = sample_mother
    sample_test_hole.colorVect[3] = 1.0
    sample_test_hole.rotation[0] = perf.rotation[0]
    y_sample, z_sample = trig_distances(1e-6 + hole.height/2.0 + sample_test_hole.height/2.0, 90 - sample_test_hole.rotation[0])
    print("sampledisk z: %f, sampledisk y: %f" % (z_sample, y_sample)) 
    sample_test_hole.center['x'] = hole.center['x']
    sample_test_hole.center['y'] = hole.center['y'] + y_sample # 1e-6 + hole.height/2.0 + sample_test_hole.height/2.0 # y_sample
    sample_test_hole.center['z'] = hole.center['z'] + z_sample
    #masterString = sample_test_hole.writeToString(masterString)

slitGenPlane = GS.BoxVolume('slitGenPlane', 5*slit_width, 5*slit_height, 0.1)
slitGenPlane.material = 'pmt_vacuum'
slitGenPlane.mother = 'light_hole'
slitGenPlane.colorVect[3] = 1.0
slitGenPlane.center['x'] = 0.0
slitGenPlane.center['y'] = 0.0
masterString = slitGenPlane.writeToString(masterString)

print("Lighthole: %f, Slit: %f, Sample: %f, Photodiode: %f" % (light_hole.center['x'], light_hole.center['x'] + slitGenPlane.center['x'], perf.center['x'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1], photodiode.center['x'])) 
print("Lighthole: %f, Slit: %f, Sample: %f, Photodiode: %f" % (light_hole.center['z'], light_hole.center['z'] + slitGenPlane.center['z'], perf.center['z'], photodiode.center['z'])) 

#print(holes[HOLE_IN_FRONT_OF_LIGHT - 1].center['x'] - sample_holder.center['x']) 
TEST_CYLINDER = GS.TubeVolume('TESTCYLINDER', PHOTODIODE_DISTANCE_FROM_SAMPLE + 0.5, 4.0, PHOTODIODE_DISTANCE_FROM_SAMPLE) 
TEST_CYLINDER.center = perf.center
TEST_CYLINDER.rotation[1] = 90.0 
TEST_CYLINDER.colorVect[3] = 1.0
TEST_CYLINDER.mother = 'cube'
TEST_CYLINDER.material = 'pmt_vacuum'
#masterString = TEST_CYLINDER.writeToString(masterString)

# Sample support
if not CYLINDER_FLAG:
    SAMPLE_SUPPORT_HEIGHT = cube_positive_x - perf.center['x'] - perf.height/2.0
else:
    SAMPLE_SUPPORT_HEIGHT = LAR.height/2.0 - perf_z - perf.height/2.0
sample_support = GS.TubeVolume('samplesupport', perf.depth/2.0, SAMPLE_SUPPORT_HEIGHT, 0.0)
sample_support.material = 'aluminum'
sample_support.mother = sample_mother.name
sample_support.colorVect[3] = 0.7
#sample_support.rotation[2] = 90.0 
if not CYLINDER_FLAG: 
    sample_support.rotation[1] = 90.0
    sample_support.center = {'x': perf.center['x'] + (perf.height+sample_support.height)/2.0, 'y': perf.center['y'], 'z': perf.center['z']}
else:
   sample_support.center = {'x': 0.0, 'y': 0.0, 'z': perf_z + (perf.height+sample_support.height)/2.0}
   print(perf.center['z'] + LAR.center['x'])
masterString = sample_support.writeToString(masterString)
support_cube_border = GS.border('supportcubeborder', sample_support.name, cube.name)
borders.append(support_cube_border)

if CYLINDER_FLAG:
    ORIENTATION = GS.BoxVolume('orientation', 3.0, 3.0, 0.1)
    ORIENTATION.mother = 'LAr'
    ORIENTATION.material = 'pmt_vacuum'
    ORIENTATION.center['z'] = perf_z + perf.height/2.0
    #masterString = ORIENTATION.writeToString(masterString)

#support_holder_border = GS.border('supportholderborder', sample_support.name, sample_holder.name) 
#borders.append(support_holder_border)

#for border in borders:
    #masterString = border.writeToString(masterString)
out_file = open(file_name, 'w+')
out_file.write(masterString)
out_file.close()

