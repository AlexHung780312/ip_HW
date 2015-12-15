// author: Alex Hung
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

void gsl_1d_interpolation(const Mat& src, Mat& tgt) {
  tgt = src.clone();
  double* xarry = new double[src.cols];
  double* yarry = new double[src.cols];
  // green
  for (int r = 0; r < src.rows; r++) {
    int idx = 0;
    for (int c = r % 2; c < src.cols; c += 2) {
      xarry[idx] = c;
      yarry[idx] = src.at<Vec3b>(r, c)[1];
      idx++;
    }
    gsl_spline* interp = gsl_spline_alloc(gsl_interp_cspline, idx);
    gsl_interp_accel* acc = gsl_interp_accel_alloc();
    gsl_spline_init(interp, xarry, yarry, idx);
    for (int c = (r % 2) + 1; c < src.cols; c += 2) {
      if (c >= interp->interp->xmin && c <= interp->interp->xmax)
        tgt.at<Vec3b>(r, c)[1] = static_cast<unsigned char>(
            min(255.0, max(0.0, gsl_spline_eval(interp, c, acc) / 2.0)));
    }
    gsl_spline_free(interp);
    gsl_interp_accel_free(acc);
  }
  for (int c = 0; c < src.cols; c++) {
    int idx = 0;
    for (int r = c % 2; r < src.rows; r += 2) {
      xarry[idx] = r;
      yarry[idx] = src.at<Vec3b>(r, c)[1];
      idx++;
    }
    gsl_spline* interp = gsl_spline_alloc(gsl_interp_cspline, idx);
    gsl_interp_accel* acc = gsl_interp_accel_alloc();
    gsl_spline_init(interp, xarry, yarry, idx);
    for (int r = (c % 2) + 1; r < src.rows; r += 2) {
      if (r >= interp->interp->xmin && r <= interp->interp->xmax)
        tgt.at<Vec3b>(r, c)[1] += static_cast<unsigned char>(
            min(255.0, max(0.0, gsl_spline_eval(interp, r, acc) / 2.0)));
    }
    gsl_spline_free(interp);
    gsl_interp_accel_free(acc);
  }

  // blue
  for (int r = 0; r < src.rows; r += 2) {
    int idx = 0;
    for (int c = 1; c < src.cols; c += 2) {
      xarry[idx] = c;
      yarry[idx] = src.at<Vec3b>(r, c)[0];
      idx++;
    }
    gsl_spline* interp = gsl_spline_alloc(gsl_interp_cspline, idx);
    gsl_interp_accel* acc = gsl_interp_accel_alloc();
    gsl_spline_init(interp, xarry, yarry, idx);
    for (int c = 0; c < src.cols; c += 2) {
      if (c >= interp->interp->xmin && c <= interp->interp->xmax)
        tgt.at<Vec3b>(r, c)[0] = static_cast<unsigned char>(
            min(255.0, max(0.0, gsl_spline_eval(interp, c, acc))));
    }
    gsl_spline_free(interp);
    gsl_interp_accel_free(acc);
  }
  for (int c = 0; c < src.cols; c++) {
    int idx = 0;
    for (int r = 0; r < src.rows; r += 2) {
      xarry[idx] = r;
      yarry[idx] = tgt.at<Vec3b>(r, c)[0];
      idx++;
    }
    gsl_spline* interp = gsl_spline_alloc(gsl_interp_cspline, idx);
    gsl_interp_accel* acc = gsl_interp_accel_alloc();
    gsl_spline_init(interp, xarry, yarry, idx);
    for (int r = 1; r < src.rows; r += 2) {
      if (r >= interp->interp->xmin && r <= interp->interp->xmax)
        tgt.at<Vec3b>(r, c)[0] = static_cast<unsigned char>(
            min(255.0, max(0.0, gsl_spline_eval(interp, r, acc))));
    }
    gsl_spline_free(interp);
    gsl_interp_accel_free(acc);
  }
  // red

  for (int r = 1; r < src.rows; r += 2) {
    int idx = 0;
    for (int c = 0; c < src.cols; c += 2) {
      xarry[idx] = c;
      yarry[idx] = src.at<Vec3b>(r, c)[2];
      idx++;
    }
    gsl_spline* interp = gsl_spline_alloc(gsl_interp_cspline, idx);
    gsl_interp_accel* acc = gsl_interp_accel_alloc();
    gsl_spline_init(interp, xarry, yarry, idx);
    for (int c = 1; c < src.cols; c += 2) {
      if (c >= interp->interp->xmin && c <= interp->interp->xmax)
        tgt.at<Vec3b>(r, c)[2] = static_cast<unsigned char>(
            min(255.0, max(0.0, gsl_spline_eval(interp, c, acc))));
    }
    gsl_spline_free(interp);
    gsl_interp_accel_free(acc);
  }
  for (int c = 0; c < src.cols; c++) {
    int idx = 0;
    for (int r = 1; r < src.rows; r += 2) {
      xarry[idx] = r;
      yarry[idx] = tgt.at<Vec3b>(r, c)[2];
      idx++;
    }
    gsl_spline* interp = gsl_spline_alloc(gsl_interp_cspline, idx);
    gsl_interp_accel* acc = gsl_interp_accel_alloc();
    gsl_spline_init(interp, xarry, yarry, idx);
    for (int r = 0; r < src.rows; r += 2) {
      if (r >= interp->interp->xmin && r <= interp->interp->xmax)
        tgt.at<Vec3b>(r, c)[2] = static_cast<unsigned char>(
            min(255.0, max(0.0, gsl_spline_eval(interp, r, acc))));
    }
    gsl_spline_free(interp);
    gsl_interp_accel_free(acc);
  }
  delete[] xarry;
  delete[] yarry;
}

