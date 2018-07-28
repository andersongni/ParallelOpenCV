#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>

using namespace std;

std::vector<int> buffer;
std::vector<int>::iterator it;

std::mutex mu;
std::condition_variable cond;

bool compare(const int& first, const int& second) {
	return first < second;
}

void serialize () {
	
	int next = 1;
	int frame;

	while (true) {
		std::unique_lock<mutex> locker(mu);
		cond.wait(locker);
	
		sort(buffer.begin(), buffer.end(), compare);
		
		while (next == buffer.front()) {
			cout << "Writing frame " << buffer.front() << endl;
			//~ buffer.pop_front();
			buffer.erase(buffer.begin());
			next++;
		}
		
		std::cout << "buffer contains:";
		for (it=buffer.begin(); it!=buffer.end(); ++it) {
			std::cout << ' ' << *it;
		}
		std::cout << '\n';
		
		locker.unlock();
	}
}

int main (int argc, char * argv[]) {
	std::thread threadSerializer(serialize);	
	threadSerializer.detach();
	
	int elements[] = {2,3,5,4,8,1,6,7,9};
	
	int size = sizeof(elements)/sizeof(*elements);
	
	for (int i=0; i< size; i++) {
		
		std::this_thread::sleep_for(chrono::milliseconds(500));	
		
		std::unique_lock<mutex> locker(mu);
		
		cout << "appending element " << elements[i] << endl;
		buffer.push_back(elements[i]);
		
		locker.unlock();
		cond.notify_one();
		
	}
	
	while (!buffer.empty()) {
		std::this_thread::sleep_for(chrono::milliseconds(100));	
	}

	return 0;
}
