#include "RANSAC.h"

void mRANSAC(Mat& src, Mat& target, vector<DMatch>& mathces, vector<KeyPoint>& keypoints_1, vector<KeyPoint>& keypoints_2)
{
	vector<KeyPoint> R_keypoint01, R_keypoint02;
	for (size_t i = 0; i < mathces.size(); i++)
	{
		R_keypoint01.push_back(keypoints_1[mathces[i].queryIdx]);
		R_keypoint02.push_back(keypoints_2[mathces[i].trainIdx]);
		//这两句话的理解：R_keypoint1是要存储img01中能与img02匹配的特征点，
		//matches中存储了这些匹配点对的img01和img02的索引值
	}

	//坐标转换
	vector<Point2f>p01, p02;
	for (size_t i = 0; i < mathces.size(); i++)
	{
		p01.push_back(R_keypoint01[i].pt);
		p02.push_back(R_keypoint02[i].pt);
	}

	//利用基础矩阵剔除误匹配点
	vector<uchar> RansacStatus;
	Mat Fundamental = findFundamentalMat(p01, p02, RansacStatus, FM_RANSAC);
	vector<uchar> RansacStatus2;
	Mat Homography = findHomography(p01, p02, CV_RANSAC, 3, RansacStatus2);

	vector<KeyPoint> RR_keypoint01, RR_keypoint02;
	vector<DMatch> RR_matches;            //重新定义RR_keypoint 和RR_matches来存储新的关键点和匹配矩阵
	int index = 0;
	for (size_t i = 0; i < mathces.size(); i++)
	{
		if (RansacStatus2[i] != 0)
		{
			RR_keypoint01.push_back(R_keypoint01[i]);
			RR_keypoint02.push_back(R_keypoint02[i]);
			mathces[i].queryIdx = index;
			mathces[i].trainIdx = index;
			RR_matches.push_back(mathces[i]);
			index++;
		}
	}

	vector<Point2f> obj_corners(4);
	obj_corners.at(0).x = 0;		obj_corners.at(0).y = 0;
	obj_corners.at(1).x = src.cols; obj_corners.at(1).y = 0;
	obj_corners.at(2).x = src.cols; obj_corners.at(2).y = src.rows;
	obj_corners.at(3).x = 0;		obj_corners.at(3).y = src.rows;
	line(src, obj_corners[0], obj_corners[1], Scalar(0, 255, 0), 2);
	line(src, obj_corners[1], obj_corners[2], Scalar(0, 255, 0), 2);
	line(src, obj_corners[2], obj_corners[3], Scalar(0, 255, 0), 2);
	line(src, obj_corners[3], obj_corners[0], Scalar(0, 255, 0), 2);
	vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, Homography);
	line(target, scene_corners[0], scene_corners[1], Scalar(0, 0, 255), 2);
	line(target, scene_corners[1], scene_corners[2], Scalar(0, 0, 255), 2);
	line(target, scene_corners[2], scene_corners[3], Scalar(0, 0, 255), 2);
	line(target, scene_corners[3], scene_corners[0], Scalar(0, 0, 255), 2);

	Mat img_RR_matches;
	drawMatches(src, RR_keypoint01, target, RR_keypoint02, RR_matches, img_RR_matches);
	cout <<"ransac inners: "<< index << endl;
	imshow("消除误匹配点后",img_RR_matches);

}