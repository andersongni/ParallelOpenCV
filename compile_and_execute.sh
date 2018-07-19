#/bin/bash

script_name=$1
g++ $script_name.cpp -o $script_name `pkg-config --cflags --libs opencv` 
./$script_name
