# APRIL - Complete Environment

## Features
- Queries:
- - Spatial Join with predicate, available predicates:
- - intersect
- - disjoint
- - inside
- - contains
- - covered by
- - covers
- - meet
- - equal
- - find relation
- Rasterizer:
- - Full support to create APRIL and store on disk
- MBR Filter:
- - Two-Layer filter (D. Tsitsigkos et al.). APRIL filter is always after the MBR Filter in the pipeline
- - Optimized Two-Layer filter for "find relation" and "relate with predicate" queries
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
- -s "setting" : sets the pipeline setting. Options include (ST2, ST3, OP2, OP3 and some development ones. For more, see topology relations paper.)
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
If using APRIL, make sure to specify '-c' in the first run to create APRIL for the dataset.

Example run:
```
./main -q <query_type> -s OP3 -R <my R dataset keyword> -S <my S dataset keyword>
```
WARNING: Datasets must have an offset map specified for faster refinement (TODO: add generation of offset map, currently unsupported)