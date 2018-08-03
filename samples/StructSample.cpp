#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct taggedFrame {
	int tag;
	string image;	
};

bool frameCompare(const taggedFrame& first, const taggedFrame& second) {
	return first.tag < second.tag;
}

int main (int argc, char * argv[]) {
	
	vector<taggedFrame> buffer;
	vector<taggedFrame>:: iterator it;
	
	taggedFrame frames[] = { { .tag = 1 },
					   { .tag = 3 },
					   { .tag = 2 } };
					   
	int size = sizeof(frames)/sizeof(*frames);
	
	for (int i=0; i< size; i++) {
		cout << "frame.tag=" << frames[i].tag << endl;
		buffer.push_back(frames[i]);
	}
	
	std::cout << "buffer contains:";
	for (it=buffer.begin(); it!=buffer.end(); ++it) {
		std::cout << ' ' << (*it).tag;
	}
	std::cout << '\n';
		
	
	sort(buffer.begin(), buffer.end(), frameCompare);
	
	
	std::cout << "buffer contains:";
	for (it=buffer.begin(); it!=buffer.end(); ++it) {
		std::cout << ' ' << (*it).tag;
	}
	std::cout << '\n';
	
	return 0;
}
