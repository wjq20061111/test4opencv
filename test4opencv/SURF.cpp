#include "SURF.h"
#include "RANSAC.h"

void SURFCompare(Mat& src,Mat& target)
{
	SURF surf1(300, 4, 2);
	SURF surf2(300, 4, 2);

	vector<KeyPoint> key_points1, key_points2;

	Mat descriptors1, descriptors2, mascara;

	surf1(src, mascara, key_points1, descriptors1);
	surf2(target, mascara, key_points2, descriptors2);

	cout << "SURF: " << key_points1.size() << " " << key_points2.size() << "\n";

	Mat output_img;

	drawKeypoints(src,     //输入图像  
		key_points1,      //特征点矢量  
		output_img,      //输出图像  
		Scalar::all(-1),      //绘制特征点的颜色，为随机  
							  //以特征点为中心画圆，圆的半径表示特征点的大小，直线表示特征点的方向  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//namedWindow("SURF1");
	//imshow("SURF1", output_img);

	drawKeypoints(target,     //输入图像  
		key_points2,      //特征点矢量  
		output_img,      //输出图像  
		Scalar::all(-1),      //绘制特征点的颜色，为随机  
							  //以特征点为中心画圆，圆的半径表示特征点的大小，直线表示特征点的方向  
		DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//namedWindow("SURF2");
	//imshow("SURF2", output_img);

	BFMatcher matcher(NORM_L2, true);
	//定义匹配器算子  
	vector<DMatch>matches;
	//实现描述符之间的匹配，得到算子matches  
	matcher.match(descriptors1, descriptors2, matches);

	cout << "matches " << matches.size() << "\n";

	mRANSAC(src, target, matches, key_points1, key_points2);
}

