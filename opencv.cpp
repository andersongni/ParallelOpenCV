#include <opencv2/opencv.hpp>

using namespace cv;

int main (int argc, char *argv[]) {
  
  Mat testGray = imread("lena.png", CV_LOAD_IMAGE_GRAYSCALE);

  imwrite("output.jpg", testGray);
  return 0;

}
