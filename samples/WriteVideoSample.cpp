#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(){

   cout << "Starting application" << endl;

   String filename_in = "walking.avi";
   VideoCapture vcap(filename_in); 

   cout << "file readed" << endl;
    
   if(!vcap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
   }

   int  fourcc  = vcap.get(CV_CAP_PROP_FOURCC);
   int  fps     = vcap.get(CV_CAP_PROP_FPS);
   int  width   = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
   int  height  = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
   bool isColor = true;
   
   cout << "fourcc:" << fourcc << " fps:" << fps << " size:(" << width << "x" << height << ")" << endl;

   VideoWriter video("output.avi",fourcc, fps, Size(width,height), isColor);
   for(;;){
       cout << "Reading frame" << endl;
       Mat frame;
       vcap >> frame;
       
       cout << "reading frame" << endl;
       if (frame.empty()) {
           cout << "stoping loop" << endl;
	   break;
       }
       
       video.write(frame);
       
    }
  return 0;
}
