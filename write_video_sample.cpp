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

   for(;;){

       Mat frame;
       vcap >> frame;
       
       cout << "reading frame" << endl;
       if (frame.empty()) {
	       cout << "stoping loop" << endl;
		   break;
	   }
       
       video.write(frame);
       
/*       imshow( "Frame", frame );
       char c = (char)waitKey(33);
       if( c == 27 ) break;
       */
    }
  return 0;
}
