#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char** argv) {
	
    String filename = "walking.avi";
    VideoCapture capture(filename);
    Mat frame;

    if( !capture.isOpened() )
        throw "Error when reading steam_avi";

	//int fps = capture.get(cv2.CAP_PROP_FPS)

    for( ; ; ) {
		capture >> frame;
		if (!(frame.size()>0)) {
			break;
		}
        imshow("Janela", frame);
        waitKey(20); // waits to display frame
    }
    waitKey(0);

}
