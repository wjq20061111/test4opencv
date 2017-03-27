#include"SIFT+RANSAC.h"

void SIFTCompare(Mat& src,Mat& target)
{
	//��ƥ�������ͼ��
	Mat img1 = src;
	Mat img2 = target;

	SIFT sift1(200, 3, 0.05, 10, 1.6);
	SIFT sift2(200, 3, 0.05, 10, 1.6);

	vector<KeyPoint> key_points1, key_points2;

	Mat descriptors1, descriptors2, mascara;

	sift1(img1, mascara, key_points1, descriptors1);
	sift2(img2, mascara, key_points2, descriptors2);

	cout << key_points1.size() << " " << key_points2.size() << "\n";

	Mat output_img;

	drawKeypoints(img1,     //����ͼ��  
		key_points1,      //������ʸ��  
		output_img,      //���ͼ��  
		Scalar::all(-1),      //�������������ɫ��Ϊ���  
							  //��������Ϊ���Ļ�Բ��Բ�İ뾶��ʾ������Ĵ�С��ֱ�߱�ʾ������ķ���  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//namedWindow("SIFT1");
	//imshow("SIFT1", output_img);

	drawKeypoints(img2,     //����ͼ��  
		key_points2,      //������ʸ��  
		output_img,      //���ͼ��  
		Scalar::all(-1),      //�������������ɫ��Ϊ���  
							  //��������Ϊ���Ļ�Բ��Բ�İ뾶��ʾ������Ĵ�С��ֱ�߱�ʾ������ķ���  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//namedWindow("SIFT2");
	//imshow("SIFT2", output_img);


	//ʵ��������ƥ��������BruteForceMatcher  
	BruteForceMatcher<L2<float>> matcher;
	//����ƥ��������  
	vector<DMatch>matches;
	//ʵ��������֮���ƥ�䣬�õ�����matches  
	matcher.match(descriptors1, descriptors2, matches);

	//��ȡ��ǰ30�����ƥ����  
	if (matches.size() > 100)
	{
		std::nth_element(matches.begin(),     //ƥ�������ӵĳ�ʼλ��  
			matches.begin() + 99,     // ���������  
			matches.end());       // ����λ��  
								  //�޳��������ƥ����  
		matches.erase(matches.begin() + 100, matches.end());
	}


	//namedWindow("SIFT_matches");
	Mat img_matches;
	//�����ͼ���л���ƥ����  
	//drawMatches(img1, key_points1,         //��һ��ͼ�������������  
	//	img2, key_points2,      //�ڶ���ͼ�������������  
	//	matches,       //ƥ��������  
	//	img_matches,      //ƥ�����ͼ��  
	//	Scalar::all(-1));     //��ֱ����������ͼ���е�������  
	//imshow("SIFT_matches", img_matches);

	mRANSAC(src, target, matches, key_points1, key_points2);

}

void mRANSAC(Mat& src, Mat& target, vector<DMatch>& mathces, vector<KeyPoint>& keypoints_1, vector<KeyPoint>& keypoints_2)
{
	vector<KeyPoint> R_keypoint01, R_keypoint02;
	for (size_t i = 0; i<mathces.size(); i++)
	{
		R_keypoint01.push_back(keypoints_1[mathces[i].queryIdx]);
		R_keypoint02.push_back(keypoints_2[mathces[i].trainIdx]);
		//�����仰����⣺R_keypoint1��Ҫ�洢img01������img02ƥ��������㣬
		//matches�д洢����Щƥ���Ե�img01��img02������ֵ
	}

	//����ת��
	vector<Point2f>p01, p02;
	for (size_t i = 0; i<mathces.size(); i++)
	{
		p01.push_back(R_keypoint01[i].pt);
		p02.push_back(R_keypoint02[i].pt);
	}

	//���û��������޳���ƥ���
	vector<uchar> RansacStatus;
	Mat Fundamental = findFundamentalMat(p01, p02, RansacStatus, FM_RANSAC);
	vector<uchar> RansacStatus2;
	Mat Homography = findHomography(p01, p02, CV_RANSAC, 3, RansacStatus2);

	vector<KeyPoint> RR_keypoint01, RR_keypoint02;
	vector<DMatch> RR_matches;            //���¶���RR_keypoint ��RR_matches���洢�µĹؼ����ƥ�����
	int index = 0;
	for (size_t i = 0; i<mathces.size(); i++)
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
	cout << index << endl;
	imshow("������ƥ����", img_RR_matches);

}