import sys
import rat
import random
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

inf = sys.argv[1]


def track_photon(infile):

    file_iterator = rat.dsreader(infile)
    tracking_dict = {}

    for iEntry, anEntry in enumerate(file_iterator):
        if iEntry % 1000 == 0 and iEntry > 0:
            print(iEntry)
        tracking_dict[iEntry] = []
        tempMC = anEntry.GetMC()
        num_tracks = tempMC.GetMCTrackCount()
#        print("Tracks: %d" % num_tracks)

        photon_track = tempMC.GetMCTrack(0)
        photon_first_step = photon_track.GetMCTrackStep(0)
         
        for iTrack in range(num_tracks):
            tempTrack = tempMC.GetMCTrack(iTrack)
 #           print("Steps: ", tempTrack.GetMCTrackStepCount())
            for iStep in range(tempTrack.GetMCTrackStepCount()):
                if iStep == 0:
		    continue
                step = tempTrack.GetMCTrackStep(iStep)
                track_end_point = step.GetEndpoint()
                xPos = track_end_point.x()
                yPos = track_end_point.y()
                zPos = track_end_point.z()
                tracking_dict[iEntry] += [(xPos, yPos, zPos)]
    
#    print(tracking_dict) 

    for particle in tracking_dict:
        coordinates = tracking_dict[particle]
        x_coordinates = [coordinate[0] for coordinate in coordinates]
        y_coordinates = [coordinate[1] for coordinate in coordinates]
        z_coordinates = [coordinate[2] for coordinate in coordinates]
        tracking_dict[particle] = [x_coordinates, y_coordinates, z_coordinates]

    fig = plt.figure()
    ax = Axes3D(fig)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    
    ax.view_init(180, 0)
    ax.set_xlim3d(-25*25.4, 25*25.4)
    ax.set_ylim3d(-25*25.4, 25*25.4)
    ax.set_zlim3d(-25*25.4, 25*25.4)
  #  ax.set_ylim3d(-1,1)
  #  ax.set_zlim3d(-1,1)
    # plot 10 random particles
    for i in range(2000):

        particle_number = random.randint(0, len(tracking_dict)-1)
        x_array = np.array(tracking_dict[particle_number][0])
        y_array = np.array(tracking_dict[particle_number][1])
        z_array = np.array(tracking_dict[particle_number][2])
        ax.scatter(x_array, y_array, z_array, c = 'blue', s=0.5)
    
    plt.show()

track_photon(inf)
