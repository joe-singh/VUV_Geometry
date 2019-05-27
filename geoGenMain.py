import geoShapes as GS
import numpy as np 
import sys

masterString = ''

# Flag to place sample holder
SAMPLE_HOLDER_FLAG = True

# Command Line Inputs
SAMPLE_HOUSING_ANGLE = float(sys.argv[1]) 
filepath=sys.argv[12] # File path to append after prep, of form config/syst/side/sign/angle_ 
file_name = '/data/snoplus/home/joesingh/VUV/VUV_Geometry/macros/'+filepath+'/VUV_'+str(sys.argv[1])+'_'+str(sys.argv[2])+'_.geo'
TPB_THICKNESS = float(sys.argv[3]) # NOTE Micrometers!
CONFIG = sys.argv[4] # 'mirror' if mirror or 'tpb' if tpb 

# NOTE THE BEAM PROPAGATES ALONG THE Y AXIS IN THE +VE DIRECTION
 
# SYSTEMATICS, unless otherwise noted, put either 1, 0, -1 on these flags. For x = y +/- z 1 will use y + z, 0 will use y and -1 will use y - z

# PMT Aperture
APERTURE_SYSTEMATIC = int(sys.argv[5])
# Aperture Distance from sample 
PMT_DIST_SYSTEMATIC = int(sys.argv[6]) 
# Sampleholder Reflectivity, 0 for no reflectivity otherwise reflectivity on. 
REFLECTIVITY_SYSTEMATIC = int(sys.argv[7])
# Laser/Sampleholder misalignment systematic NOTE put the actual offset value in here, not 1 0 -1. 
LASER_AXIS_MISALIGNMENT = float(sys.argv[8]) 
# Sample Rotation axis misalignment
ROTATION_AXIS_MISALIGNMENT = int(sys.argv[9]) 
# PMT Axis Rotation misalignment parallel to beam
PMT_MISALIGNMENT_PARALLEL = int(sys.argv[10])
# PMT Axis Rotation misalignment transverse to beam
PMT_MISALIGNMENT_TRANSVERSE = int(sys.argv[11]) 

# NOTE For photodiode, theta = 0.0 puts it on the right side (y, z > 0). 
# For samples, theta = 0.0 means TPB is facing the left side where the light comes 
# in. Both rotations occur in the same direction though, there is just a 180 degree
# offset

SAMPLE_NORMAL = 180.0 + SAMPLE_HOUSING_ANGLE
# measure angles relative to sample normal
PHOTODIODE_THETA = SAMPLE_NORMAL + float(sys.argv[2]) 
#PERFBOX_HOLE_INNER_SURFACE_LAYER_THICKNESS = 0.001 # Set to zero to disable. Diameter of hole is constant regardelss of this value.
PERFBOX_HOLE_INNER_SURFACE_LAYER_THICKNESS = 0 # Set to zero to disable. Diameter of hole is constant regardelss of this value.

# True if including all cylinders. False if only quartz.
CYLINDER_FLAG = False 
SAMPLE_CYLINDER_RADIUS = 1.5
SAMPLE_CYLINDER_HEIGHT = 3.0
BASE_PLATE_HEIGHT = 0.75
LIGHT_HOLE_HEIGHT_FROM_BOTTOM = 8.0
LIGHT_HOLE_DIAMETER = 1.5
PHOTODIODE_BOTTOM_ARM_THICKNESS = 0.5
PHOTODIODE_DISTANCE_FROM_SAMPLE = 5.5 + PMT_DIST_SYSTEMATIC * 0.125
PHOTODIODE_PHOTOSENSITIVE_SURFACE_OFFSET = 0.1

BEAM_FWHM = 0.1535 # inch
BEAM_SIGMA = BEAM_FWHM/(2*np.sqrt(2*np.log(2)))
slit_width = 5*BEAM_SIGMA # 5 sigma when generating gaussian profile beam
#slit_height = 5*BEAM_SIGMA # 5 sigma when generating gaussian profile beam
#slit_width = BEAM_FWHM # FWHM with generating flat top beam with diameter FWHM
 
PHOTODIODE_HEIGHT = 0.38 + APERTURE_SYSTEMATIC * 0.05 
NUM_SAMPLE_HOLES = 4
HOLE_IN_FRONT_OF_LIGHT = 2
assert 0 < HOLE_IN_FRONT_OF_LIGHT <= NUM_SAMPLE_HOLES

# Thickness of sample disk
sample_disk_thicknesses = {'acrylic': 0.116,
                           'mirror_1': 0.046,
                           'mirror_2': 0.120}

sample_thickness = sample_disk_thicknesses['mirror_1']

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
#sample_housing.material = 'quartz_suprasil'
sample_housing.material = 'pmt_vacuum'
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
#LAR.material = 'liquid_Ar'
LAR.material = 'pmt_vacuum'
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