inline unsigned char fourPointsAvg(const Mat& src, int dim, Point2i a,
                                   Point2i b, Point2i c, Point2i d) {
  Rect rect(Point(), src.size());
  double sum = 0;
  int count = 0;
  if (rect.contains(a)) {
    sum += src.at<Vec3b>(a)[dim];
    count++;
  }
  if (rect.contains(b)) {
    sum += src.at<Vec3b>(b)[dim];
    count++;
  }
  if (rect.contains(c)) {
    sum += src.at<Vec3b>(c)[dim];
    count++;
  }
  if (rect.contains(d)) {
    sum += src.at<Vec3b>(d)[dim];
    count++;
  }
  return static_cast<unsigned char>(sum / count);
}
inline unsigned char twoPointsAvg(const Mat& src, int dim, Point2i a,
                                  Point2i b) {
  Rect rect(Point(), src.size());
  double sum = 0;
  int count = 0;
  if (rect.contains(a)) {
    sum += src.at<Vec3b>(a)[dim];
    count++;
  }
  if (rect.contains(b)) {
    sum += src.at<Vec3b>(b)[dim];
    count++;
  }
  return static_cast<unsigned char>(sum / count);
}

void bilinearInterpolationFromBayerImg(const Mat& src, Mat& tgt) {
  tgt = src.clone();

  for (int r = 1; r < tgt.rows - 1; r++) {
    for (int c = 1; c < tgt.cols - 1; c++) {
      /* fill Green */
      if ((r + c) % 2 == 1) {
        tgt.at<Vec3b>(r, c)[1] =
            fourPointsAvg(tgt, 1, Point(c, r + 1), Point(c, r - 1),
                          Point(c + 1, r), Point(c - 1, r));
      }
      /*fill Blue*/
      if (r % 2 == 0 && c % 2 == 0) {
        tgt.at<Vec3b>(r, c)[0] =
            twoPointsAvg(tgt, 0, Point(c + 1, r), Point(c - 1, r));
      } else if (r % 2 == 1 && c % 2 == 1) {
        tgt.at<Vec3b>(r, c)[0] =
            twoPointsAvg(tgt, 0, Point(c, r + 1), Point(c, r - 1));

      } else if (r % 2 == 1 && c % 2 == 0) {
        tgt.at<Vec3b>(r, c)[0] =
            fourPointsAvg(tgt, 0, Point(c + 1, r + 1), Point(c - 1, r + 1),
                          Point(c + 1, r - 1), Point(c - 1, r - 1));
      }
      /*fill red*/
      if (r % 2 == 0 && c % 2 == 0) {
        tgt.at<Vec3b>(r, c)[2] =
            twoPointsAvg(tgt, 2, Point(c, r + 1), Point(c, r - 1));

      } else if (r % 2 == 1 && c % 2 == 1) {
        tgt.at<Vec3b>(r, c)[2] =
            twoPointsAvg(tgt, 2, Point(c + 1, r), Point(c - 1, r));
      } else if (r % 2 == 0 && c % 2 == 1) {
        tgt.at<Vec3b>(r, c)[2] =
            fourPointsAvg(tgt, 2, Point(c + 1, r + 1), Point(c - 1, r + 1),
                          Point(c + 1, r - 1), Point(c - 1, r - 1));
      }
    }
  }
}

void getBayerImg(const Mat& src, Mat& tgt) {
  /* src is BGR format */
  tgt = src.clone();
  /* remove colors */
  for (int r = 0; r < tgt.rows; r++) {
    for (int c = 0; c < tgt.cols; c++) {
      if ((r + c) % 2 == 0) {
        /*keep green*/
        tgt.at<Vec3b>(r, c)[0] = tgt.at<Vec3b>(r, c)[2] = 0;
      } else if (r % 2 == 0) {
        /*keep blue*/
        tgt.at<Vec3b>(r, c)[1] = tgt.at<Vec3b>(r, c)[2] = 0;
      } else {
        /*keep red*/
        tgt.at<Vec3b>(r, c)[0] = tgt.at<Vec3b>(r, c)[1] = 0;
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cerr << "usage: ./hw5 img.tiff\n"
            "eg: ./hw5\n";
    exit(1);
  }
  try {
    Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat bayer_img;
    getBayerImg(img, bayer_img);
    Mat linear_img;
    bilinearInterpolationFromBayerImg(bayer_img, linear_img);
    Mat diff_img = max(linear_img, img) - min(linear_img, img);
    Mat spline_img;
    gsl_1d_interpolation(bayer_img, spline_img);
    Mat diff_img2 = max(spline_img, img) - min(spline_img, img);
    cout << (int)spline_img.at<Vec3b>(3, 2)[0] << endl
         << (int)img.at<Vec3b>(3, 2)[0] << endl
         << (int)bayer_img.at<Vec3b>(3, 2)[0] << endl;
#ifdef SHOW_IMG_WINDOW
    namedWindow("Color image",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Color image", img);
    moveWindow("Color image", 0, 0);
    namedWindow("Bayer image",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Bayer image", bayer_img);
    moveWindow("Bayer image", img.rows + 20, 0);
    namedWindow("Linear interpolation",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Linear interpolation", linear_img);
    moveWindow("Linear interpolation", 0, img.cols + 20);
    namedWindow("difference",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("difference", diff_img);
    moveWindow("difference", img.rows + 20, img.cols + 20);
    namedWindow("Spline interpolation",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Spline interpolation", spline_img);
    moveWindow("Spline interpolation", 0, (img.cols + 20) * 2);
    namedWindow("difference(Spline)",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("difference(Spline)", diff_img2);
    moveWindow("difference(Spline)", img.rows + 20, (img.cols + 20) * 2);
#endif
    waitKey(0);
  } catch (exception& e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
}
