#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include"opencv2/imgproc/imgproc.hpp" 
#include <cv.h>
#include <iostream>  

using namespace std;
using namespace cv;


int mode;
Mat g_srcImage;
Mat out1,out2;
vector<Mat> color;
int main()
{
	//输入转换模式
	//cin >> mode;

	color.resize(3);
	
	//读入用户提供的图像  
	g_srcImage = imread("tests3.jpg");
	if (!g_srcImage.data) { printf("读取图片错误~！\n"); return false; }
	
	Mat bk(g_srcImage.size(),CV_8U,Scalar(200));
	
	//namedWindow("【效果图窗口】", 1);
	namedWindow("【原图窗口】", 1);
	imshow("【原图窗口】", g_srcImage);

	cvtColor(g_srcImage,out1, CV_BGR2HSV);

	split(out1,color);

	
	//color[1] = bk;
	color[2] = bk;

	merge(color, out1);

	cvtColor(out1, out2, CV_HSV2BGR);
	//namedWindow("【h】", 1);
	//imshow("【h】", color[0]);
	//namedWindow("【s】", 1);
	//imshow("【s】", color[1]);
	//namedWindow("【v】", 1);
	//imshow("【v】", color[2]);
	namedWindow("【out2】", 1);
	imshow("【out2】", out2);



	waitKey(0);
	return 0;
}

