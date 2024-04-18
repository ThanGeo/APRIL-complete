# APRIL - Complete Environment

## Features
- Queries:
- - Intersection Join
- - Within Join
- - Topology relations (*find relation* and *relate* with specific topological relation queries)
- Rasterizer:
- - Full support to create APRIL and store on disk
- MBR Filter:
- - Two-Layer filter (D. Tsitsigkos et al.). APRIL filter is always after the MBR Filter in the pipeline
- - Optimized Two-Layer filter for "find topological relation" queries
- APRIL Features
- - Partitioning mechanism [1,32] for better performance
- - SOON: compression
- - SOON: different granularity support


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
- -m "keyword" : sets the "keyword" filter as the MBR filter, options:
- - MBR_STANDARD : standard MBR intersection filter (default)
- - MBR_FR : specialized MBR filter for find relation queries
- -f "keyword" : sets the "keyword" filter as the intermediate filter, options:
- - "" : no intermediate filter (default)
- - APRIL_STANDARD : standard APRIL intersection intermediate filter
- - APRIL_FR : specialized APRIL intermediate filter for find relation queries
- - APRIL_OTF : specialized APRIL intermediate filter for **find relation queries** that performs rasterization on the fly (no pre-processing)
- -p "num" : sets "num" partitions for the APRIL approximation
- -e : evaluates the query 10 times and prints the average times
- -v "file" : export CSV with the results to "file". If file exists, then append the output

## Configuration Files
### config.ini
Includes the system parameters

### datasets.ini
Includes the datasets parameters. 
To run on new datasets, first an entry must be added to the datasets.ini configuration file.
Use a unique keyword for the section of the dataset's parameters.

Then, input the new dataset in main in the following way:
```
./main -q <query_type> -f APRIL -R <my R dataset keyword> -S <my S dataset keyword>
```
Datasets must have an offset map specified for faster refinement (TODO: add generation of offset map)