#include"SIFT+RANSAC.h"

void SIFTCompare(Mat& src,Mat& target)
{
	//待匹配的两幅图像
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

	drawKeypoints(img1,     //输入图像  
		key_points1,      //特征点矢量  
		output_img,      //输出图像  
		Scalar::all(-1),      //绘制特征点的颜色，为随机  
							  //以特征点为中心画圆，圆的半径表示特征点的大小，直线表示特征点的方向  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//namedWindow("SIFT1");
	//imshow("SIFT1", output_img);

	drawKeypoints(img2,     //输入图像  
		key_points2,      //特征点矢量  
		output_img,      //输出图像  
		Scalar::all(-1),      //绘制特征点的颜色，为随机  
							  //以特征点为中心画圆，圆的半径表示特征点的大小，直线表示特征点的方向  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//namedWindow("SIFT2");
	//imshow("SIFT2", output_img);


	//实例化暴力匹配器——BruteForceMatcher  
	BruteForceMatcher<L2<float>> matcher;
	//定义匹配器算子  
	vector<DMatch>matches;
	//实现描述符之间的匹配，得到算子matches  
	matcher.match(descriptors1, descriptors2, matches);

	//提取出前30个最佳匹配结果  
	if (matches.size() > 100)
	{
		std::nth_element(matches.begin(),     //匹配器算子的初始位置  
			matches.begin() + 99,     // 排序的数量  
			matches.end());       // 结束位置  
								  //剔除掉其余的匹配结果  
		matches.erase(matches.begin() + 100, matches.end());
	}


	//namedWindow("SIFT_matches");
	Mat img_matches;
	//在输出图像中绘制匹配结果  
	//drawMatches(img1, key_points1,         //第一幅图像和它的特征点  
	//	img2, key_points2,      //第二幅图像和它的特征点  
	//	matches,       //匹配器算子  
	//	img_matches,      //匹配输出图像  
	//	Scalar::all(-1));     //用直线连接两幅图像中的特征点  
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
		//这两句话的理解：R_keypoint1是要存储img01中能与img02匹配的特征点，
		//matches中存储了这些匹配点对的img01和img02的索引值
	}

	//坐标转换
	vector<Point2f>p01, p02;
	for (size_t i = 0; i<mathces.size(); i++)
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
	imshow("消除误匹配点后", img_RR_matches);

}