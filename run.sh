#!/bin/bash

PATH=$PATH:~/bin

if [ ! -d "./Working-Directory" ]; then
	mkdir ./Working-Directory
fi

cp SampleSort Working-Directory

echo "Running SampleSort"

mpirun -wdir ./Working-Directory -np 8 "$@" SampleSort
