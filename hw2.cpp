// author: Alex Hung
#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "usage: ./hw2 input-img output-gradiant-img output-downsample-img\n"
         << "eg: ./hw2 img/lena512bw.tiff img/lena512bw.gradiant.tiff  "
         << "img/lena256bw.tiff\n";
    exit(1);
  }
  try {
    Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    imwrite(argv[2], img);
    Mat city_block(img.cols, img.rows, CV_8UC1);

    bgr2gray(img, grey_img);
    imwrite(argv[3], grey_img);
#ifdef SHOW_IMG_WINDOW
    namedWindow("Input image", CV_WINDOW_AUTOSIZE);
    namedWindow("Gray image", CV_WINDOW_AUTOSIZE);
    imshow("Input image", img);
    imshow("Gray image", grey_img);
    waitKey(0);
#endif
  } catch (exception e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
  return 0;
}
