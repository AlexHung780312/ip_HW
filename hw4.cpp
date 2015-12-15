// author: Alex Hung
#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
using namespace cv;
using namespace std;

Mat drawHistogram(const Mat& img) {
  Mat hist(256, 256, CV_8UC1, Scalar(255));
  vector<int> count(256, 0);
  for (int y = 0; y < img.rows; y++) {
    for (int x = 0; x < img.cols; x++) {
      count[img.at<uchar>(Point(x, y))]++;
    }
  }
  for (int i = 0; i < 256; i++) {
    line(hist, Point(i, 255), Point(i, 256 * (1.f - 20 * (count[i] / 65536.f))),
         Scalar(0));
  }
  return hist;
}
/*
 * Algo.2.3
 */
inline void fillGaussianNoise(Mat& img, float mean, float stdev, mt19937& gen) {
  uniform_real_distribution<> dis(0.f, 1.f);
  for (int y = 0; y < img.rows; y += 2) {
    for (int x = 0; x < img.cols; x++) {
      float garma = dis(gen);
      float phi = dis(gen);
      float z1 = stdev * cos(2.f * M_PI * phi) * sqrtf(-2.f * logf(garma));
      float z2 = stdev * sin(2.f * M_PI * phi) * sqrtf(-2.f * logf(garma));
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
float gaussian(float mean, float stdev, float x) {
  float diff = x - mean;
  float result = exp(-(diff * diff) / (2.f * (stdev * stdev)));
  return 0.3989422804f / stdev * result;
}
float log_add(float lna, float lnb) {
  if (lna == 1.f) return lnb;
  if (lnb == 1.f) return lna;
  float diff = lna - lnb;
  if (diff < 23.0)
    return log1p(expf(diff)) + lnb;
  else
    return lna;
}
/*
 * Method 2
 * 有95%機率會落在正負兩倍標準差區間內
 * 如果sigma=10，區間為[-20,+20]時候相當於分佈面積會多了5%的部分
 * 造成統計圖比Algo2.3方法還要凸起
 */
inline void fillGaussianNoiseMethod2(Mat& img, float mean, float stdev,
                                     mt19937& gen, bool fix_ = false) {
  float ceilling_ = mean + stdev * 2.f;
  float floor_ = mean - stdev * 2.f;
  int sections_ = 1024;
  float interv_ = (ceilling_ - floor_) / sections_;
  vector<float> pdfs(sections_ + 3, 0.f);
  vector<float> accu_pdfs(sections_ + 3, 0.f);
  float log_sum = -3.40282347E+38F;
  for (int i = 0; i <= sections_; i++) {
    float val = floor_ + i * interv_;
    pdfs[i + 1] = log(gaussian(mean, stdev, val));
    log_sum = log_add(log_sum, pdfs[i + 1]);
    accu_pdfs[i + 1] = log_sum;
  }
  float offset = -3.40282347E+38F;
  if (fix_) offset = log_sum - log(0.95) + log(0.025);
  for (auto itr = accu_pdfs.begin(); itr != accu_pdfs.end(); itr++) {
    *itr = log_add(*itr, offset);
  }

  accu_pdfs[0] = offset;
  accu_pdfs[sections_ + 2] = log_add(accu_pdfs[sections_ + 1], offset);
  uniform_real_distribution<> dis(1.0E-30F, exp(accu_pdfs.back()));
  // draw gaussian noise
  for (int y = 0; y < img.rows; y++) {
    for (int x = 0; x < img.cols; x++) {
      float z = 0.f;
      float rn = log(dis(gen));
      if (rn <= accu_pdfs.front()) {
        z = floor_;
      } else if (rn > accu_pdfs[sections_ + 1]) {
        z = ceilling_;
      } else {
        for (int i = 0; i <= sections_; i++) {
          if (rn <= accu_pdfs[i + 1]) {
            z = floor_ + i * interv_;
            break;
          }
        }
      }
      unsigned char p = static_cast<unsigned char>(std::max(
          0.f,
          std::min(255.f, static_cast<float>(img.at<uchar>(Point(y, x))) + z)));
      img.at<uchar>(Point(y, x)) = p;
    }
  }
}
inline void showImg(const char* title, const Mat& img, int x, int y) {
  namedWindow(title,
              CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
  imshow(title, img);
  moveWindow(title, x, y);
}

/*
 * Method 3
 * 有99.75%機率會落在正負三倍標準差區間內
 */
inline void fillGaussianNoiseMethod3(Mat& img, float mean, float stdev,
                                     mt19937& gen, bool fix_ = false) {
  float ceilling_ = mean + stdev * 3.f;
  float floor_ = mean - stdev * 3.f;
  int sections_ = 1024;
  float interv_ = (ceilling_ - floor_) / sections_;
  vector<float> pdfs(sections_ + 3, 0.f);
  vector<float> accu_pdfs(sections_ + 3, 0.f);
  float log_sum = -3.40282347E+38F;
  for (int i = 0; i <= sections_; i++) {
    float val = floor_ + i * interv_;
    pdfs[i + 1] = log(gaussian(mean, stdev, val));
    log_sum = log_add(log_sum, pdfs[i + 1]);
    accu_pdfs[i + 1] = log_sum;
  }
  float offset = -3.40282347E+38F;
  if (fix_) offset = log_sum - log(0.95) + log(0.025);
  for (auto itr = accu_pdfs.begin(); itr != accu_pdfs.end(); itr++) {
    *itr = log_add(*itr, offset);
  }
  accu_pdfs[0] = offset;
  accu_pdfs[sections_ + 2] = log_add(accu_pdfs[sections_ + 1], offset);
  uniform_real_distribution<> dis(1.0E-30F, exp(accu_pdfs.back()));
  // draw gaussian noise
  for (int y = 0; y < img.rows; y++) {
    for (int x = 0; x < img.cols; x++) {
      float z = 0.f;
      float rn = log(dis(gen));
      if (rn <= accu_pdfs.front()) {
        z = floor_;
      } else if (rn > accu_pdfs[sections_ + 1]) {
        z = ceilling_;
      } else {
        for (int i = 0; i <= sections_; i++) {
          if (rn <= accu_pdfs[i + 1]) {
            z = floor_ + i * interv_;
            break;
          }
        }
      }
      unsigned char p = static_cast<unsigned char>(std::max(
          0.f,
          std::min(255.f, static_cast<float>(img.at<uchar>(Point(y, x))) + z)));
      img.at<uchar>(Point(y, x)) = p;
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
    Mat hist_algo23_v10_img = drawHistogram(noise_algo23_v10_img);
    Mat hist_algo23_v15_img = drawHistogram(noise_algo23_v15_img);
    /// add gaussian noise method2
    Mat noise_m2_v10_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoiseMethod2(noise_m2_v10_img, 0, 10, gen);
    Mat noise_m2_v15_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoiseMethod2(noise_m2_v15_img, 0, 15, gen);
    Mat noise_m2_v15_fix_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoiseMethod2(noise_m2_v15_fix_img, 0, 15, gen, true);
    Mat hist_m2_v10_img = drawHistogram(noise_m2_v10_img);
    Mat hist_m2_v15_img = drawHistogram(noise_m2_v15_img);
    Mat hist_m2_v15_fix_img = drawHistogram(noise_m2_v15_fix_img);
    /// add gaussian noise method3
    Mat noise_m3_v10_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoiseMethod3(noise_m3_v10_img, 0, 10, gen);
    Mat noise_m3_v15_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoiseMethod3(noise_m3_v15_img, 0, 15, gen);
    Mat noise_m3_v15_fix_img(256, 256, CV_8UC1, Scalar(127));
    fillGaussianNoiseMethod3(noise_m3_v15_fix_img, 0, 15, gen, true);
    Mat hist_m3_v10_img = drawHistogram(noise_m3_v10_img);
    Mat hist_m3_v15_img = drawHistogram(noise_m3_v15_img);
    Mat hist_m3_v15_fix_img = drawHistogram(noise_m3_v15_fix_img);
#ifdef SHOW_IMG_WINDOW
    showImg("Gray image (i=127)", gray_img, 0, 100);
    showImg("Algo2.3, sigma=10", noise_algo23_v10_img, 300, 100);
    showImg("Algo2.3, sigma=15", noise_algo23_v15_img, 600, 100);
    showImg("Hist.Algo2.3, sigma=10", hist_algo23_v10_img, 900, 100);
    showImg("Hist.Algo2.3, sigma=15", hist_algo23_v15_img, 1200, 100);

    showImg("Method2,[-2*sigma,2*sigma], sigma=10", noise_m2_v10_img, 300, 400);
    showImg("Method2,[-2*sigma,2*sigma], sigma=15", noise_m2_v15_img, 600, 400);
    showImg("Hist.Method2,[-2*sigma,2*sigma], sigma=10", hist_m2_v10_img, 900,
            400);
    showImg("Hist.Method2,[-2*sigma,2*sigma], sigma=15", hist_m2_v15_img, 1200,
            400);

    showImg("Method2,[-3*sigma,3*sigma], sigma=10", noise_m3_v10_img, 300, 700);
    showImg("Method2,[-3*sigma,3*sigma], sigma=15", noise_m3_v15_img, 600, 700);
    showImg("Hist.Method2,[-3*sigma,3*sigma], sigma=10", hist_m3_v10_img, 900,
            700);
    showImg("Hist.Method2,[-3*sigma,3*sigma], sigma=15", hist_m3_v15_img, 1200,
            700);

    waitKey(0);
#endif
    /// write out
    imwrite("img/algo23_v10_img.tiff", noise_algo23_v10_img);
    imwrite("img/algo23_v15_img.tiff", noise_algo23_v15_img);
    imwrite("img/algo23_v10_hist.tiff", hist_algo23_v10_img);
    imwrite("img/algo23_v15_hist.tiff", hist_algo23_v15_img);
    ///
    imwrite("img/m2_2sigma_v10_img.tiff", noise_m2_v10_img);
    imwrite("img/m2_2sigma_v15_img.tiff", noise_m2_v15_img);
    imwrite("img/m2_2sigma_v10_hist.tiff", hist_m2_v10_img);
    imwrite("img/m2_2sigma_v15_hist.tiff", hist_m2_v15_img);
    ///
    imwrite("img/m2_3sigma_v10_img.tiff", noise_m3_v10_img);
    imwrite("img/m2_3sigma_v15_img.tiff", noise_m3_v15_img);
    imwrite("img/m2_3sigma_v10_hist.tiff", hist_m3_v10_img);
    imwrite("img/m2_3sigma_v15_hist.tiff", hist_m3_v15_img);
    ///
    imwrite("img/m2_2sigma_v15_fix_img.tiff", noise_m2_v15_fix_img);
    imwrite("img/m2_2sigma_v15_fix_hist.tiff", hist_m2_v15_fix_img);
    imwrite("img/m2_3sigma_v15_fix_img.tiff", noise_m3_v15_fix_img);
    imwrite("img/m2_3sigma_v15_fix_hist.tiff", hist_m3_v15_fix_img);
  } catch (exception& e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
  return 0;
}
