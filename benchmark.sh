#!/bin/bash

PATH=$PATH:~/bin

if [ ! -d "./Working-Directory" ]; then
	mkdir ./Working-Directory
fi

cp SampleSort Working-Directory

NP=(1 2 4 8 16)

for np in ${NP[@]}
do
	echo "Benchmarking SampleSort with $np threads"
	mpirun -wdir ./Working-Directory -np $np SampleSort
done
