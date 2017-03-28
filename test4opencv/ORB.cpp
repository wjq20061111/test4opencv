#include "ORB.h"
#include "RANSAC.h"

void ORBCompare(Mat& src, Mat& target)
{
	// -- Step 1: Detect the keypoints using STAR Detector 
	vector<KeyPoint> keypoints_1, keypoints_2;
	ORB orb(250, 1.2f, 8, 1);
	orb.detect(src, keypoints_1);
	orb.detect(target, keypoints_2);

	// -- Stpe 2: Calculate descriptors (feature vectors) 
	Mat descriptors_1, descriptors_2;
	orb.compute(src, keypoints_1, descriptors_1);
	orb.compute(target, keypoints_2, descriptors_2);

	cout << "ORB: " << keypoints_1.size() << " " << keypoints_2.size() << "\n";

	//-- Step 3: Matching descriptor vectors with a brute force matcher 
	BFMatcher matcher(NORM_HAMMING,true);
	vector<DMatch> mathces;
	matcher.match(descriptors_1, descriptors_2, mathces);

	cout << "matches " << mathces.size() << "\n";

	// -- dwaw matches 
	Mat img_mathes;
	//drawMatches(src, keypoints_1, target, keypoints_2, mathces, img_mathes);
	// -- show 
	//imshow("Mathces", img_mathes);

	mRANSAC(src, target, mathces, keypoints_1, keypoints_2);
}

