#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Segement.h"
#include "SIFT+RANSAC.h"

using namespace std;
using namespace cv;


int main(int argc, char** argv)
{

	Mat book = imread(".\\seg\\1.jpg");
	Mat target = imread("book6.jpg");

	
	
	string path,realpath;
	string fname;
	path = ".\\seg\\";
	char name[10];
	for (int i = 0; i < 16; i++)
	{
		cout << "now: "<<i<<"\n" ;
		sprintf_s(name, "%d", i);
		fname = name;
		realpath = path + fname + ".jpg";
		book = imread(realpath);
		SIFTCompare(book, target);
	}
	//imshow("book", book);

	//vector<KeyPoint> keyPoint1;
	//vector<KeyPoint> keyPoint2;
	//SURF surf(300);			//1000为检测算子的阀值
	//surf.detect(book, keyPoint1, Mat());
	//surf.detect(target, keyPoint2, Mat());

	//Mat descriptors_1, descriptors_2;
	//surf.compute(book, keyPoint1, descriptors_1);
	//surf.compute(target, keyPoint2, descriptors_2);

	//drawKeypoints(book, keyPoint1, book
	//	, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255)), 2);
	//drawKeypoints(target, keyPoint2, target
	//	, Scalar(theRNG().uniform(0, 255), theRNG().uniform(0, 255), theRNG().uniform(0, 255)), 2);

	//imshow("book", book);
	//imshow("target", target);

	//cout << "book : " << keyPoint1.size() << " target : " << keyPoint2.size() << "\n";

	//BruteForceMatcher<L2<float> > matcher;//定义一个burte force matcher对象
	//									  //    FlannBasedMatcher matcher;
	//vector<DMatch>matches;
	//matcher.match(descriptors_1, descriptors_2, matches);

	//cout << "Match个数：" << matches.size() << endl;

	//double max_dist = 0;
	//double min_dist = 100;
	//for (int i = 0; i<matches.size(); i++)
	//{
	//	double dist = matches[i].distance;
	//	if (dist < min_dist) min_dist = dist;
	//	if (dist > max_dist) max_dist = dist;
	//}

	//vector<DMatch> goodMatches;
	//for (int i = 0; i<matches.size(); i++)
	//{
	//	if (matches[i].distance < 0.6 * max_dist)
	//	{
	//		goodMatches.push_back(matches[i]);
	//	}
	//}
	//cout << "goodMatch个数：" << goodMatches.size() << endl;

	////画出匹配结果     
	//Mat img_matches;
	////红色连接的是匹配的特征点对，绿色是未匹配的特征点     
	//drawMatches(book, keyPoint1, target, keyPoint2, goodMatches, img_matches,
	//	Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0, 255, 0), Mat(), 2);

	//imshow("MatchSIFT", img_matches);



	waitKey(0);
	return 0;
}