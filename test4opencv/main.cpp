#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Segement.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{

	Mat src = imread("shelve2.jpg");
	Mat img1;
	Mat imageROI;//一层

	resize(src, img1, Size(0, 0), 0.12, 0.12);
	//resize(src, img1, Size(0, 0), 0.24, 0.24);
	cout << "w: " << img1.cols << " h: " << img1.rows << "\n";
	//cvtColor(src, img1, CV_BGR2GRAY);
	//imshow("ori", img1);
	segement(img1,imageROI);

	vector<Mat> spine;//分割的书脊
	
	if (!imageROI.empty())
	{
		//imshow("line", imageROI);

		spineSegement(imageROI, spine);
		imshow("s1", spine.at(0));
		
		cout << "end\n";
	}
	else
		cout << "failed to detect\n";

	waitKey(0);

	return 0;
}