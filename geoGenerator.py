import geoShapes as GS

masterString = ''
file_name = './VUV.geo'

# Create World
world = GS.boxVolume('world',400, 400,400)
world.material = 'acrylic_black'
world.mother = ''
world.invisible = 1
masterString = world.writeToString(masterString)

# Create outside cube
cube = GS.BoxVolume('cube', 10, 10, 10)
cube.material = 'acrylic_black'
cube.mother = 'world'
masterString = cube.writeToString(masterString)

# Create cylinder
cylinder = GS.tubeVolume('cylinder', 5, 10, 0)
cylinder.material = 'acrylic_black'
cylinder.mother = 'cube'
masterString = cylinder.writeToString(masterString)

out_file = open(file_name, 'w+')
out_file.write(masterString)
out_file.close()

