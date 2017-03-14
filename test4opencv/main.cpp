#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Segement.h"
#include "SIFT+RANSAC.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{

	Mat src = imread("shelve2.jpg");
	Mat img1;
	Mat img2;
	Mat imageROI;//一层
	Mat imageROIbig;//一层

	resize(src, img1, Size(0, 0), 0.12, 0.12);
	resize(src, img2, Size(0, 0), 0.24, 0.24);
	cout << "w: " << img1.cols << " h: " << img1.rows << "\n";
	//cvtColor(src, img1, CV_BGR2GRAY);
	//imshow("ori", img1);
	int range[4]{};
	segement(img1,imageROI,range);
	for (int i = 1; i < 4; i++)
	{
		range[i] = range[i] - 1;
	}
	imageROIbig = img2(Rect((range[0])*2, (range[1])*2, (range[2] - range[0])*2, (range[3] - range[1])*2));
	//imshow("big", imageROIbig);
	vector<Mat> spine;//分割的书脊
	
	if (!imageROI.empty())
	{
		

		spineSegement(imageROIbig, spine);
		//char name[10];
		//for (int i=0; i < spine.size(); i++)
		//{
		//	sprintf_s(name, "%d", i);			
		//	imshow(name, spine.at(i));
		//}
		cout << "get "<< spine.size() <<"\n";

		Mat targetbook= imread("book1.jpg");


		SIFTCompare(spine.at(19), targetbook);


	}
	else
		cout << "failed to detect\n";





	waitKey(0);

	return 0;
}