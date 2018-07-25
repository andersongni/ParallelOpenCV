#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(){

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

	VideoWriter video("output.avi",fourcc, fps, Size(width,height), isColor);
	
	clock_t begin = clock();   // get time now
	
	#pragma omp parallel num_threads(4)
	while(true){

		Mat frame;
		vcap >> frame;

		//~ cout << "reading frame" << endl;
		if (frame.empty()) {
		   //~ cout << "stoping loop" << endl;
		   break;
		}

		GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
		cvtColor(frame, frame, CV_BGR2GRAY);
		Canny(frame, frame, 0, 30, 3);
		cvtColor(frame, frame, CV_GRAY2BGR); //Converting frame to tree channels again

		video.write(frame);

		//~ imshow( "Frame", frame );
		//~ char c = (char)waitKey(33);
		//~ if( c == 27 ) break;

	}

	clock_t end = clock();   // get time now
	
	double elapsed = double(end - begin) / CLOCKS_PER_SEC;
	cout << elapsed << " seconds" << endl;

	return 0;
}
