#!/bin/bash

PATH=$PATH:~/bin

if [ ! -d "./Working-Directory" ]; then
	mkdir ./Working-Directory
fi

cp TestPrefix Working-Directory

echo "Running TestPrefix"

mpirun -wdir ./Working-Directory -np 8 "$@" TestPrefix
