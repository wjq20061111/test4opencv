#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Segement.h"
#include "SIFT+RANSAC.h"
#include "ORB.h"


using namespace std;
using namespace cv;



int main(int argc, char** argv)
{

	//Mat book = imread(".\\seg\\4.jpg");
	//Mat target = imread("book2.jpg");
	Mat book = imread("g1.jpg");
	Mat target = imread("g2.jpg");

	//SIFTCompare(book, target);
	
	SIFTCompare( target,book);

		
	waitKey(0);
	return 0;
}
