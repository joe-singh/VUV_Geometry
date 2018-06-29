"""
Geometry class for RAT. 
Chris Benson 11/12/2015
"""

import random


def inTomm(inch):
    return inch * 25.4


class GeoVolume:
    def __init__(self, center_coord={'x':0,'y':0,'z':0 }):
        self.center = center_coord
        self.material = 'pmt_vacuum'
        self.mother = 'world'
        self.invisible = 1
        self.colorVect = [random.random(),random.random(),random.random(),0.5]
        self.drawstyle = 'solid'


class BoxVolume(GeoVolume):
    def __init__(self,name,height,width,depth):
        GeoVolume.__init__(self)
        self.name = name
        self.height = height
        self.width = width
        self.depth = depth
        self.volType = 'box'
        self.invisible = 0
        self.rotation = [0,0,0]

    def writeToString(self,OutputString):
        OutputString += '\n{\nname: "GEO",\nindex: "'+self.name+'",\nvalid_begin: [0,0],\nvalid_end: [0,0],\n'
        OutputString += 'type: "'+self.volType+'",\n'
        OutputString += 'mother: "'+self.mother+'",\n'
        OutputString += 'size: ['+str(inTomm(float(self.height))/2.0)+','+str(inTomm(float(self.width))/2.0)+','+str(inTomm(float(self.depth))/2.0)+'],\n'
        OutputString += 'position: ['+str(inTomm(float(self.center['x'])))+','+str(inTomm(float(self.center['y'])))+','+str(inTomm(float(self.center['z'])))+'],\n'
        OutputString += 'rotation: ['+str(float(self.rotation[0]))+','+str(float(self.rotation[1]))+','+str(float(self.rotation[2]))+'],\n'
        OutputString += 'material: "'+self.material+'",\n'
        OutputString += 'color: ['+str(self.colorVect[0])+','+str(self.colorVect[1])+','+str(self.colorVect[2])+','+str(self.colorVect[3])+'],\n'
        OutputString += 'drawstyle: "solid",\n'
        OutputString += 'invisible: '+str(int(self.invisible))+',\n}\n'

        return OutputString


class TubeVolume(GeoVolume):
    def __init__(self,name,rMax,height,rMin=0):
        GeoVolume.__init__(self)
        self.name = name
        self.rMax = rMax
        self.rMin = rMin
        self.height = height
        self.volType = 'tube'
        self.invisible = 0
        self.rotation = [0.0,0.0,0.0]

    def writeToString(self,OutputString):
        OutputString += '\n{\nname: "GEO",\nindex: "'+self.name+'",\nvalid_begin: [0,0],\nvalid_end: [0,0],\n'
        OutputString += 'type: "'+self.volType+'",\n'
        OutputString += 'mother: "'+self.mother+'",\n'
        OutputString += 'r_max: '+str(inTomm(float(self.rMax)))+',\n'
        OutputString += 'r_min: '+str(inTomm(float(self.rMin)))+',\n'
        OutputString += 'size_z: '+str(inTomm(float(self.height))/2.0)+',\n'
        OutputString += 'position: ['+str(inTomm(float(self.center['x'])))+','+str(inTomm(float(self.center['y'])))+','+str(inTomm(float(self.center['z'])))+'],\n'
        OutputString += 'rotation: ['+str(float(self.rotation[0]))+','+str(float(self.rotation[1]))+','+str(float(self.rotation[2]))+'],\n'
        OutputString += 'material: "'+self.material+'",\n'
        OutputString += 'color: ['+str(self.colorVect[0])+','+str(self.colorVect[1])+','+str(self.colorVect[2])+','+str(self.colorVect[3])+'],\n'
        OutputString += 'drawstyle: "solid",\n'
        OutputString += 'invisible: '+str(int(self.invisible))+',\n}\n'
        return OutputString

#### Classes for a border
class border:
    def __init__(self,name,volume1,volume2):
        # to find more surfaces to choose from, refer to $RATROOT/data/OPTICS.ratdb
        self.surface = 'stainless_steel'
        self.mother = 'world'
        self.vol1 = volume1
        self.vol2 = volume2
        self.name = name
        self.volType = 'border'
        self.colorVect = [random.random(),random.random(),random.random(),0.5]
        self.invisible = 0
        self.drawstyle = 'solid'

    def writeToString(self,OutputString):
        OutputString += '\n{\nname: "GEO",\nindex: "'+self.name+'",\nvalid_begin: [0,0],\nvalid_end: [0,0],\n'
        OutputString += 'type: "'+self.volType+'",\n'
        OutputString += 'mother: "'+self.mother+'",\n'
        OutputString += 'volume1: "'+self.vol1+'",\n'
        OutputString += 'volume2: "'+self.vol2+'",\n'
        OutputString += 'surface: "'+self.surface+'",\n'
        OutputString += 'color: ['+str(self.colorVect[0])+','+str(self.colorVect[1])+','+str(self.colorVect[2])+','+str(self.colorVect[3])+'],\n'
        OutputString += 'drawstyle: "solid",\n'
        OutputString += 'invisible: '+str(int(self.invisible))+',\n}\n'
        return OutputString
