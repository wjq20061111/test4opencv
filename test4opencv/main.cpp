#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Segement.h"
#include "SIFT.h"
#include "SURF.h"
#include "RANSAC.h"
#include "ORB.h"


using namespace std;
using namespace cv;



int main(int argc, char** argv)
{


	Mat src = imread("shelve7.jpg");
	Mat img1;
	Mat img2;
	Mat imageROI;//书架单层
	Mat imageROIbig;//书架单层

	resize(src, img2, Size(0, 0), 0.24, 0.24);
	resize(img2, img1, Size(0, 0), 0.5, 0.5);
	cout << "ori: w: " << img2.cols << " h: " << img2.rows << "\n";
	cout << "half: w: " << img1.cols << " h: " << img1.rows << "\n";
	//cvtColor(src, img1, CV_BGR2GRAY);
	//imshow("half", img1);

	int range[4]{};
	segement(img1, imageROI, range);
	for (int i = 1; i < 4; i++)
	{
		range[i] = range[i] - 1;
	}
	imageROIbig = img2(Rect((range[0]) * 2, (range[1]) * 2, (range[2] - range[0]) * 2, (range[3] - range[1]) * 2));
	//imshow("big", imageROIbig);
	vector<Mat> spine;//分割的书脊

	if (!imageROI.empty())
	{

		//string path("E:\\c++\\test4opencv\\test4opencv\\seg\\");
		//string realpath;
		//string fname;
		spineSegement(imageROIbig, spine);
		char name[10];
		for (int i = 0; i < spine.size(); i++)
		{
			//sprintf_s(name, "%d", i);
			//fname = name;
			//realpath = path + fname + ".jpg";
			//imwrite(realpath, spine.at(i));
			//imshow(name, spine.at(i));
		}
		cout << "get " << spine.size() << "\n";

		//Mat targetbook = imread("book1.jpg");


		//SIFTCompare(spine.at(19), targetbook);


	}
	else
		cout << "failed to detect shelves\n";
		
	waitKey(0);
	return 0;
}
