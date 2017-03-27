#ifndef ORB_H
#define ORB_H

#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

void ORBCompare(Mat& src, Mat& target);

#endif // !ORB_H