#ifndef RANSAC_H
#define RANSAC_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/calib3d/calib3d.hpp"
#include "highgui.h"
#include <vector>

using namespace cv;
using namespace std;

void mRANSAC(Mat& src, Mat& target, vector<DMatch>& mathces, vector<KeyPoint>& keypoints_1, vector<KeyPoint>& keypoints_2);

#endif // !RANSAC_H