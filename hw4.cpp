#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
using namespace cv;
using namespace std;

void fillGaussianNoise(Mat& img, float mean, float stdev, mt19937& gen) {
  uniform_real_distribution<> dis(0, 1);
  for (int y = 0; y < img.rows; y += 2) {
    for (int x = 0; x < img.cols; x++) {
      float garma = dis(gen);
      float phi = dis(gen);
      float z1 = stdev * cos(M_2_PI * phi) * sqrt(-2. * log(garma));
      float z2 = stdev * sin(M_2_PI * phi) * sqrt(-2. * log(garma));
      unsigned char p1 = static_cast<unsigned char>(std::max(
          0.f, std::min(255.f,
                        static_cast<float>(img.at<uchar>(Point(y, x))) + z1)));
      unsigned char p2 = static_cast<unsigned char>(std::max(
          0.f,
          std::min(255.f,
                   static_cast<float>(img.at<uchar>(Point(y + 1, x))) + z2)));
      img.at<uchar>(Point(y, x)) = p1;
      img.at<uchar>(Point(y + 1, x)) = p2;
    }
  }
}
int main(int argc, char* argv[]) {
  try {
    random_device rd;
    mt19937 gen(rd());
    // gray img
    Mat gray_img(256, 256, CV_8UC1, Scalar(127));
    /// add gaussian noise algo2.3
    Mat noise_algo23_v10_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoise(noise_algo23_v10_img, 0, 10, gen);
    Mat noise_algo23_v15_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoise(noise_algo23_v15_img, 0, 15, gen);

#ifdef SHOW_IMG_WINDOW
    namedWindow("Gray image (i=127)",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Gray image (i=127)", gray_img);
    moveWindow("Gray image (i=127)", 0, 100);
    namedWindow("Algo2.3, sigma=10",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Algo2.3, sigma=10", noise_algo23_v10_img);
    moveWindow("Algo2.3, sigma=10", 300, 100);
    namedWindow("Algo2.3, sigma=15",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Algo2.3, sigma=15", noise_algo23_v15_img);
    moveWindow("Algo2.3, sigma=15", 600, 100);

    waitKey(0);
#endif
  } catch (exception e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
  return 0;
}
