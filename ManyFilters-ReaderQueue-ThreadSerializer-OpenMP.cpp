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
#include <chrono>

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
	if (argc < 2) {
		cout << "This code requires two parametes: <number of threads workers> <number to filter complexity>" << endl;
		return 1;
	}
	
	int workers = atoi(argv[1]);
	int complexity = atoi(argv[2]);
	
	cv::setNumThreads(1);
	
	chrono::steady_clock::time_point start_main = chrono::steady_clock::now();   // get time now
	chrono::steady_clock::time_point start_read = chrono::steady_clock::now();   // get time now
	
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
	
	cout << "fourcc:" << fourcc << " fps:" << fps << " size:(" << width << "x" << height << ")" << endl;
	
	std::thread threadSerializer(serialize);	
	threadSerializer.detach();
	
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
		
	chrono::steady_clock::time_point end_read = chrono::steady_clock::now();   // get time now
	chrono::steady_clock::duration duration_read = end_read - start_read;
	cout << "Time to read:\t" << chrono::duration_cast<chrono::milliseconds>(duration_read).count() 
		<< " milliseconds" << endl;
	
	chrono::steady_clock::time_point start_filter = chrono::steady_clock::now();   // get time now
	
	#pragma omp parallel num_threads(workers)
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

				for (int i=0; i<complexity; i++) {
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
		
	chrono::steady_clock::time_point end_filter = chrono::steady_clock::now();   // get time now
	chrono::steady_clock::duration duration_filter = end_filter - start_filter;
	cout << "Time to filter:\t" << chrono::duration_cast<chrono::milliseconds>(duration_filter).count() 
		<< " milliseconds" << endl;

	while (!writeQueue.empty()) {
		std::this_thread::sleep_for(chrono::milliseconds(100));	
	}
	
	chrono::steady_clock::time_point end_main = chrono::steady_clock::now();   // get time now
	chrono::steady_clock::duration duration_main = end_main - start_main;
	cout << "Total time:\t" << chrono::duration_cast<chrono::milliseconds>(duration_main).count() 
		<< " milliseconds" << endl;
	
	return 0;
}