if CYLINDER_FLAG:
    masterString = sample_housing.writeToString(masterString)    
    masterString = LAR.writeToString(masterString) 

# Add hole for light to come in from
light_hole = GS.TubeVolume('light_hole', LIGHT_HOLE_DIAMETER/2.0, 10, 0)
light_hole.material = 'pmt_vacuum'
light_hole.mother = 'world'
light_hole.invisible = 0
light_hole.colorVect[3] = 0.2
light_hole.rotation[0] = 90.0
light_hole.center = {'x': LIGHT_HOLE_CENTRE_HEIGHT,'y': cube_negative_y - light_hole.height/2.0, 'z': -0.0}
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
bottom_arm = GS.BoxVolume('bottomarm', 0.5, 8, 2) 
bottom_arm.material = 'aluminum'
bottom_arm.mother = 'cube'
bottom_arm.colorVect[3] = 0.5
THETA_RAD = deg_to_rad(PHOTODIODE_THETA)
y_arm_offset, z_arm_offset = trig_distances(bottom_arm.width/2, PHOTODIODE_THETA)

# Photiodiode Axis Shift Systematic:

y_bottom = 0.0 + PMT_MISALIGNMENT_PARALLEL * 0.25 + (bottom_arm.width/2)*np.cos(deg_to_rad(PHOTODIODE_THETA))
z_bottom = 0.0 + PMT_MISALIGNMENT_TRANSVERSE * 0.25 + (bottom_arm.width/2)*np.sin(deg_to_rad(PHOTODIODE_THETA))

#bottom_arm.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + bottom_arm.height/2.0, 'y': sample_housing.center['y'] + y_arm_offset, 'z': sample_housing.center['z'] + z_arm_offset}
bottom_arm.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + bottom_arm.height/2.0, 'y': sample_housing.center['y']+ y_bottom, 'z': sample_housing.center['z'] + z_bottom}

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
y_support_offset, z_support_offset = trig_distances(PHOTODIODE_DISTANCE_FROM_SAMPLE - 0.5*bottom_arm.width + 0.5*3.5 + photodiode_support.rMax + PHOTODIODE_PHOTOSENSITIVE_SURFACE_OFFSET, PHOTODIODE_THETA)
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
#assert apparatusHeight == photodiode.center['x'] and lightHeight == apparatusHeight
masterString = photodiode.writeToString(masterString)
photodiode_support_border = GS.border('photodiodesupportborder', photodiode.name, photodiode_support.name) 
borders.append(photodiode_support_border)
photodiode_cube_border = GS.border('photodiodecubeborder', photodiode.name, cube.name) 

# Photodiode test volume
photodiodetest = GS.TubeVolume('photodiodetest', photodiode.rMax, 0.01, 0.0) 
photodiodetest.material = 'pmt_vacuum' 
photodiodetest.mother = 'cube'
photodiodetest.colorVect[3] = 1.0
y_pdt_offset, z_pdt_offset = trig_distances(PHOTODIODE_PHOTOSENSITIVE_SURFACE_OFFSET + photodiode.height/2.0 + photodiodetest.height/2.0, PHOTODIODE_THETA)
photodiodetest.center = {'x': photodiode.center['x'], 'y': photodiode.center['y'] - y_pdt_offset, 'z': photodiode.center['z'] - z_pdt_offset} 
photodiodetest.rotation[0] = 90.0 - PHOTODIODE_THETA
masterString = photodiodetest.writeToString(masterString)

if CYLINDER_FLAG:
    sample_mother = LAR
else: 
    sample_mother = cube

