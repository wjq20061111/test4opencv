#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include <algorithm>
#include <cmath>
#include "Segement.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{

	Mat src = imread("shelve2.jpg");
	Mat img1;
	Mat imageROI;



	resize(src, img1, Size(0, 0), 0.12, 0.12);
	//cvtColor(src, img1, CV_BGR2GRAY);
	segement(img1,imageROI);

	if (!imageROI.empty())
	{
		imshow("line", imageROI);

		Mat imgCanny;
		GaussCanny(imageROI, imgCanny, 3);

		vector<pair<float, float>> vertical_lines;//筛选线数组
		lineFilterVer(imgCanny, vertical_lines, 110);

		//imshow("canny", imgCanny);

		sort(vertical_lines.begin(), vertical_lines.end(), paircompare);//从左到右排序
		cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
		pair<float, float> lastline = vertical_lines.at(0);//上条线
		vector<pair<float, float>> edge;
		float zero = 0;
		edge.push_back(make_pair(zero, zero));
		edge.push_back(make_pair(zero, zero));
		float dis = 0, delta = 0;//与上条线距离和夹角
		int edgeline = 0;
		int crosspoint[2];
		int width = imageROI.cols;
		int height = imageROI.rows;
		int range[4]{};
		Mat imagesei;
		char name[10];
		Mat imageper;
		for (unsigned int i = 0; i < vertical_lines.size(); i++)
		{
			//dis = vertical_lines.at(i).first - lastline.first;
			calCross(vertical_lines.at(i).first, vertical_lines.at(i).second, 0, CV_PI / 2, crosspoint);
			dis = crosspoint[0];
			calCross(lastline.first, lastline.second, 0, CV_PI / 2, crosspoint);
			dis = dis-crosspoint[0];
			if (dis < 0)
				dis = -dis;
			delta = vertical_lines.at(i).second - lastline.second;
			if (delta > CV_PI / 2)
				delta = delta - CV_PI;
			if (delta < -CV_PI / 2)
				delta = delta + CV_PI;

			if ((dis > 5 && dis < 70) && (delta > -2.5* CV_PI / 180 && delta < 2.5* CV_PI / 180))
				//距离间隔在一定范围内同时平行度良好认为是书架边缘
			{
				//float rho = vertical_lines.at(i).first, theta = vertical_lines.at(i).second;
				//drawHoughLine(rho, theta, imgCanny);
				edge.at(0) = lastline;
				edge.at(1) = vertical_lines.at(i);


				//calBorder(width, height, edge, range);
				//sprintf_s(name, "%d", i);
				//imagesei = imageROI(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));
				//imshow(name, imagesei);
				calBorder(width, height, edge, range);
				sprintf_s(name, "%d", i);
				imagesei = imageROI(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));
				//imshow(name, imagesei);


				calPerspect(width, height, range[0], range[1], edge, imagesei, imageper);
				imshow(name, imageper);

				edgeline++;
			}
			lastline = vertical_lines.at(i);
		}


		




		cout << "end\n";
	}
	else
		cout << "failed to detect\n";

	waitKey(0);

	return 0;
}