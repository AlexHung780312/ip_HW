// author: Alex Hung
#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
using namespace cv;
using namespace std;

void RegularSampling(const Mat& src, Mat& tgt) {
  assert(src.rows == tgt.rows * 2);
  assert(src.cols == tgt.cols * 2);
  for (int i = 0; i < tgt.rows; i++) {
    for (int j = 0; j < tgt.cols; j++) {
      //雙內差
      // tgt.at<uchar>(i, j) = src.at<uchar>(i * 2, j * 2);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "usage: ./hw2 input-img output-regular-downsample-img "
            "output-adaptive-downsample-img\n"
         << "eg: ./hw2 img/lena512bw.tiff img/lena256bw.regular.tiff  "
         << "img/lena256bw.adaptive.tiff\n";
    exit(1);
  }
  try {
    Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat regular_ds_img(img.cols / 2, img.rows / 2, CV_8UC1);
    RegularSampling(img, regular_ds_img);
    imwrite(argv[2], regular_ds_img);
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
