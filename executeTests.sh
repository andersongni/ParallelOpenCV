for script in $(ls -1 *.cpp); 
do 
	echo "Executing $script"; 
	for i in {1..5}; 
	do 
		printf "$i " ; 
		g++ $script -o $script.out `pkg-config --cflags --libs opencv` && ./$script.out ;  
	done; 
done
