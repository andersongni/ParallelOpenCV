#!/bin/bash
n_executions=$1
n_threads=$2

echo "execution_number;script;complexity;threads;time(milliseconds)"; 

testScript () {	
	n_executions=$1
	complexity=$2
	script=$3
	threads=$4
	
	for execution in $(seq 1 $n_executions); 
	do 
		
		printf "$execution;$script;$complexity;$threads;"; 
		#~ g++ -pthread $script -o $script.out -fopenmp -std=c++0x `pkg-config --cflags --libs opencv`;
		./$script.out $complexity $threads;
		#~ echo ' '$script'.out '$complexity' '$threads
	done;
}

for complexity in {5..50..5}; 
do 
	for threads in $(seq 1 $n_threads); 
	do
		for script in $(ls -1 *.cpp); 
		do 
			if !([[ $script == *"Sequential"* ]] && [ $threads -gt 1 ]);
			then
				testScript $n_executions $complexity $script $threads
			fi;
		done;
	done;
done
