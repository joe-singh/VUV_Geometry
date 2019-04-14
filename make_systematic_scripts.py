import sys
import os
import shutil

directory= "./shellscripts"
if os.path.exists(directory):
    shutil.rmtree(directory)
    
os.makedirs(directory) 
def write_shell_script(isBack, low_angle, high_angle, systematic, configuration, direction):

    
    if isBack:
        backstr = "back"
    else: 
        backstr = "front" 

    if direction == 1:
       dirstr = "plus"
    elif direction == -1:
       dirstr = "minus"
    else:
       dirstr = "nominal" 

    if configuration == 'mirror':
        n_photon = '10000'
        wavelength = '400.0'
    else:
        n_photon = '1000000'
        wavelength = '128.0'

    absolute_path = '/data/snoplus/home/joesingh/VUV/VUV_Geometry'
    systematic_path = configuration + "/" + systematic + "/" + backstr + "/" + dirstr
    systematic_name = configuration + "_" + systematic + "_" + backstr + "_" + dirstr + ".sh"
    filepath = absolute_path+"/shellscripts/" + systematic_name 
    sweep_dir = absolute_path+"/angle_sweep/" + systematic_path
    macro_dir = absolute_path+"/macros/"+ systematic_path 
    f = open(filepath, "w") 

  
    f.write("#!/bin/bash\n\n")
    
    f.write('if [ ! -d "' + sweep_dir + '" ]; then\n')
    f.write('    mkdir -p ' + sweep_dir + '\n') 
    f.write('fi\n')
    f.write('rm -rf '+sweep_dir+'/*\n') 
     
    f.write('if [ ! -d "'+macro_dir+'" ]; then\n')
    f.write('    mkdir -p '+macro_dir+'\n')
    f.write('fi\n')
    f.write('rm -rf '+macro_dir+'/*\n') 

    f.write('s_min='+str(low_angle)+'\n')
    f.write('s_max='+str(high_angle)+'\n')
    f.write('s=$s_min\n')
    f.write('while [ $s -le $s_max ]; do\n')
    f.write('    if [ ! -d "'+sweep_dir+'/angle_$s" ]; then\n')
    f.write('        mkdir '+sweep_dir+'/angle_$s\n')
    f.write('    fi\n\n')
    f.write('    if [ ! -d "'+macro_dir+'/angle_$s" ]; then\n')
    f.write('        mkdir '+macro_dir+'/angle_$s\n')
    f.write('    fi\n\n')

    f.write('    n_min=$((s - 10))\n')
    f.write('    n_max=$((s + 10))\n')
    f.write('    n=$n_min\n')
    f.write('    while [ $n -le $n_max ]\n')
    f.write('    do\n')

    if systematic == 'aperture': 
        f.write('        python '+absolute_path+'/geoGenMain.py $s $n 1.0 "' + configuration + '" ' + str(direction) + ' 0 0 0 0 '+systematic_path+'/angle_$s\n') 
    elif systematic == 'distance': 
        f.write('        python '+absolute_path+'/geoGenMain.py $s $n 1.0 "' + configuration + '" 0 ' + str(direction) + ' 0 0 0 '+systematic_path+'/angle_$s\n')
    elif systematic == 'reflection':
        f.write('        python '+absolute_path+'/geoGenMain.py $s $n 1.0 "' + configuration + '" 0 0 1 0 0 '+systematic_path+'/angle_$s\n')
    elif systematic == 'laser_axis':
        f.write('        python '+absolute_path+'/geoGenMain.py $s $n 1.0 "' + configuration + '" 0 0 0 0.2 0 '+systematic_path+'/angle_$s\n')
    elif systematic == 'rotation_axis':
        f.write('        python '+absolute_path+'/geoGenMain.py $s $n 1.0 "' + configuration + '" 0 0 0 0.2 1 '+systematic_path+'/angle_$s\n')
    elif systematic == 'default':
        f.write('        python '+absolute_path+'/geoGenMain.py $s $n 1.0 "' + configuration + '" 0 0 0 0 0 '+systematic_path+'/angle_$s\n')
 
    f.write('        python '+absolute_path+'/makePhotonMacro.py $s $n ' + wavelength + ' ' + n_photon + ' ' + systematic_path+'/angle_$s\n')
    f.write('   echo ">>> Done $s $n"\n')
    f.write('   sbatch -p debug --wait --comment="$@VUVSim_$s_$n" '+absolute_path+'/ratbatch.sh -o ' + sweep_dir + '/angle_$s/no_cylinder_angle_$s_$n.root '+macro_dir+'/angle_$s/photon_$s.$n.mac $@ >/dev/null 2>&1 &\n')
    f.write('   n=$[n+1]\n')
    f.write('   done\n')
    f.write('s=$[s+1]\n')
    f.write('done\n')
    f.write('wait')
         

configs = ['mirror']
back_checks = [True]#, False]
systematic = sys.argv[1] #['distance']#aperture, 'distance', 'reflection', 'laser_axis', 'rotation_axis', 'default'] 

for config in configs:
    for isBack in back_checks:
        low = 10; high = 80
        if isBack:
            low = 215; high = 220
        if systematic == 'aperture' or systematic == 'distance':
            for direction in range(-1, 2, 1): 
                write_shell_script(isBack, low, high, systematic, config, direction)
        else:
            write_shell_script(isBack, low, high, systematic, config, 0) # Default
            write_shell_script(isBack, low, high, systematic, config, 1) # Systematic on
       

