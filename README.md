Spatial Simulator
======================

# Spatial SBML Simulator with HDF5+OpenCV
This program is a PDE simulator for SBML models.
By using this program, users may simulate their spatially extended SBML models.

## How to Compile ##
### Dependencies ###
+ [libsbml 5.13.0](http://sbml.org/Software/libSBML "libsbml")
+ [HDF5](https://support.hdfgroup.org/HDF5/ "HDF5")
+ [OpenCV 3.2](http://opencv.org/ "OpenCV")

## How to Use ##

### Build Spatial Simulator ###

    % git clone /path/to/spatial_simulator
    % cd Spatial_Simulator
    % make

### Run ###

    % ./spatialsimulator  -x 100 -y 100 -z 100 -t 10 -d 0.001 -o 500 -c 0 -C 5 -s z30 /path/to/model.xml

| Options | Definitions|
|:--------:|:------------|
|-x | Mesh size for x dimension (only for analytic geometry)|
|-y | Mesh size for y dimension (only for analytic geometry)|
|-z | Mesh size for z dimension (only for analytic geometry)|
|-t | End time|
|-d | Step size|
|-o | Number of steps per output|
|-C | Max Value for color bar|
|-c | Min Value for color bar|
|-s | Select which dimension and slice (e.g. z30 means xy plane where z = 30)|
|model.xml | Target SBML Model|


## License ##
This software is released under the MIT License, see [LICENSE.txt](./LICENSE.txt).
