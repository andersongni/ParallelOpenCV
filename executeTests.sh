#!/bin/bash

echo "complexity;execution_number;script;threads;time(milliseconds)"; 

testScript () {
	complexity=$1
	script=$2
	threads=$3
	
	printf "$complexity;$execution;$script;$threads;"; 
	g++ -pthread $script -o $script.out -fopenmp -std=c++0x `pkg-config --cflags --libs opencv`;
	./$script.out $complexity $threads;
	#echo ' '$script'.out '$complexity' '$threads
}

for index_complexity in {1..10}; 
do 
	complexity=$(($index_complexity*5));	
	for execution in {1..5}; 
	do 
		for script in $(ls -1 *.cpp); 
		do 
			if [[ $script != *"Sequential"* ]];
			then
				for threads in {4..20};
				do
					testScript $complexity $script $threads
				done;
			else
				testScript $complexity $script 1
			fi;
		done;
	done; 
done
