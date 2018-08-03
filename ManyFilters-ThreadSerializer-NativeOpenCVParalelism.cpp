/*
 * Anderson Ibrahim
 * 
 * g++ -pthread ManyFilters-ThreadSerializer-NativeOpenCVParalelism.cpp -o ManyFilters-ThreadSerializer-NativeOpenCVParalelism.cpp.out -std=c++0x `pkg-config --cflags --libs opencv`
 * 
 */ 
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct TaggedFrame {
	int tag;
	Mat frame;	
};

std::vector<TaggedFrame> buffer;
std::vector<TaggedFrame>::iterator it;

std::mutex mu;
std::condition_variable cond;

bool frameCompare(const TaggedFrame& first, const TaggedFrame& second) {
	return first.tag < second.tag;
}

int  fourcc;
int  fps;
int  width;
int  height;
bool isColor;

void serialize () {
	
	int next = 0;	
	VideoWriter video("output.avi",fourcc, fps, Size(width,height), isColor);

	while (true) {
		std::unique_lock<mutex> locker(mu);
		cond.wait(locker);
	
		sort(buffer.begin(), buffer.end(), frameCompare);
		
		while (next == buffer.front().tag) {
			//~ cout << "Writing frame " << buffer.front().tag << endl;
			video.write(buffer.front().frame);
			
			buffer.erase(buffer.begin());
			next++;
		}
		
		//~ std::cout << "buffer contains:";
		//~ for (it=buffer.begin(); it!=buffer.end(); ++it) {
			//~ std::cout << " " << (*it).tag;
		//~ }
		//~ std::cout << '\n';
		
		locker.unlock();
	}
}

int main (int argc, char * argv[]) {	
	
	chrono::steady_clock::time_point start_main = chrono::steady_clock::now();   // get time now
	
	if (argc < 2) {
		cout << "This code requires two parametes: <number of threads workers> <number to filter complexity>" << endl;
		return 1;
	}
	
	int complexity = atoi(argv[1]);
	int workers = atoi(argv[2]);
	
	cv::setNumThreads(workers);	
	
	String filename_in = "walking.avi";
	VideoCapture vcap(filename_in); 

	if(!vcap.isOpened()){
		 cout << "Error opening video stream or file" << endl;
		 return -1;
	}

	fourcc  = vcap.get(CV_CAP_PROP_FOURCC);
	fps     = vcap.get(CV_CAP_PROP_FPS);
	width   = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	height  = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	isColor = true;
	
	//~ cout << "fourcc:" << fourcc << " fps:" << fps << " size:(" << width << "x" << height << ")" << endl;
	
	std::thread threadSerializer(serialize);	
	threadSerializer.detach();
	
	clock_t begin = clock();   // get time now
	
	TaggedFrame currentFrame;
	int index = 0;
	
	while(true){

		Mat frame;
		vcap >> frame;

		if (frame.empty()) {
			break;
		}

		if (index % 15 == 0) {
			//~ cout << "Sleeping for frame.tag=" << index << endl;
			std::this_thread::sleep_for(chrono::milliseconds(1000));
		}

		for (int i=0; i<complexity; i++) {
			GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
			cvtColor(frame, frame, CV_BGR2GRAY);
			Canny(frame, frame, 0, 30, 3);
			cvtColor(frame, frame, CV_GRAY2BGR); //Converting frame to tree channels again
		}

		currentFrame.tag=index;
		currentFrame.frame=frame;

		std::unique_lock<mutex> locker(mu);
	
		//~ cout << "appending element " << currentFrame.tag << endl;
		buffer.push_back(currentFrame);
		
		locker.unlock();
		cond.notify_one();
		
		index++;
	}
	
	while (!buffer.empty()) {
		std::this_thread::sleep_for(chrono::milliseconds(100));	
	}
	
	chrono::steady_clock::time_point end_main = chrono::steady_clock::now();   // get time now
	chrono::steady_clock::duration duration_main = end_main - start_main;
	cout << chrono::duration_cast<chrono::milliseconds>(duration_main).count() << endl;

	return 0;
}
