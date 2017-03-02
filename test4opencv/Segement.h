#ifndef SEGEMENT_H
#define SEGEMENT_H

#include <vector>
#include "opencv2/opencv.hpp"
#include <algorithm>
#include <cmath>

using namespace std;
using namespace cv;

int paircompare(const pair<float, float> &p1, const pair<float, float> &p2);
void segement(Mat &src, Mat &dst);

#endif // !SEGEMENT_H