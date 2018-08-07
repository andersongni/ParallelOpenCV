/*
 * Anderson Ibrahim
 * 
 * g++ Stitching.cpp -o Stitching.cpp.out -std=c++0x `pkg-config --cflags --libs opencv`
 * 
 */
#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

using namespace std;
using namespace cv;

bool try_use_gpu = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;

int main(int argc, char* argv[])
{

	string images [] = {"S1.jpg", 
						"S2.jpg", 
						"S3.jpg", 
						 
						"S5.jpg", 
						"S6.jpg", };

	int size = sizeof(images)/sizeof(*images);

	for (int i=0; i<size; i++) {
		cout << "Reading " << images[i] << endl;
		Mat img = imread(images[i]);
		if (img.empty())
		{
			cout << "Can't read image '" << images[i] << "'" << endl;
			return -1;
		}  
		imgs.push_back(img);
	}
	
	Mat pano;
	Ptr<Stitcher> stitcher = Stitcher::create(mode, try_use_gpu);
	Stitcher::Status status = stitcher->stitch(imgs, pano);
	
	cout << status << endl;

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return -1;
	}

	imwrite("output.jpg", pano);
	cout << "Stitching completed successfully" << endl;
	return 0;
}
