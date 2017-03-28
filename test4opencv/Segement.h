#ifndef SEGEMENT_H
#define SEGEMENT_H

#include <vector>
#include "opencv2/opencv.hpp"
#include <algorithm>
#include <cmath>
#include <math.h>

using namespace std;
using namespace cv;

int paircompare(const pair<float, float> &p1, const pair<float, float> &p2);
//比较霍夫检测得到的线，按rho从小到大排序，用于sort的自定义排序
//rho=pair.first theta=pair.second

void segement(Mat &src, Mat &dst, int* range);
//检测近似水平的直线，将间隔在一定范围的判定为书架层，分割层间的图像用于后续处理
//输入原图 通过reference输出分割图

void spineSegement(Mat &src, vector<Mat> &dst);
//检测竖直线进行书脊分割，分割后进行校准，存入目标容器中
//输入单层书架图 通过reference输出分割容器

void drawHoughLine(float rho, float theta, Mat &obj, double R = 255, double G = 0, double B = 0);
//在指定图上画极坐标线
//输入rho，theta 通过引用输出
//考虑加入颜色

void GaussCanny(Mat &src, Mat &dst, int psize);
//高斯滤波+canny边缘提取
//输入原图、引用目标、高斯模板尺寸

void lineFilterHor(Mat &src, vector<pair<float, float>> &horizontal_lines, int threshold);
//霍夫检测直线，筛选水平线
//输入边缘图(gray) 输出目标容器 输入阈值

void lineFilterVer(Mat &src, vector<pair<float, float>> &vertical_lines, int threshold);
//霍夫检测直线，筛选垂直线
//输入边缘图(gray) 输出目标容器 输入阈值

void calCross(double rho1, double theta1, double rho2, double theta2, int* crosspoint);
//计算两直线交点
//输入两直线极坐标参数 输出在crosspoint数组中，[1]为x，[2]为y

void calBorder(int width, int height, vector<pair<float, float>> &edge, int* range);
//计算截取边界，求两条目标直线与四边线的交点，取能完全包含两直线的范围
//输入图像宽高 输入两直线 输出范围的对角两点(x0,y0,x1,y1)

void calPerspect(int width, int height, int orix, int oriy, vector<pair<float, float>> &edge, Mat& src, Mat& dst);
//计算透视校准
//输入书架层图宽高，输入截取书本原点，输入两条边界，书架层图像 输出书脊图  

#endif // !SEGEMENT_H