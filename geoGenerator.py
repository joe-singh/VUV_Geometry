import geoShapes as GS

masterString = ''
file_name = './VUV.geo'

# Create World
world = GS.BoxVolume('world',400, 400,400)
world.material = 'acrylic_black'
world.mother = ''
world.invisible = 1
masterString = world.writeToString(masterString)

# Create outside cube
cube = GS.BoxVolume('cube', 5, 5, 5)
cube.material = 'pmt_vacuum'
cube.mother = 'world'
cube.colorVect[3] = 0.1
#cube.rotation = [45,0,0]
masterString = cube.writeToString(masterString)

# Create cylinder
cylinder = GS.TubeVolume('cylinder', 1, 5, 0)
cylinder.material = 'pmt_vacuum'
cylinder.mother = 'cube'
cylinder.colorVect[3] = 1.0
masterString = cylinder.writeToString(masterString)

# Create border
border = GS.border('border', cylinder, cube)

out_file = open(file_name, 'w+')
out_file.write(masterString)
out_file.close()

