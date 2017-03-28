#include "SIFT.h"
#include "RANSAC.h"

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

	cout << "SIFT: "<<key_points1.size() << " " << key_points2.size() << "\n";

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
	//BruteForceMatcher<L2<float>> matcher;
	BFMatcher matcher(NORM_L1, true);
	//定义匹配器算子  
	vector<DMatch>matches;
	//实现描述符之间的匹配，得到算子matches  
	matcher.match(descriptors1, descriptors2, matches);

	cout<<"matches " << matches.size() << "\n";
	
	//提取出前100个最佳匹配结果  
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

