/*
 * Anderson Ibrahim
 * 
 * g++ -pthread ManyFilters-ReaderQueue-ThreadSerializer-OpenMP.cpp -o ManyFilters-ReaderQueue-ThreadSerializer-OpenMP.cpp.out -fopenmp -std=c++0x `pkg-config --cflags --libs opencv`
 * 
 */ 
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include "opencv2/opencv.hpp"
#include <omp.h>
#include <stdio.h>

using namespace std;
using namespace cv;

struct TaggedFrame {
	int tag;
	Mat frame;	
};

std::vector<TaggedFrame> writeQueue;
std::vector<TaggedFrame> readQueue;

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
		std::vector<TaggedFrame>::iterator it;
		std::unique_lock<mutex> locker(mu);
		cond.wait(locker);
	
		sort(writeQueue.begin(), writeQueue.end(), frameCompare);
		
		while (next == writeQueue.front().tag) {
			//~ cout << "Writing frame " << writeQueue.front().tag << endl;
			video.write(writeQueue.front().frame);
			
			writeQueue.erase(writeQueue.begin());
			next++;
		}
		
		//~ std::cout << "writeQueue contains:";
		//~ for (it=writeQueue.begin(); it!=writeQueue.end(); ++it) {
			//~ std::cout << " " << (*it).tag;
		//~ }
		//~ std::cout << '\n';
		
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
	
	TaggedFrame currentFrame;
	int index = 0;
	
	while(true){

		Mat frame;
		vcap >> frame;

		if (frame.empty()) {
		   break;
		}
		
		//~ cout << "ReadQueue: appending tag " << index << endl;
		readQueue.push_back({ .tag=index, .frame=frame });
		
		index++;
		
	}
	
	#pragma omp parallel num_threads(4)
	{
		int id = omp_get_thread_num();
		int n_threads = omp_get_num_threads();
				
		//~ string msg;
		//~ msg = "Thread " + to_string(id) + " are processing frames: ";
		
		std::vector<TaggedFrame>::iterator it;
		for (it=readQueue.begin(); it!=readQueue.end(); ++it) {
			if ((*it).tag % n_threads == id) {
				//~ cout << "(*it).tag=" << (*it).tag << " % n_threads=" << n_threads << " == " "id=" << id << endl;
				Mat frame = (*it).frame;

				//~ msg = msg + to_string((*it).tag) + " ";

				for (int i=0; i<10; i++) {
					GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
					cvtColor(frame, frame, CV_BGR2GRAY);
					Canny(frame, frame, 0, 30, 3);
					cvtColor(frame, frame, CV_GRAY2BGR); //Converting frame to tree channels again
				}

				std::unique_lock<mutex> locker(mu);
			
				//~ cout << "WriteQueue: Thread " << id << " appending tag " << (*it).tag << endl;
				writeQueue.push_back({ .tag=(*it).tag, .frame=frame });
				
				locker.unlock();
				cond.notify_one();	
			}
		}
		
		//~ msg = msg + "\n";
		//~ cout << msg;
	}

	clock_t end = clock();   // get time now
	
	double elapsed = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed << " seconds" << endl;
	
	while (!writeQueue.empty()) {
		std::this_thread::sleep_for(chrono::milliseconds(100));	
	}

	return 0;
}
