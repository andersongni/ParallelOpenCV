#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

struct taggedFrame {
	int tag;
	Mat frame;	
};

std::vector<taggedFrame> buffer;
std::vector<taggedFrame>::iterator it;

std::mutex mu;
std::condition_variable cond;

bool frameCompare(const taggedFrame& first, const taggedFrame& second) {
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
			cout << "Writing frame " << buffer.front().tag << endl;
			video.write(buffer.front().frame);
			
			buffer.erase(buffer.begin());
			next++;
		}
		
		std::cout << "buffer contains:";
		for (it=buffer.begin(); it!=buffer.end(); ++it) {
			std::cout << " " << (*it).tag;
		}
		std::cout << '\n';
		
		locker.unlock();
	}
}

int main (int argc, char * argv[]) {	
	cv::setNumThreads(1);
	
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
	
	std::thread threadSerializer(serialize);	
	threadSerializer.detach();
	
	clock_t begin = clock();   // get time now
	
	taggedFrame current;
	int index = 0;
	
	while(true){

		Mat frame;
		vcap >> frame;

		if (frame.empty()) {
		   break;
		}

		for (int i=0; i<50; i++) {
			GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
			cvtColor(frame, frame, CV_BGR2GRAY);
			Canny(frame, frame, 0, 30, 3);
			cvtColor(frame, frame, CV_GRAY2BGR); //Converting frame to tree channels again
		}

		current.tag=index;
		current.frame=frame;

		std::unique_lock<mutex> locker(mu);
	
		cout << "appending element " << current.tag << endl;
		buffer.push_back(current);
		
		locker.unlock();
		cond.notify_one();
		
		index++;
	}

	clock_t end = clock();   // get time now
	
	double elapsed = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed << " seconds" << endl;
	
	while (!buffer.empty()) {
		std::this_thread::sleep_for(chrono::milliseconds(100));	
	}

	return 0;
}
