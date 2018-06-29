import geoShapes as GS
import numpy as np 

masterString = ''
file_name = './VUV.geo'

# NOTE 12inches in 1 foot


# True if including all cylinders. False if only quartz.
CYLINDER_FLAG = True

TPB_THICKNESS = 4.0 # NOTE Micrometers!!
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
PHOTODIODE_THETA = 180.0 + 45.0
SAMPLE_HOUSING_ANGLE = 45.0

PHOTODIODE_HEIGHT = 1.25
NUM_SAMPLE_HOLES = 4
HOLE_IN_FRONT_OF_LIGHT = 3
assert 0 < HOLE_IN_FRONT_OF_LIGHT <= NUM_SAMPLE_HOLES

def deg_to_rad(deg):
    return np.pi * (deg / 180)

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
cube.colorVect[3] = 0.2

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
plate.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT/2.0, 'y': cube_negative_y + 10, 'z': 0}
masterString = plate.writeToString(masterString)

# Plate-cube border
border_plate_cube = GS.border('borderplatecube', plate.name, cube.name)
borders.append(border_plate_cube) 

BOTTOM_HEIGHT = LIGHT_HOLE_HEIGHT_FROM_BOTTOM - plate.height - PHOTODIODE_BOTTOM_ARM_THICKNESS - SAMPLE_CYLINDER_HEIGHT/2.0

# Create cylinder which will house the samples
sample_housing = GS.TubeVolume('samplehousing', SAMPLE_CYLINDER_RADIUS, SAMPLE_CYLINDER_HEIGHT, SAMPLE_CYLINDER_RADIUS - 0.25)
sample_housing.material = 'quartz'
sample_housing.mother = 'cube'
sample_housing.colorVect[3] = 0.5
x_sample_housing = cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS +  SAMPLE_CYLINDER_HEIGHT/2.0
sample_housing.center = {'x': x_sample_housing, 'y': cube_negative_y + 10, 'z': 0}
sample_housing.rotation[1] = 90.0

sample_center_y, sample_center_z = sample_housing.center['y'], sample_housing.center['z']
LIGHT_HOLE_CENTRE_HEIGHT = cube_negative_x + LIGHT_HOLE_HEIGHT_FROM_BOTTOM

# Top and bottom cylinders
if CYLINDER_FLAG:
    BOTTOM_HEIGHT = LIGHT_HOLE_HEIGHT_FROM_BOTTOM - plate.height - PHOTODIODE_BOTTOM_ARM_THICKNESS - SAMPLE_CYLINDER_HEIGHT/2.0 
    bottom_cylinder = GS.TubeVolume('bottomcylinder', SAMPLE_CYLINDER_RADIUS, BOTTOM_HEIGHT, SAMPLE_CYLINDER_RADIUS - 0.25)
    bottom_cylinder.material = 'aluminum'
    bottom_cylinder.mother = 'cube'
    bottom_cylinder.colorVect[3] = 0.6
    bottom_cylinder.center = {'x': cube_negative_x + plate.height + PHOTODIODE_BOTTOM_ARM_THICKNESS + BOTTOM_HEIGHT/2.0, 'y': sample_center_y, 'z': sample_center_z}
    bottom_cylinder.rotation[1] = 90.0
    sample_housing.center['x'] += BOTTOM_HEIGHT
    masterString = bottom_cylinder.writeToString(masterString)
   
    top_height = cube.height - bottom_cylinder.height - sample_housing.height - plate.height - PHOTODIODE_BOTTOM_ARM_THICKNESS
    top_cylinder = GS.TubeVolume('topcylinder', SAMPLE_CYLINDER_RADIUS, top_height, SAMPLE_CYLINDER_RADIUS - 0.25)
    top_cylinder.mother = 'cube'
    top_cylinder.material = 'aluminum'
    top_cylinder.colorVect[3] = 0.6
    top_cylinder.center = {'x': sample_housing.center['x'] + sample_housing.height/2.0 + top_cylinder.height/2.0, 'y': sample_center_y, 'z': sample_center_z}
    top_cylinder.rotation[1] = 90.0
    masterString = top_cylinder.writeToString(masterString)
    assert sample_housing.center['x'] == LIGHT_HOLE_CENTRE_HEIGHT
    assert bottom_cylinder.center['x'] + bottom_cylinder.height/2.0 == sample_housing.center['x'] - sample_housing.height/2.0 and \
           sample_housing.center['x'] + sample_housing.height/2.0 == top_cylinder.center['x'] - top_cylinder.height/2.0 and \
           top_cylinder.center['x'] + top_cylinder.height/2.0 == cube_positive_x

masterString = sample_housing.writeToString(masterString)

# Add hole for light to come in from
light_hole = GS.TubeVolume('lighthole', LIGHT_HOLE_DIAMETER/2.0, 10, 0)
light_hole.material = 'pmt_vacuum'
light_hole.mother = 'world'
light_hole.colorVect[3] = 0.2
light_hole.rotation[0] = 90.0
light_hole.center = {'x': LIGHT_HOLE_CENTRE_HEIGHT,'y': cube_negative_y - light_hole.height/2.0, 'z': 0}
masterString = light_hole.writeToString(masterString)
#light_cube_border = GS.border('lightcubeborder', light_hole.name, cube.name) 
#borders.append(light_cube_border)

