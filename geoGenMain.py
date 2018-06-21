import geoShapes as GS
import numpy as np 

masterString = ''
file_name = './VUV.geo'

# NOTE 12inches in 1 foot


SAMPLE_CYLINDER_HEIGHT = 8.0
BASE_PLATE_HEIGHT = 0.75
LIGHT_HOLE_HEIGHT_FROM_BOTTOM = 8.0
LIGHT_HOLE_DIAMETER = 1.5
PHOTODIODE_BOTTOM_ARM_THICKNESS = 0.5
PHOTODIODE_DISTANCE_FROM_SAMPLE = 9.0
PHOTODIODE_THETA = 180.0
PHOTODIODE_HEIGHT = 1.25

def deg_to_rad(deg):
    return np.pi * (deg / 180)

def trig_distances(r, theta):
    y = r * np.cos(deg_to_rad(theta))
    z = r * np.sin(deg_to_rad(theta))
    return y, z

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
plate.material = 'pmt_vacuum'
plate.mother = 'cube'
plate.colorVect[3] = 0.5
plate.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT/2.0, 'y': cube_negative_y + 10, 'z': 0}
masterString = plate.writeToString(masterString)

# Create cylinder which will house the samples
sample_housing = GS.TubeVolume('samplehousing', 3, SAMPLE_CYLINDER_HEIGHT, 2.75)
sample_housing.material = 'quartz'
sample_housing.mother = 'cube'
sample_housing.colorVect[3] = 0.5
x_sample_housing = cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS +  SAMPLE_CYLINDER_HEIGHT/2.0
sample_housing.center = {'x': x_sample_housing, 'y': cube_negative_y + 10, 'z': 0}
sample_housing.rotation[1] = 90.0
masterString = sample_housing.writeToString(masterString)

# Other cylinder which goes above sample cylinder
OTHER_CYLINDER_HEIGHT = cube.height - BASE_PLATE_HEIGHT  - SAMPLE_CYLINDER_HEIGHT
other_cylinder = GS.TubeVolume('othercylinder', 3, OTHER_CYLINDER_HEIGHT, 2.75)
other_cylinder.material = 'aluminum'
other_cylinder.mother = 'cube'
other_cylinder.colorVect[3] = 0.1
x_other_cylinder = cube_negative_x + BASE_PLATE_HEIGHT + PHOTODIODE_BOTTOM_ARM_THICKNESS + SAMPLE_CYLINDER_HEIGHT + OTHER_CYLINDER_HEIGHT/2
other_cylinder.center = {'x':x_other_cylinder, 'y':cube_negative_y + 10, 'z':0}
other_cylinder.rotation[1] = 90.0
masterString = other_cylinder.writeToString(masterString)

# Add hole for light to come in from
LIGHT_HOLE_CENTRE_HEIGHT = cube_negative_x + LIGHT_HOLE_HEIGHT_FROM_BOTTOM 
light_hole = GS.TubeVolume('lighthole', LIGHT_HOLE_DIAMETER/2.0, 10, 0)
light_hole.material = 'pmt_vacuum'
light_hole.mother = 'world'
light_hole.colorVect[3] = 0.2
light_hole.rotation[0] = 90.0
light_hole.center = {'x': LIGHT_HOLE_CENTRE_HEIGHT,'y': cube_negative_y - light_hole.height/2.0, 'z': 0}
masterString = light_hole.writeToString(masterString)

# Add lever joining sample housing to photodiode 
# Assert statement ensures that rod doesn't break cube boundaries
assert PHOTODIODE_DISTANCE_FROM_SAMPLE < min(sample_housing.center['y'] - cube_negative_y, sample_housing.center['z'] - cube_negative_z)
bottom_arm = GS.BoxVolume('bottomarm', 0.5, PHOTODIODE_DISTANCE_FROM_SAMPLE, 2) 
bottom_arm.material = 'aluminum'
bottom_arm.mother = 'cube'
bottom_arm.colorVect[3] = 0.5
THETA_RAD = deg_to_rad(PHOTODIODE_THETA)
y_arm_offset, z_arm_offset = trig_distances(bottom_arm.width/2, PHOTODIODE_THETA)
bottom_arm.center = {'x': cube_negative_x + BASE_PLATE_HEIGHT + bottom_arm.height/2, 'y': sample_housing.center['y'] + y_arm_offset, 'z': sample_housing.center['z'] + z_arm_offset}
bottom_arm.rotation[0] = -PHOTODIODE_THETA
masterString = bottom_arm.writeToString(masterString) 


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
assert apparatusHeight == lightHeight
masterString = photodiode.writeToString(masterString)
#border = GS.border('border', cylinder, cube)

# Sample holder
sample_holder = GS.BoxVolume("samplehousing", 3.0, 1.25, 1.0)
sample_holder.material = 'aluminum'
sample_holder.mother = 'cube'
sample_holder.colorVect[3] = 1.0
sample_holder.center = {'x': 0.0, 'y': sample_housing.center['y'], 'z': sample_housing.center['z']}
#sample_holder.rotation[0] = 45.0
sample_holder.writeToString(masterString)


out_file = open(file_name, 'w+')
out_file.write(masterString)
out_file.close()

