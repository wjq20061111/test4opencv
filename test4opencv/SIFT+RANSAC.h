#ifndef SIFTRANSAC_H
#define SIFTRANSAC_H


#include "opencv2/core/core.hpp"  
#include "highgui.h"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/nonfree/nonfree.hpp"  
#include "opencv2/legacy/legacy.hpp"  
#include <vector>

using namespace cv;
using namespace std;

void SIFTCompare(Mat& src,Mat& target);
void mRANSAC(Mat& src, Mat& target, vector<DMatch>& mathces, vector<KeyPoint>& keypoints_1, vector<KeyPoint>& keypoints_2);


#endif // !SIFTRANSAC_H