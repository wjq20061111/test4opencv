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

#endif // !SIFTRANSAC_H