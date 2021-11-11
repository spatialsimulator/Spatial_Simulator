# Spatial SBML Simulator with HDF5+OpenCV
This program is a PDE simulator for SBML models.
By using this program, users may simulate their spatially extended SBML models.
There is also an [image that runs on docker](https://github.com/funasoul/docker-spatialsim), so please refer to that as well.

## How to Compile
### Dependencies
+ [libsbml 5.17.0](http://sbml.org/Software/libSBML "libsbml")
+ [HDF5](https://support.hdfgroup.org/HDF5/ "HDF5")
+ [OpenCV 3.4](http://opencv.org/ "OpenCV")

## How to Use

### Build Spatial Simulator
```sh
% git clone /path/to/spatial_simulator
% cd Spatial_Simulator
% make
```

### Run
```sh
% ./spatialsimulator -t 0.1 -d 0.001 -o 10 /path/to/sampledfield_model.xml
% ./spatialsimulator -x 100 -y 100 -z 100 -m 0.1 -t 10 -d 0.001 -o 500 -c 0 -C 5 -s z30 /path/to/analytic_model.xml
```

Simulation results will be generated under `result/${model_name}` directory.

 Options     | Definitions
:------------|:------------
-h           | display usage
-x #(int)    | the number of points at x coordinate (for analytic geometry only) (ex. -x 200 (default:100))
-y #(int)    | the number of points at y coordinate (for analytic geometry only) (ex. -y 200 (default:100))
-z #(int)    | the number of points at z coordinate (for analytic geometry only) (ex. -z 200 (default:100))
-m #(double) | mesh size between {x,y,z} coordinate (ex. -m 0.15)
-t #(double) | simulation time (ex. -t 10) (default:1.0)
-d #(double) | delta t (ex. -d 0.1) (default:0.01)
-o #(int)    | output results every # steps (ex. -o 10) (default:1)
-c #(double) | min of color bar range (ex. -c 1) (default:0.0)
-C #(double) | max of color bar range (ex. -C 10) (default:Max value of InitialConcentration or InitialAmount)
-s char#(int)| select which dimension {x,y,z} and the number of slice to output (only 3D) (ex. -s z10 which means xy plane where z = 10)
-O outDir    | path to output directory (default:"result")
model.xml    | Target SBML Model


## License
This software is released under the MIT License, see [LICENSE.txt](./LICENSE.txt).
