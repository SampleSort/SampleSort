#!/bin/bash


if [ ! -d "./Working-Directory" ]; then
	mkdir ./Working-Directory
fi

mpirun -wdir ./Debug -np 8 "$@" SampleSort
