// author: Alex Hung
#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
using namespace cv;
using namespace std;
void bgr2gray(Mat& src, Mat& tgt) {
  /// Convert RGB to Grey-level
  /// I = (R+G+B)/3
  assert(src.rows == tgt.rows);
  assert(src.cols == tgt.cols);
  for (int i = 0; i < src.rows; i++) {    // for each rows
    for (int j = 0; j < src.cols; j++) {  // for each cols
      tgt.at<uchar>(j, i) =
          (src.at<Vec3b>(i, j)[0] + src.at<cv::Vec3b>(i, j)[1] +
           src.at<cv::Vec3b>(i, j)[2]) /
          3;
    }
  }
}
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
    Mat grey_img(img.cols, img.rows, CV_8UC1);
    ///// cvtColor(img, grey_img, CV_BGR2GRAY);
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
