#include "opencv2/core/core.hpp"  
#include "highgui.h"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/nonfree/nonfree.hpp"  
#include "opencv2/legacy/legacy.hpp"  

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	//待匹配的两幅图像
	Mat img1 = imread("car2.jpg", 0);
	Mat img2 = imread("car1.jpg", 0);

	SIFT sift1(200, 3, 0.05, 10, 1.6);
	SIFT sift2(200, 3, 0.05, 10, 1.6);

	vector<KeyPoint> key_points1, key_points2;

	Mat descriptors1, descriptors2, mascara;

	sift1(img1, mascara, key_points1, descriptors1);
	sift2(img2, mascara, key_points2, descriptors2);


	Mat output_img;

	drawKeypoints(img1,     //输入图像  
		key_points1,      //特征点矢量  
		output_img,      //输出图像  
		Scalar::all(-1),      //绘制特征点的颜色，为随机  
							  //以特征点为中心画圆，圆的半径表示特征点的大小，直线表示特征点的方向  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	namedWindow("SIFT");
	imshow("SIFT", output_img);


	//实例化暴力匹配器——BruteForceMatcher  
	BruteForceMatcher<L2<float>> matcher;
	//定义匹配器算子  
	vector<DMatch>matches;
	//实现描述符之间的匹配，得到算子matches  
	matcher.match(descriptors1, descriptors2, matches);

	//提取出前30个最佳匹配结果  
	std::nth_element(matches.begin(),     //匹配器算子的初始位置  
		matches.begin() + 99,     // 排序的数量  
		matches.end());       // 结束位置  
							  //剔除掉其余的匹配结果  
	matches.erase(matches.begin() + 100, matches.end());

	namedWindow("SIFT_matches");
	Mat img_matches;
	//在输出图像中绘制匹配结果  
	drawMatches(img1, key_points1,         //第一幅图像和它的特征点  
		img2, key_points2,      //第二幅图像和它的特征点  
		matches,       //匹配器算子  
		img_matches,      //匹配输出图像  
		Scalar::all(-1));     //用直线连接两幅图像中的特征点  
	imshow("SIFT_matches", img_matches);

	vector<KeyPoint> R_keypoint01, R_keypoint02;
	for (size_t i = 0; i<matches.size(); i++)
	{
		R_keypoint01.push_back(key_points1[matches[i].queryIdx]);
		R_keypoint02.push_back(key_points2[matches[i].trainIdx]);
		//这两句话的理解：R_keypoint1是要存储img01中能与img02匹配的特征点，
		//matches中存储了这些匹配点对的img01和img02的索引值
	}

	//坐标转换
	vector<Point2f>p01, p02;
	for (size_t i = 0; i<matches.size(); i++)
	{
		p01.push_back(R_keypoint01[i].pt);
		p02.push_back(R_keypoint02[i].pt);
	}

	//利用基础矩阵剔除误匹配点
	vector<uchar> RansacStatus;
	Mat Fundamental = findFundamentalMat(p01, p02, RansacStatus, FM_RANSAC);


	vector<KeyPoint> RR_keypoint01, RR_keypoint02;
	vector<DMatch> RR_matches;            //重新定义RR_keypoint 和RR_matches来存储新的关键点和匹配矩阵
	int index = 0;
	for (size_t i = 0; i<matches.size(); i++)
	{
		if (RansacStatus[i] != 0)
		{
			RR_keypoint01.push_back(R_keypoint01[i]);
			RR_keypoint02.push_back(R_keypoint02[i]);
			matches[i].queryIdx = index;
			matches[i].trainIdx = index;
			RR_matches.push_back(matches[i]);
			index++;
		}
	}
	Mat img_RR_matches;
	drawMatches(img1, RR_keypoint01, img2, RR_keypoint02, RR_matches, img_RR_matches);
	imshow("消除误匹配点后", img_RR_matches);


	waitKey(0);

	return 0;
}