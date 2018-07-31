	/*
 * Anderson Ibrahim
 * 
 * g++ OpenMPSample.cpp -o OpenMPSample.cpp.out -fopenmp -std=c++11 && ./OpenMPSample.cpp.out
 * 
 */
#include <omp.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


struct TaggedFrame {
	int tag;
	string frame;	
};

int main (int argc, char *argv[]) {
	
	vector<TaggedFrame> readQueue;
	
	cout << "Ola 1 - FORA DA REGIAO PARALELA ... " << endl;

	for (int i=0; i<102; i++) {
		readQueue.push_back({ .tag=i, .frame="original" });	  
	}

	#pragma omp parallel num_threads(4)
	{

		int id = omp_get_thread_num();
		int n_threads = omp_get_num_threads();
		//cout << "Sou a thread " << id << " de um total de " << n_threads << endl;

		string msg;
		msg = "Thread " + to_string(id) + " are processing frames: ";

		std::vector<TaggedFrame>::iterator it;
		for (it=readQueue.begin(); it!=readQueue.end(); ++it) {
			if ((*it).tag % n_threads == id) {
				msg = msg + to_string((*it).tag) + " ";
			}
		}
		
		msg = msg + "\n";
		
		cout << msg;
	}

	cout << "Ola 2 - FORA DA REGIAO PARALELA ... " << endl;

	return 0;

}
