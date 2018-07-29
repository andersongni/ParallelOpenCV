/*
 * Anderson Ibrahim
 * 
 * g++ -pthread ThreadSample.cpp -o ThreadSample.cpp.out -std=c++0x && ./ThreadSample.cpp.out
 * 
 */ 
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>

using namespace std;

struct taggedFrame {
	int tag;
	string image;	
};

std::vector<taggedFrame> buffer;
std::vector<taggedFrame>::iterator it;

std::mutex mu;
std::condition_variable cond;

bool frameCompare(const taggedFrame& first, const taggedFrame& second) {
	return first.tag < second.tag;
}

void serialize () {
	
	int next = 0;

	while (true) {
		std::unique_lock<mutex> locker(mu);
		cond.wait(locker);
	
		sort(buffer.begin(), buffer.end(), frameCompare);
		
		while (next == buffer.front().tag) {
			cout << "Writing frame " << buffer.front().tag << " " << buffer.front().image << endl;
			buffer.erase(buffer.begin());
			next++;
			
		}
		
		std::cout << "buffer contains:";
		for (it=buffer.begin(); it!=buffer.end(); ++it) {
			std::cout << ' ' << (*it).tag;
		}
		std::cout << '\n';
		
		locker.unlock();
	}
}

int main (int argc, char * argv[]) {
	std::thread threadSerializer(serialize);	
	threadSerializer.detach();
	
	taggedFrame elements[] = { { .tag = 2 },
							 { .tag = 1 },
							 { .tag = 0 },
							 { .tag = 5 },
							 { .tag = 4 },
							 { .tag = 8 },
							 { .tag = 3 },
							 { .tag = 6 },
							 { .tag = 7 },
							 { .tag = 9 } };
	
	int size = sizeof(elements)/sizeof(*elements);
	
	for (int i=0; i< size; i++) {
		std::this_thread::sleep_for(chrono::milliseconds(500));

		std::unique_lock<mutex> locker(mu);

		cout << "appending element " << elements[i].tag << endl;
		buffer.push_back(elements[i]);

		locker.unlock();
		cond.notify_one();
	}
	
	while (!buffer.empty()) {
		std::this_thread::sleep_for(chrono::milliseconds(100));	
	}

	return 0;
}
