#/bin/bash

script_name=$1
g++ $script_name -o $script_name.out `pkg-config --cflags --libs opencv` 
chmod +x $script_name
./$script_name
