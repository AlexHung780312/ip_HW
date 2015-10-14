#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
using namespace cv;
using namespace std;
int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "usage: ./hw1 input-img output-color-img output-greylevel-img\n"
         << "eg: ./hw1 img/lena512color.tiff img/lena512color.jpg "
         << "img/lena512bw.tiff\n";
    exit(1);
  }
  try {
    Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    imwrite(argv[2], img);
    Mat gray_img;
    cvtColor(img, gray_img, CV_BGR2GRAY);
    imwrite(argv[3], gray_img);
    namedWindow("Input image", CV_WINDOW_AUTOSIZE);
    namedWindow("Gray image", CV_WINDOW_AUTOSIZE);
    imshow("Input image", img);
    imshow("Gray image", gray_img);
    waitKey(0);
  } catch (exception e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
  return 0;
}
