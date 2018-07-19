#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int, char**)
{
    String filename_in = "walking.avi";
    VideoCapture inputVideo(filename_in);
    
    if(!inputVideo.isOpened()) { // check if we succeeded
        cout  << "Could not open the input video: " << filename_in << endl;
        return -1;
	}
    
    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
    int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
    // Transform from int to char via Bitwise operators
    char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
    
    VideoWriter outputVideo; 
    String filename_out = "output.avi";
    
    outputVideo.open(filename_out, ex, inputVideo.get(CV_CAP_PROP_FPS), S, true);
    
    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << filename_out << endl;
        return -1;
    }
    
    int n_frames=inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
    
    cout << "Input frame resolution: Width=" << S.width << "  Height=" << S.height
         << " of nr#: " << n_frames << endl;         
    cout << "Input codec type: " << EXT << endl;
    
    Mat frame, res;
    vector<Mat> spl;
    
    int channel = 2;
    
    /*DEBUG*/ cout << "Starting loop" << endl;
    for(int i_frame=1; i_frame<=n_frames; i_frame++) {

		
		/*DEBUG*/ cout << "Reading frame " << i_frame << endl;
        inputVideo >> frame; // get a new frame from camera
        
        cvtColor(frame, frame, CV_BGR2GRAY);
        GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);
        Canny(frame, frame, 0, 30, 3);
        
		/*DEBUG*/ cout << "Starting split logic " << endl;
        split(frame, spl);                // process - extract only the correct channel
		/*DEBUG*/ cout << "Frame splited " << endl;
        for (int i =0; i < 3; ++i){
			/*DEBUG*/ cout << "For channel " << i << endl;
            if (i != channel) {
				/*DEBUG*/ cout << "Changing channel " << i << endl;
                spl[i] = Mat::zeros(S, spl[0].type());
			}
		}
        
		/*DEBUG*/ cout << "Merging channels " << endl;        
        merge(spl, res);

		/*DEBUG*/ cout << "Writing res " << endl;
		
        //outputVideo.write(res); //save or
        outputVideo << res;
        
        
		/*DEBUG*/ cout << "Final of loop " << endl;
        
        /*
        imshow("edges", edges);
        
        if(waitKey(30) >= 0) {
			break;
		}*/
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