# Add rectangular prism to simulate slit
slitGenPlane = GS.BoxVolume('slitGenPlane', slit_height, 0.05, slit_width)
slitGenPlane.material = 'pmt_vacuum'
slitGenPlane.mother = 'world'
slitGenPlane.colorVect[3] = 0.5
slitGenPlane.center = {'x': light_hole.center['x'], 'y': light_hole.center['y'], 'z': 0.0}
masterString = slitGenPlane.writeToString(masterString)
#slitCubeBorder = GS.border('slitcubeborder', slitGenPlane.name, cube.name)
#slit_hole_border = GS.border('slitholeborder', slitGenPlane.name, light_hole.name)
#borders.append(slitCubeBorder)
#borders.append(slit_hole_border)

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

sample_base_border = GS.border('samplebaseborder', sample_housing.name, bottom_arm.name)
borders.append(sample_base_border)
sample_cube_border = GS.border('samplecubeborder', sample_housing.name, cube.name)
borders.append(sample_cube_border)
arm_cube_border = GS.border('armcubeborder', bottom_arm.name, cube.name)

if CYLINDER_FLAG:
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
borders.append(photodiode_cube_border)

# Sample holder
sample_holder = GS.BoxVolume("sampleholder", 6.25, 1.25, 0.33)
sample_holder.material = 'aluminum'
sample_holder.mother = 'cube'
sample_holder.colorVect[3] = 0.7
sample_holder.center = {'x': light_hole.center['x'], 'y': sample_housing.center['y'],  'z': sample_housing.center['z']}
sample_holder.rotation[0] = 90.0 - SAMPLE_HOUSING_ANGLE

# Holes on the sample holder. 
Delta_H = sample_holder.height/(float(NUM_SAMPLE_HOLES + 1))
sample_holder_bottom = sample_holder.center['x'] - sample_holder.height/2.0

# Move sample housing up or down to put hole number HOLE_IN_FRONT_OF_LIGHT in front of the light hole
Delta_l = HOLE_IN_FRONT_OF_LIGHT * Delta_H
Delta_L = light_hole.center['x'] - sample_holder_bottom

sample_holder.center['x'] += Delta_L - Delta_l
sample_holder_bottom += Delta_L - Delta_l 

sample_cube_border = GS.border('samplecubeborder', sample_holder.name, cube.name) 
borders.append(sample_cube_border) 

masterString = sample_holder.writeToString(masterString)

# List to keep track of holes to move sample up or down. 
holes = []

for i in range(1, NUM_SAMPLE_HOLES + 1):
    hole = GS.TubeVolume('hole%d' % i, 0.5, sample_holder.depth, 0.0)
    hole.material = 'acrylic_suvt'
    hole.mother = 'cube'
    hole.colorVect[3] = 0.9
    hole.rotation[0] = sample_holder.rotation[0]
    hole.center = {'x': sample_holder_bottom + i * Delta_H, 'y': sample_holder.center['y'], 'z': sample_holder.center['z']}
    holes.append(hole) 
    masterString = hole.writeToString(masterString) 
    hole_sample_border = GS.border(hole.name + "sampleborder", hole.name, sample_holder.name) 
    borders.append(hole_sample_border) 

    # Apply tpb to hole
    tpb = GS.TubeVolume('tpb%d' % i, hole.rMax, TPB_THICKNESS * 3.93701*10**-5, 0.0) 
    tpb.material = 'tpb'
    tpb.mother = 'cube'
    tpb.colorVect[3] = 1.0
    tpb.rotation[0] = sample_holder.rotation[0] 
    y_tpb, z_tpb = trig_distances(hole.height/2.0 + tpb.height/2.0, 90 - tpb.rotation[0])
    tpb.center = {'x': hole.center['x'], 'y': hole.center['y'] - y_tpb, 'z': hole.center['z'] - z_tpb}
    hole_tpb_border = GS.border('tpb' + hole.name + 'border', tpb.name, hole.name)
#    tpb_cube_border = GS.border('tpb' + hole.name + 'cubeborder', tpb.name, cube.name)
    borders.append(hole_tpb_border) 
#    borders.append(tpb_cube_border)
    masterString = tpb.writeToString(masterString) 

assert holes[HOLE_IN_FRONT_OF_LIGHT - 1].center['x'] == light_hole.center['x']

# Sample support
SAMPLE_SUPPORT_HEIGHT = cube_positive_x - sample_holder.center['x'] - sample_holder.height/2.0
sample_support = GS.TubeVolume('samplesupport', sample_holder.depth/2.0, SAMPLE_SUPPORT_HEIGHT, 0.0)
sample_support.material = 'aluminum'
sample_support.mother = 'cube'
sample_support.colorVect[3] = 0.7
sample_support.rotation[1] = 90.0
sample_support.center = {'x': sample_holder.center['x'] + (sample_holder.height+sample_support.height)/2.0, 'y': sample_holder.center['y'], 'z': sample_holder.center['z']}
assert sample_support.center['x'] + sample_support.height/2.0 == cube_positive_x
assert sample_support.center['x'] - sample_support.height/2.0 == sample_holder.center['x'] + sample_holder.height/2.0
masterString = sample_support.writeToString(masterString)
support_cube_border = GS.border('supportcubeborder', sample_support.name, cube.name)
borders.append(support_cube_border)
support_holder_border = GS.border('supportholderborder', sample_support.name, sample_holder.name) 
borders.append(support_holder_border)

for border in borders:
    masterString = border.writeToString(masterString)
out_file = open(file_name, 'w+')
out_file.write(masterString)
out_file.close()

