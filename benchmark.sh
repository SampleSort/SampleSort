#!/bin/bash

PATH=$PATH:~/bin

if [ ! -d "./Working-Directory" ]; then
	mkdir ./Working-Directory
fi

cp SampleSort Working-Directory

echo "Running SampleSort"

NP=(1, 2, 4, 8, 16)

for np in ${NP[@]} do
	mpirun -wdir ./Working-Directory -np $np SampleSort > benchmark_SS_np_${np}.txt
done
