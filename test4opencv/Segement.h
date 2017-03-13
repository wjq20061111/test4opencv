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
void drawHoughLine(float rho, float theta, Mat &obj);
void GaussCanny(Mat &src, Mat &dst, int psize);
void lineFilterHor(Mat &src, vector<pair<float, float>> &horizontal_lines, int threshold);
void lineFilterVer(Mat &src, vector<pair<float, float>> &vertical_lines, int threshold);
void calCross(double rho1, double theta1, double rho2, double theta2, int* crosspoint);
void calBorder(int width, int height, vector<pair<float, float>> &edge, int* range);
void calPerspect(int width, int height, int orix, int oriy, vector<pair<float, float>> &edge, Mat& src, Mat& dst);

#endif // !SEGEMENT_H