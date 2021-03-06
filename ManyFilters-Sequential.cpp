/*
 * Anderson Ibrahim
 * 
 * g++ -pthread ManyFilters-Sequential.cpp -o ManyFilters-Sequential.cpp.out -std=c++0x `pkg-config --cflags --libs opencv`
 * 
 */

#include "opencv2/opencv.hpp"
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;

int main(int argc, char * argv[]) {
	
	chrono::steady_clock::time_point start_main = chrono::steady_clock::now();   // get time now
	
	if (argc < 2) {
		cout << "This code requires one parametes: <number to filter complexity>" << endl;
		return 1;
	}
	
	int complexity = atoi(argv[1]);
	
	cv::setNumThreads(1);

	String filename_in = "walking.avi";
	VideoCapture vcap(filename_in); 

	if(!vcap.isOpened()){
		 cout << "Error opening video stream or file" << endl;
		 return -1;
	}

	int  fourcc  = vcap.get(CV_CAP_PROP_FOURCC);
	int  fps     = vcap.get(CV_CAP_PROP_FPS);
	int  width   = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int  height  = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	bool isColor = true;
	
	//~ cout << "fourcc:" << fourcc << " fps:" << fps << " size:(" << width << "x" << height << ")" << endl;

	VideoWriter video("output.avi",fourcc, fps, Size(width,height), isColor);

	clock_t begin = clock();   // get time now
	
	while(true){

		Mat frame;
		vcap >> frame;

		//~ cout << "reading frame" << endl;
		if (frame.empty()) {
		   //~ cout << "stoping loop" << endl;
		   break;
		}

		for (int i=0; i<complexity; i++) {
			GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
			cvtColor(frame, frame, CV_BGR2GRAY);
			Canny(frame, frame, 0, 30, 3);
			cvtColor(frame, frame, CV_GRAY2BGR); //Converting frame to tree channels again
		}

		video.write(frame);

		//~ imshow( "Frame", frame );
		//~ char c = (char)waitKey(33);
		//~ if( c == 27 ) break;

	}
	
	chrono::steady_clock::time_point end_main = chrono::steady_clock::now();   // get time now
	chrono::steady_clock::duration duration_main = end_main - start_main;
	cout << chrono::duration_cast<chrono::milliseconds>(duration_main).count() << endl;

	return 0;
}
