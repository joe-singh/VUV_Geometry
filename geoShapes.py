"""
Geometry class for RAT. 
Jyotirmai Singh 6/8/18
"""

import random

class geoShapes:

    def __init__(self, centerCoord={'x':0,'y':0,'z':0 }):
        self.center = centerCoord
        self.material = 'pmt_vacuum'
        self.mother = 'world'
        self.invisible = 1
        self.colorVect = [random.random(),random.random(),random.random(),0.5]
        self.drawstyle = 'solid'
    

