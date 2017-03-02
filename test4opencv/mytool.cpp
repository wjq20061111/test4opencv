#include "mytool.h"

int drawhistgray(Mat* src)
{
	int histSize = 255;
	float range[] = { 0, 255 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;
	Mat hist;
	calcHist(src, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
	
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);
	histSize = hist.rows;
	Mat histImage(histSize, histSize, CV_8U, Scalar(0));

	int hpt = static_cast<int>(0.9*histSize);

	for (int h = 0; h<histSize; h++)
	{
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		line(histImage, Point(h, histSize), Point(h, histSize - intensity), Scalar::all(255));
	}

	namedWindow("calcHist", CV_WINDOW_AUTOSIZE);
	imshow("calcHist", histImage);
	return 0;
}