if SAMPLE_HOLDER_FLAG:
    perf = GS.HoledBox('sampleholder', 6.155, 1.25, 0.35, NUM_SAMPLE_HOLES, 0.505 + PERFBOX_HOLE_INNER_SURFACE_LAYER_THICKNESS, 0.0)
    perf.material = 'aluminum'
    perf.mother = sample_mother.name
    perf.colorVect[3] = 0.3

    sampleholder_thickness = 0.35
    sample_disk_offset = 0.04 
    sample_axis_shift = sampleholder_thickness/2.0 - sample_disk_offset  #Accounting for the fact that the face is 0.04 inches behind the front face of the sample


    print("ROTATION_AXIS_MISALIGNMENT %s" % ROTATION_AXIS_MISALIGNMENT)
    if ROTATION_AXIS_MISALIGNMENT:
        # Done under assumption that misaligned axis is towards photon source. If not, need to swap +/- signs later on.  
        y_shift_mich = 0.06
        z_shift_mich = -0.02

        # Mich's origin is at front face of sample and transverse origin is centre of laser beam. Shift variables accordingly. 
        # Our coordinate system is at centre of mass of perf.  
        sample_axis_shift_parallel_y =  y_shift_mich - sample_thickness/2.0 #Sign of this important, determines validity of assumption. 
        sample_axis_shift_transverse_z = z_shift_mich - LASER_AXIS_MISALIGNMENT 

        phi = np.arctan(sample_axis_shift_transverse_z/sample_axis_shift_parallel_y) 
        radius = np.sqrt(sample_axis_shift_transverse_z**2 + sample_axis_shift_parallel_y**2) 
        #perf.rotation[0] = 90 - SAMPLE_HOUSING_ANGLE
        perf.rotation[0] = 90-SAMPLE_HOUSING_ANGLE
        perf_rot_radians = deg_to_rad(SAMPLE_HOUSING_ANGLE)

        z_trig_term =  radius * np.cos(perf_rot_radians + phi - np.pi/2.0)
        y_trig_term = radius * np.sin(perf_rot_radians + phi - np.pi/2.0)

        z_perf = sample_axis_shift_transverse_z + z_trig_term 
        y_perf = sample_axis_shift_parallel_y - y_trig_term  # These +/- may need to be swapped! 
        print("y_perf: %s = %s - %s" % (y_perf, sample_axis_shift_parallel_y, y_trig_term))
        print("z_perf: %s = %s + %s" % (z_perf, sample_axis_shift_transverse_z, z_trig_term)) 
        perf.center = {'x': light_hole.center['x'], 'y':sample_housing.center['y'] + y_perf,  'z': sample_housing.center['z'] + z_perf}

        height_offset = perf.center['x'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1] - light_hole.center['x']
        perf.center['x'] -= height_offset
        hole_locations = perf.x

    if not CYLINDER_FLAG:
        perf.rotation[0] = 90 - SAMPLE_HOUSING_ANGLE
        z_perf, y_perf = trig_distances(sample_axis_shift, perf.rotation[0])
        print("Yperf %s, Zperf %s" % (y_perf, z_perf))
        perf.center = {'x': light_hole.center['x'], 'y': sample_housing.center['y'] + y_perf,  'z': sample_housing.center['z'] + z_perf}
        if LASER_AXIS_MISALIGNMENT:
            perf.center = {'x': light_hole.center['x'], 'y': sample_housing.center['y'] + y_perf,  'z': sample_housing.center['z'] + z_perf + LASER_AXIS_MISALIGNMENT}
        print("PERF CENTER - SHIFT: %s" % (perf.center['y'] - y_perf))
        height_offset = perf.center['x'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1] - light_hole.center['x']
        perf.center['x'] -= height_offset
        hole_locations = perf.x

    elif CYLINDER_FLAG:
        perf.rotation[1] = 90.0
        perf.center['z'] = -LAR.center['x'] + light_hole.center['x'] 
        height_offset = perf.center['z'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1] - light_hole.center['x'] + LAR.center['x']
        perf.center['z'] -= height_offset
        perf_z = perf.center['z']
        hole_locations = perf.x 
    masterString = perf.writeToString(masterString)
    print("Perf centre (%s, %s) at %s deg incidence" % (perf.center['y'], perf.center['z'], SAMPLE_HOUSING_ANGLE))


    sample_holder_border = GS.border('perfborder', cube.name, perf.name) 
    sample_holder_border.mother = cube.name
    sample_holder_border.surface = 'aluminum'

    if REFLECTIVITY_SYSTEMATIC:
        masterString = sample_holder_border.writeToString(masterString)

    hole_locations = perf.x

    for i in range(1, len(perf.x) + 1):
        height = float(perf.x[i-1])
        hole = GS.TubeVolume('hole_%d' % i, 0.5,sample_thickness, 0.0) # make sure thickness is correct here!
        hole.material = 'acrylic_suvt'
        hole.mother = sample_mother.name
        hole.colorVect[3] = 0.9
        if not CYLINDER_FLAG:
            hole.rotation[0] = perf.rotation[0]
            y_hole, z_hole = trig_distances((0.35-sample_thickness)/2.0 - sample_disk_offset,90 - hole.rotation[0]) # Sample face is 0.04 inches behind holder face
            hole.center = {'x': perf.center['x'] + height, 'y': perf.center['y'] - y_hole, 'z': perf.center['z'] - z_hole} 
        if CYLINDER_FLAG:
            hole.rotation[1] = 90.0
            hole.center = {'x': 0.0, 'y': 0.0, 'z': perf_z - height}  
        masterString = hole.writeToString(masterString)

        if PERFBOX_HOLE_INNER_SURFACE_LAYER_THICKNESS > 0: # Inner surface of sample holder perforation
            perfbox_hole_inner_surface = GS.TubeVolume('perf_inner_surface_%d' % i, 0.505 + PERFBOX_HOLE_INNER_SURFACE_LAYER_THICKNESS, perf.depth, 0.505)
            perfbox_hole_inner_surface.material = 'aluminum'
            perfbox_hole_inner_surface.mother = cube.name
            perfbox_hole_inner_surface.rotation[0] = perf.rotation[0]
            perfbox_hole_inner_surface.center = {'x': perf.center['x'] + height, 'y': perf.center['y'], 'z': perf.center['z']}
            hole.colorVect[3] = 0.9
            masterString = perfbox_hole_inner_surface.writeToString(masterString)

        hole_border = GS.border('hole_border_%d' % i, perf.name, hole.name) 
        hole_border.mother = sample_mother
        #masterString = hole_border.writeToString(masterString) 

        # TPB Surface
        tpb = GS.TubeVolume('tpb_%d' % i, hole.rMax, TPB_THICKNESS * 3.93701*10**(-5), 0.0)
        tpb.material = 'tpb' #'pmt_vacuum' 
        tpb.mother = sample_mother.name
        tpb.colorVect[3] = 1.0
        tpb.rotation[0] = perf.rotation[0]
        y_tpb, z_tpb = trig_distances(hole.height/2.0 + tpb.height/2.0 + 1e-6, 90 - tpb.rotation[0])

        if not CYLINDER_FLAG:
            tpb.center['x'] = hole.center['x']
            tpb.center['y'] = hole.center['y'] - y_tpb # hole.height/2.0 - tpb.height/2.0  #y_tpb
            tpb.center['z'] = hole.center['z'] - z_tpb 
        else: 
            tpb.rotation[1] = 90.0
            tpb.center = {'y': 0.0, 'x': hole.height/2.0 + tpb.height/2.0 + 1e-6, 'z': hole.center['z']}

        tpb_surface = GS.border('tpb_surface_%d' % i, sample_mother.name, tpb.name)
        tpb_surface.mother = sample_mother.name
        tpb_surface.surface = 'tpb_surface_border'


        # Calibration mirror
        mirror_surface = GS.border('mirror_surface_%d' % i, sample_mother.name, hole.name)
        mirror_surface.mother = sample_mother.name
        mirror_surface.surface = 'mirror'

        if CONFIG == 'tpb':
            masterString = tpb.writeToString(masterString)
            masterString = tpb_surface.writeToString(masterString)
        elif CONFIG == 'mirror': 
            masterString = mirror_surface.writeToString(masterString)

        # Ring behind sample 
        ring = GS.TubeVolume('ring_%d' % i, hole.rMax, 0.077, 0.4525) 
        ring.material = 'acrylic_black'
        ring.mother = sample_mother.name 
        ring.rotation[0] = perf.rotation[0]
        y_ring, z_ring = trig_distances(hole.height/2.0 + ring.height/2.0 + 1e-6, 90 - ring.rotation[0])
        ring.center['x'] = hole.center['x']
        ring.center['y'] = hole.center['y'] + y_ring
        ring.center['z'] = hole.center['z'] + z_ring
        masterString = ring.writeToString(masterString) 


