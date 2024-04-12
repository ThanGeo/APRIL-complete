#! /bin/bash

./build.sh

mkdir -p experiment_results
output_file="experiment_results/all_datasets.txt" 
build_output_file="../${output_file}" 
echo -e "output \n" > $output_file

cd build && ./main -q find_relation -m FIND_RELATION -f APRIL -R T1NA -S T2NA >> $build_output_file

#cd build && ./main -q find_relation -m FIND_RELATION -f APRIL -R T1NA -S T3NA

# cd build && ./main -q find_relation -m FIND_RELATION -f APRIL -R O5EU -S O6EU > output_file