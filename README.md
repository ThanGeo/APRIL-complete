# APRIL - Complete Environment

## Features
- Queries:
- - Intersection Join
- - Within Join
- - SOON: Topology relations
- Rasterizer:
- - Full support to create APRIL and store on disk
- MBR Filter:
- - Implementation of Two-Layer filter. APRIL filter is always after the MBR Filter in the pipeline


## Build
In main directory, run ```source ./build.sh``` or 
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd build
make
```
Inside build/ directory, the executable 'main' is program executable.

## Run
- -c : creates the APRIL approximations for the input datasets.
- -R "keyword" : sets as the R input dataset, the dataset indicated by the keyword (see configuration files)
- -S "keyword" : sets as the S input dataset, the dataset indicated by the keyword (see configuration files)
- -q "query type" : sets the query type. Query types include:
- - disjoint : r and s are disjoint
- - intersect : r and s intersect
- - inside : r is inside s
- - contains : r contains s
- - covered_by : r is covered by s
- - covers : r covers s
- - meet : r and s meet (touch)
- - equal : r and s are geometrically equal
- - find_relation : detect all relations between r and s


## Configuration Files
### config.ini
Includes the system parameters

### datasets.ini
Includes the datasets parameters. 
To run on new datasets, first an entry must be added to the datasets.ini configuration file.
Use a unique keyword for the section of the dataset's parameters.

Then, input the new dataset in main in the following way:
```
./main -q <query_type> -R <my R dataset keyword> -S <my S dataset keyword>
```
Datasets must have an offset map specified for faster refinement (TODO: add generation of offset map)