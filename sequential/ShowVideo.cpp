#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
    String filename = "output.avi";
    VideoCapture cap(filename);
    
    if(!cap.isOpened()) { // check if we succeeded
        return -1;
	}

    Mat edges;
    namedWindow("edges",1);
    
    for(;;) {
		
        Mat frame;
        cap >> frame; // get a new frame from camera
        
        imshow("edges", edges);
        
        if(waitKey(30) >= 0) {
			break;
		}
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
