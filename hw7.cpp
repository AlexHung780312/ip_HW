// author: Alex Hung
// Local binary coding
#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
using namespace cv;
using namespace std;
void Img2LBC_4b(const Mat& img, Mat& lbc) {
  for (int r = 1; r < img.rows - 1; r++) {
    for (int c = 1; c < img.cols - 1; c++) {
      if (img.at<uchar>(r - 1, c) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 8u;
      if (img.at<uchar>(r, c + 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 4u;
      if (img.at<uchar>(r + 1, c) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 2u;
      if (img.at<uchar>(r, c - 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 1u;
    }
  }
  lbc *= 17;  //因為原本值域只有[0~15]
}
void Img2LBC_8b(const Mat& img, Mat& lbc) {
  for (int r = 1; r < img.rows - 1; r++) {
    for (int c = 1; c < img.cols - 1; c++) {
      if (img.at<uchar>(r - 1, c) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 128u;
      if (img.at<uchar>(r - 1, c + 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r - 1, c + 1) |= 64u;
      if (img.at<uchar>(r, c + 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 32u;
      if (img.at<uchar>(r + 1, c + 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 16u;
      if (img.at<uchar>(r + 1, c) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 8u;
      if (img.at<uchar>(r + 1, c - 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 4u;
      if (img.at<uchar>(r, c - 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 2u;
      if (img.at<uchar>(r - 1, c - 1) > img.at<uchar>(r, c))
        lbc.at<uchar>(r, c) |= 1u;
    }
  }
}

inline double MaxRatio(const double& v1, const double& v2) {
  return max((double)v1 / (double)v2, (double)v2 / (double)v1);
}

inline double Ratio(const double& v1, const double& v2) {
  return (double)v1 / (double)v2;
}
inline void Convert_64FC1_to_8UC1(const Mat& src, Mat& tgt) {
  double min, max;
  minMaxLoc(src, &min, &max);
  src -= min;
  src /= (max - min);
  src *= 255.0;
  src.convertTo(tgt, CV_8UC1);
}
void Img2MaxRatioImg(const Mat& img, Mat& ratio) {
  Mat tmp = Mat::zeros(img.rows - 2, img.cols - 2, CV_64FC1);
  for (int r = 1; r < img.rows - 1; r++) {
    for (int c = 1; c < img.cols - 1; c++) {
      tmp.at<double>(r - 1, c - 1) =
          0.25 * (MaxRatio(img.at<uchar>(r, c), img.at<uchar>(r + 1, c)) +
                  MaxRatio(img.at<uchar>(r, c), img.at<uchar>(r - 1, c)) +
                  MaxRatio(img.at<uchar>(r, c), img.at<uchar>(r, c + 1)) +
                  MaxRatio(img.at<uchar>(r, c), img.at<uchar>(r, c - 1)));
    }
  }
  Convert_64FC1_to_8UC1(tmp, ratio);
}
void Img2RatioImg(const Mat& img, Mat& ratio) {
  Mat tmp = Mat::zeros(img.rows - 2, img.cols - 2, CV_64FC1);
  for (int r = 1; r < img.rows - 1; r++) {
    for (int c = 1; c < img.cols - 1; c++) {
      tmp.at<double>(r - 1, c - 1) =
          0.25 * (Ratio(img.at<uchar>(r, c), img.at<uchar>(r + 1, c)) +
                  Ratio(img.at<uchar>(r, c), img.at<uchar>(r - 1, c)) +
                  Ratio(img.at<uchar>(r, c), img.at<uchar>(r, c + 1)) +
                  Ratio(img.at<uchar>(r, c), img.at<uchar>(r, c - 1)));
    }
  }
  Convert_64FC1_to_8UC1(tmp, ratio);
}

void ApplyFloor1(Mat& m) {
  for (int r = 0; r < m.rows; r++) {
    for (int c = 0; c < m.cols; c++) {
      if (m.at<uchar>(r, c) == 0) {
        m.at<uchar>(r, c) = 1;
      }
    }
  }
}
int main(int argc, char* argv[]) {
  if (argc < 3) {
    cerr << "usage: ./hw7 img.tiff(grayscale) out-dir\n";
    exit(1);
  }
  try {
    string odir(argv[2]);
    Mat img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    ApplyFloor1(img);
    Mat lbc_img_4 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat lbc_img_8 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat ratio_img;
    Mat max_ratio_img;
    Img2LBC_4b(img, lbc_img_4);
    Img2LBC_8b(img, lbc_img_8);
    Img2RatioImg(img, ratio_img);
    Img2MaxRatioImg(img, max_ratio_img);
#ifdef SHOW_IMG_WINDOW
    namedWindow("4-bit LBC image",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("4-bit LBC image", lbc_img_4);
    namedWindow("8-bit LBC image",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("8-bit LBC image", lbc_img_8);
    namedWindow("Ratio image(Max)",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Ratio image(Max)", max_ratio_img);
    namedWindow("Ratio image",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Ratio image", ratio_img);
#endif
    imwrite(odir + "/lbc.4.tiff", lbc_img_4);
    imwrite(odir + "/lbc.8.tiff", lbc_img_8);
    imwrite(odir + "/ratio.tiff", ratio_img);
    imwrite(odir + "/ratio.max.tiff", max_ratio_img);
    waitKey(0);
  } catch (exception& e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
  return 0;
}