slitGenPlane = GS.TubeVolume('slitGenPlane', slit_width, 0.25, 0.0); 
slitGenPlane.material = 'pmt_vacuum'
slitGenPlane.mother = 'light_hole'
slitGenPlane.colorVect[3] = 1.0
slitGenPlane.center['x'] = 0.0
slitGenPlane.center['y'] = 0.0 #LASER_AXIS_MISALIGNMENT
masterString = slitGenPlane.writeToString(masterString)

if SAMPLE_HOLDER_FLAG:
    print("Lighthole: %f, Slit: %f, Sample: %f, Photodiode: %f" % (light_hole.center['x'], light_hole.center['x'] + slitGenPlane.center['x'], perf.center['x'] + perf.x[HOLE_IN_FRONT_OF_LIGHT - 1], photodiode.center['x'])) 
    print("Lighthole: %f, Slit: %f, Sample: %f, Photodiode: %f" % (light_hole.center['z'], light_hole.center['z'] + slitGenPlane.center['z'], perf.center['z'], photodiode.center['z'])) 

if SAMPLE_HOLDER_FLAG:
    #print(holes[HOLE_IN_FRONT_OF_LIGHT - 1].center['x'] - sample_holder.center['x']) 
    TEST_CYLINDER = GS.TubeVolume('TESTCYLINDER', PHOTODIODE_DISTANCE_FROM_SAMPLE + 0.5, 4.0, PHOTODIODE_DISTANCE_FROM_SAMPLE) 
    TEST_CYLINDER.center = perf.center
    TEST_CYLINDER.rotation[1] = 90.0 
    TEST_CYLINDER.colorVect[3] = 1.0
    TEST_CYLINDER.mother = 'cube'
    TEST_CYLINDER.material = 'pmt_vacuum'
    #masterString = TEST_CYLINDER.writeToString(masterString)

# Sample support
if SAMPLE_HOLDER_FLAG:
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

