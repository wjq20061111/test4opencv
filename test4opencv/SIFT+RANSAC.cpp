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
	//��ƥ�������ͼ��
	Mat img1 = imread("car2.jpg", 0);
	Mat img2 = imread("car1.jpg", 0);

	SIFT sift1(200, 3, 0.05, 10, 1.6);
	SIFT sift2(200, 3, 0.05, 10, 1.6);

	vector<KeyPoint> key_points1, key_points2;

	Mat descriptors1, descriptors2, mascara;

	sift1(img1, mascara, key_points1, descriptors1);
	sift2(img2, mascara, key_points2, descriptors2);


	Mat output_img;

	drawKeypoints(img1,     //����ͼ��  
		key_points1,      //������ʸ��  
		output_img,      //���ͼ��  
		Scalar::all(-1),      //�������������ɫ��Ϊ���  
							  //��������Ϊ���Ļ�Բ��Բ�İ뾶��ʾ������Ĵ�С��ֱ�߱�ʾ������ķ���  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	namedWindow("SIFT");
	imshow("SIFT", output_img);


	//ʵ��������ƥ��������BruteForceMatcher  
	BruteForceMatcher<L2<float>> matcher;
	//����ƥ��������  
	vector<DMatch>matches;
	//ʵ��������֮���ƥ�䣬�õ�����matches  
	matcher.match(descriptors1, descriptors2, matches);

	//��ȡ��ǰ30�����ƥ����  
	std::nth_element(matches.begin(),     //ƥ�������ӵĳ�ʼλ��  
		matches.begin() + 99,     // ���������  
		matches.end());       // ����λ��  
							  //�޳��������ƥ����  
	matches.erase(matches.begin() + 100, matches.end());

	namedWindow("SIFT_matches");
	Mat img_matches;
	//�����ͼ���л���ƥ����  
	drawMatches(img1, key_points1,         //��һ��ͼ�������������  
		img2, key_points2,      //�ڶ���ͼ�������������  
		matches,       //ƥ��������  
		img_matches,      //ƥ�����ͼ��  
		Scalar::all(-1));     //��ֱ����������ͼ���е�������  
	imshow("SIFT_matches", img_matches);

	vector<KeyPoint> R_keypoint01, R_keypoint02;
	for (size_t i = 0; i<matches.size(); i++)
	{
		R_keypoint01.push_back(key_points1[matches[i].queryIdx]);
		R_keypoint02.push_back(key_points2[matches[i].trainIdx]);
		//�����仰����⣺R_keypoint1��Ҫ�洢img01������img02ƥ��������㣬
		//matches�д洢����Щƥ���Ե�img01��img02������ֵ
	}

	//����ת��
	vector<Point2f>p01, p02;
	for (size_t i = 0; i<matches.size(); i++)
	{
		p01.push_back(R_keypoint01[i].pt);
		p02.push_back(R_keypoint02[i].pt);
	}

	//���û��������޳���ƥ���
	vector<uchar> RansacStatus;
	Mat Fundamental = findFundamentalMat(p01, p02, RansacStatus, FM_RANSAC);


	vector<KeyPoint> RR_keypoint01, RR_keypoint02;
	vector<DMatch> RR_matches;            //���¶���RR_keypoint ��RR_matches���洢�µĹؼ����ƥ�����
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
	imshow("������ƥ����", img_RR_matches);


	waitKey(0);

	return 0;
}