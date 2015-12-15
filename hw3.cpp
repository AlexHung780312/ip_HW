// author: Alex Hung
#include <opencv/cv.hpp>
#include <iostream>
#include <exception>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace cv;
using namespace std;

void getChamfer(Mat& chamfer, const Mat& src) {
  Mat chamfer_tmp(258, 258, CV_8UC1, Scalar(255));
  src.copyTo(chamfer_tmp(Rect(1, 1, 256, 256)));
  for (int i = 2; i <= 256; i++) {
    for (int j = 2; j <= 256; j++) {
      chamfer_tmp.at<uchar>(i, j) = min((int)chamfer_tmp.at<uchar>(i, j),
                                        chamfer_tmp.at<uchar>(i - 1, j) + 2);
      chamfer_tmp.at<uchar>(i, j) =
          min((int)chamfer_tmp.at<uchar>(i, j),
              chamfer_tmp.at<uchar>(i - 1, j - 1) + 3);
      chamfer_tmp.at<uchar>(i, j) = min((int)chamfer_tmp.at<uchar>(i, j),
                                        chamfer_tmp.at<uchar>(i, j - 1) + 2);
      chamfer_tmp.at<uchar>(i, j) =
          min((int)chamfer_tmp.at<uchar>(i, j),
              chamfer_tmp.at<uchar>(i + 1, j - 1) + 3);
    }
  }
  for (int i = 255; i >= 1; i--) {
    for (int j = 255; j >= 1; j--) {
      chamfer_tmp.at<uchar>(i, j) = min((int)chamfer_tmp.at<uchar>(i, j),
                                        chamfer_tmp.at<uchar>(i + 1, j) + 2);
      chamfer_tmp.at<uchar>(i, j) =
          min((int)chamfer_tmp.at<uchar>(i, j),
              chamfer_tmp.at<uchar>(i + 1, j + 1) + 3);
      chamfer_tmp.at<uchar>(i, j) = min((int)chamfer_tmp.at<uchar>(i, j),
                                        chamfer_tmp.at<uchar>(i, j + 1) + 2);
      chamfer_tmp.at<uchar>(i, j) =
          min((int)chamfer_tmp.at<uchar>(i, j),
              chamfer_tmp.at<uchar>(i - 1, j + 1) + 3);
    }
  }
  // Rescale to 0~255
  chamfer_tmp(Rect(1, 1, 256, 256)).copyTo(chamfer);
  int floor_ = *min_element(chamfer.begin<uchar>(), chamfer.end<uchar>());
  int ceil_ = *max_element(chamfer.begin<uchar>(), chamfer.end<uchar>());

  chamfer = (chamfer - floor_) * (255.0 / (ceil_ - floor_));
}
void getPointsOfStar(vector<Point>& vec, const Point& centroid) {
  vec.resize(10);
  for (int i = 0; i < 10; i++) {
    float x = sin((i)*0.2 * CV_PI);
    float y = cos((i)*0.2 * CV_PI);
    if (i % 2 == 1) {
      vec[i] = Point(centroid.x - 40 * x, centroid.y - 40 * y);
    } else {
      vec[i] = Point(centroid.x - 80 * x, centroid.y - 80 * y);
    }
  }
}
void drawStarBoundary(Mat& img, const vector<Point>& points) {
  int thickness = 5;
  int lineType = CV_AA;
  for (int i = 1; i < points.size(); i++) {
    line(img, points[i - 1], points[i], Scalar(255), thickness, lineType);
  }
  line(img, points.back(), points.front(), Scalar(255), thickness, lineType);
}
void drawStarCorners(Mat& img, const vector<Point>& points) {
  for (int i = 0; i < points.size(); i++) {
    // img.at<uchar>(points[i]) = 255;
    circle(img, points[i], 2, Scalar(255), -2, CV_AA);
  }
}
int main(int argc, char* argv[]) {
  /*
    if (argc < 3) {
      cout << "usage: ./hw3 out-boundary-chamfer out-corners-chamfer\n"
              "eg: ./hw3 img/star_boundary_chamferv.tiff "
              "img/star_corners_chamferv.tiff\n";
      // exit(1);
    }*/
  try {
    /// get points
    vector<Point> points;
    getPointsOfStar(points, Point(127, 127));
    /// draw a star
    Mat star_img = Mat::zeros(256, 256, CV_8UC1);
    drawStarBoundary(star_img, points);
    /// initial chamfer array D
    Mat star_D(256, 256, CV_8UC1, Scalar(255));
    getChamfer(star_D, star_img);
    /// draw corners
    Mat corner_img = Mat::zeros(256, 256, CV_8UC1);
    drawStarCorners(corner_img, points);
    /// initial chamfer array D
    Mat corner_D(256, 256, CV_8UC1, Scalar(255));
    getChamfer(corner_D, corner_img);

#ifdef SHOW_IMG_WINDOW
    namedWindow("Boundary",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Boundary", star_img);
    moveWindow("Boundary", 0, 100);
    namedWindow("Boundary_Chamfer",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Boundary_Chamfer", star_D);
    moveWindow("Boundary_Chamfer", 300, 100);
    namedWindow("Corners",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Corners", corner_img);
    moveWindow("Corners", 0, 400);
    namedWindow("Corners_Chamfer",
                CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    imshow("Corners_Chamfer", corner_D);
    moveWindow("Corners_Chamfer", 300, 400);
    waitKey(0);
#endif
    /// write out
    imwrite("img/Boundary.tiff", star_img);
    imwrite("img/Corners.tiff", corner_img);
    imwrite("img/Boundary_Chamfer.tiff", star_D);
    imwrite("img/Corners_Chamfer.tiff", corner_D);
  } catch (exception& e) {
    cerr << "exception caught: " << e.what() << '\n';
  }
  return 0;
}
