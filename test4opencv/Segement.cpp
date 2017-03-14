#include "Segement.h"

int paircompare(const pair<float, float> &p1, const pair<float, float> &p2)
//比较霍夫检测得到的线，按rho从小到大排序，用于sort的自定义排序
//rho=pair.first theta=pair.second
{
	return p1.first < p2.first;
}

void segement(Mat &src, Mat &dst)
//检测近似水平的直线，将间隔在一定范围的判定为书架层，分割层间的图像用于后续处理
//输入原图 通过reference输出分割图
{

	Mat imgCanny;
	GaussCanny(src, imgCanny, 3);//高斯滤波+canny边缘提取，滤波器大小Size(3,3)

	vector<pair<float, float>> horizontal_lines;//筛选出的水平线
	lineFilterHor(imgCanny, horizontal_lines, 150);//水平线筛选，霍夫阈值150

	if (horizontal_lines.size() > 100)//水平线太多，调整滤波系数
	{	
		//cout << "filter failed\n";
		GaussCanny(src, imgCanny, 7);//try again,滤波器大小Size(7,7)
		horizontal_lines.clear();//清空之前筛选的线
		lineFilterHor(imgCanny, horizontal_lines, 150);
	}
	if (horizontal_lines.size() > 100)//水平线还太多，没救了
	{
		cout << "filter failed\n";
		return;
	}


	sort(horizontal_lines.begin(), horizontal_lines.end(), paircompare);//将筛选出的水平线从上到下排序
	//cout << "sort done\n";

	//选出最上和最下边缘
	//cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
	pair<float, float> lastline = horizontal_lines.at(0);//上条线
	vector<pair<float, float>> edge;//边缘at(0)上边界 at(1)下边界 后续计算不区分
	//rho=pair.first theta=pair.second
	float dis = 0, delta = 0;//与上条线距离和夹角 
	int edgeline = 0;//检出书架层边线对数量
	for (unsigned int i = 0; i < horizontal_lines.size(); i++)
	{
		dis = horizontal_lines.at(i).first - lastline.first;
		delta = horizontal_lines.at(i).second - lastline.second;
		if ((dis > 10 && dis < 30) && (delta > -1.5* CV_PI / 180 && delta < 1.5* CV_PI / 180))
			//距离间隔在一定范围内（10-30pix）同时平行度良好（±1.5deg）认为是书架边缘
		{
			//float rho = horizontal_lines.at(i).first, theta = horizontal_lines.at(i).second;
			//drawHoughLine(rho, theta, imgCanny,0,255);//用于绘图
			if (edgeline < 2)
				//第一次检测到的作为上边缘，第二次下边缘，此后测到的覆盖下边缘（可能有问题）
			{
				edge.push_back(horizontal_lines.at(i));
			}
			else
			{
				edge.at(1) = horizontal_lines.at(i);
			}
			edgeline++;
		}
		lastline = horizontal_lines.at(i);
	}
	//resize(imgCanny, imgCanny, Size(0, 0), 0.5, 0.5);
	//imshow("seg1", imgCanny);
	if (edgeline == 1)//只有一条线将pair(0,PI/2)作边线
	{
		float zero = 0;
		edge.push_back(make_pair(zero,  CV_PI / 2));
	}


	if (edgeline > 0)//至少一个边缘
	{

		int width = src.cols;
		int height = src.rows;
		int range[4]{};
		calBorder(width, height, edge, range);//计算需截取部分
		//Mat imageROI;
		dst = src(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));//roi截取，分割完毕
		
	}
	
}

void spineSegement(Mat &src, vector<Mat> &dst)
//检测竖直线进行书脊分割，分割后进行校准，存入目标容器中
//输入单层书架图 通过reference输出分割容器
{
	Mat imgCanny;
	GaussCanny(src, imgCanny, 3);//高斯滤波+canny边缘提取，滤波器大小Size(3,3)

	vector<pair<float, float>> vertical_lines;//筛选出的竖直线
	lineFilterVer(imgCanny, vertical_lines, 110);//筛选竖直线，阈值110

	//imshow("canny", imgCanny);

	sort(vertical_lines.begin(), vertical_lines.end(), paircompare);//有问题的排序，接近垂直的线有问题
	//cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
	pair<float, float> lastline = vertical_lines.at(0);//上条线

	vector<pair<float, float>> edge;//垂直边缘，分别表示左右
	//rho=pair.first theta=pair.second
	float zero = 0;
	edge.push_back(make_pair(zero, zero));
	edge.push_back(make_pair(zero, zero));//事先定好大小

	float dis = 0, delta = 0;//与上条线距离和夹角
	int edgeline = 0;//分割书脊边线对数量
	int width = src.cols, height = src.rows;
	int crosspoint[2];//两条线的交点
	int range[4]{};//分割大小
	Mat imagesei;//分割后的书脊(未校准)
	//char name[10];
	Mat imageper;//分割后的书脊(校准)
	for (unsigned int i = 0; i < vertical_lines.size(); i++)
	{
		//垂直线的rho有正负，计算在上边缘的投影距离
		calCross(vertical_lines.at(i).first, vertical_lines.at(i).second, 0, CV_PI / 2, crosspoint);
		dis = crosspoint[0];//上边缘交点
		calCross(lastline.first, lastline.second, 0, CV_PI / 2, crosspoint);
		dis = dis - crosspoint[0];//另一个上边缘交点

		if (dis < 0)//保证大于0
			dis = -dis;

		delta = vertical_lines.at(i).second - lastline.second;//角度会超过180
		if (delta > CV_PI / 2)//修正为±90deg之内
			delta = delta - CV_PI;
		if (delta < -CV_PI / 2)
			delta = delta + CV_PI;

		if ((dis > 5 && dis < 70) && (delta > -2.5* CV_PI / 180 && delta < 2.5* CV_PI / 180))
			//距离间隔在一定范围内(5-70pix)同时平行度良好（±2.5deg）认为是书边缘
		{
			//float rho = vertical_lines.at(i).first, theta = vertical_lines.at(i).second;
			//drawHoughLine(rho, theta, imgCanny);//绘图用
			edge.at(0) = lastline;
			edge.at(1) = vertical_lines.at(i);

			calBorder(width, height, edge, range);//计算需截取部分
			imagesei = src(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));
			//截取图像，其中range[0]，range[1]为截取的坐标原点，后校准要用
			//sprintf_s(name, "%d", i);			
			//imshow(name, imagesei);

			calPerspect(width, height, range[0], range[1], edge, imagesei, imageper);//透视校准计算，变成正视图
			//imshow("1", imageper);
			dst.push_back(imageper.clone());//存入目标容器，分割书脊结束

			edgeline++;
		}
		lastline = vertical_lines.at(i);
	}

	//imshow("seg2", imgCanny);
}

void drawHoughLine(float rho , float theta , Mat &obj, double R,double G,double B)
//在指定图上画极坐标线
//输入rho，theta 通过引用输出 输入颜色(默认红色)
{
Point pt1, pt2;//随便找直线上两个点
double a = cos(theta), b = sin(theta);
double x0 = a*rho, y0 = b*rho;
pt1.x = cvRound(x0 + 1000 * (-b));
pt1.y = cvRound(y0 + 1000 * (a));
pt2.x = cvRound(x0 - 1000 * (-b));
pt2.y = cvRound(y0 - 1000 * (a));
line(obj, pt1, pt2, Scalar(B, G, R), 1, CV_AA);//画线
}

void GaussCanny(Mat &src, Mat &dst,int psize)
//高斯滤波+canny边缘提取
//输入原图、引用目标、高斯模板尺寸
{
	Mat imgGaussBlur;
	GaussianBlur(src, imgGaussBlur, Size(psize, psize), 0, 0);
	//imshow("Gaussshelve", imgGaussBlur);
	Canny(imgGaussBlur, dst, 50, 200, 3);//参数大多数情况下不用改
	//imshow("Canny", imgCanny);
}

void lineFilterHor(Mat &src, vector<pair<float, float>> &horizontal_lines,int threshold)
//霍夫检测直线，筛选水平线
//输入边缘图(gray) 输出目标容器 输入阈值
{
	vector<Vec2f> lines;//霍夫直线检测 
	HoughLines(src, lines, 1, CV_PI / 180, threshold, 0, 0);//threshold=150
	//cout << "find " << lines.size() << " lines\n";
	
	//cvtColor(src, src, CV_GRAY2BGR);
	//vector<pair<float, float>> horizontal_lines;//筛选线数组
	pair<float, float> linepair;
	//rho=pair.first theta=pair.second
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		if ((theta<100 * CV_PI / 180 && theta>80 * CV_PI / 180))//近似水平，80deg<theta<100deg
		{
			linepair = make_pair(rho, theta);
			horizontal_lines.push_back(linepair);//记录线
			//drawHoughLine(rho, theta, src);//画线用
		}
	}
	cout << "filter HorLine " << horizontal_lines.size() << " lines\n";
}

void lineFilterVer(Mat &src, vector<pair<float, float>> &vertical_lines, int threshold)
//霍夫检测直线，筛选垂直线
//输入边缘图(gray) 输出目标容器 输入阈值
{
	vector<Vec2f> lines;//霍夫直线检测 
	HoughLines(src, lines, 1, CV_PI / 180, threshold, 0, 0);//threshold=110 垂直线短一些
	//cout << "find " << lines.size() << " lines\n";

	//cvtColor(src, src, CV_GRAY2BGR);

	pair<float, float> linepair;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		if ((theta<10 * CV_PI / 180 || theta>170 * CV_PI / 180))//近似垂直theta<10deg || theta>170deg
		{
			linepair = make_pair(rho, theta);
			vertical_lines.push_back(linepair);//记录线
			//drawHoughLine(rho, theta, src);
		}
	}
	cout << "filter VerLine " << vertical_lines.size() << " lines\n";

}

void calCross(double rho1, double theta1, double rho2, double theta2, int* crosspoint)
//计算两直线交点
//输入两直线极坐标参数 输出在crosspoint数组中，[1]为x，[2]为y
{
	if (theta1 == theta2)
	{
		crosspoint[0] = -1;
		crosspoint[1] = -1;
		return;//平行线输出(-1,-1)不影响
	}
	else
	{
		double det = cos(theta1)*sin(theta2) - cos(theta2)*sin(theta1);
		crosspoint[0] = static_cast <int>(round((sin(theta2)*rho1 - sin(theta1)*rho2) / det));
		crosspoint[1] = static_cast <int>(round((-cos(theta2)*rho1 + cos(theta1)*rho2) / det));
		return;//计算交点
	}
}


void calBorder(int width,int height, vector<pair<float, float>> &edge,int* range)
//计算截取边界，求两条目标直线与四边线的交点，取能完全包含两直线的范围
//输入图像宽高 输入两直线 输出范围的对角两点(x0,y0,x1,y1)
{
	int crosspoint[2];
	vector<pair<float, float>> border;//确定边界
	int minx = width, miny = height, maxx = 0, maxy = 0;//存范围，默认最大
	border.push_back(make_pair(0, 0));
	border.push_back(make_pair(0, CV_PI / 2));
	border.push_back(make_pair(width, 0));
	border.push_back(make_pair(height, CV_PI / 2));
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			calCross(edge.at(j).first, edge.at(j).second, border.at(i).first, border.at(i).second, crosspoint);
			//求交点
			if (!(crosspoint[0]<0 || crosspoint[1]<0 || crosspoint[0]>width || crosspoint[1]>height))//超出图像不考虑
			{
				if (crosspoint[0] < minx)
					minx = crosspoint[0];
				if (crosspoint[1] < miny)
					miny = crosspoint[1];
				if (crosspoint[0] > maxx)
					maxx = crosspoint[0];
				if (crosspoint[1] > maxy)
					maxy = crosspoint[1];
			}

		}
	}
	range[0] = minx; range[1] = miny;
	range[2] = maxx; range[3] = maxy;
	
}

void calPerspect(int width, int height,int orix, int oriy, vector<pair<float, float>> &edge, Mat& src,Mat& dst)
//计算透视校准
//输入书架层图宽高，输入截取书本原点，输入两条边界，书架层图像 输出书脊图  
{
	int crosspoint[2];
	vector<pair<float, float>> border;
	Point2f srcTri[4], dstTri[4];//四对匹配点
	border.push_back(make_pair(0, CV_PI / 2));//上下边
	border.push_back(make_pair(height, CV_PI / 2));
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			calCross(edge.at(j).first, edge.at(j).second, border.at(i).first, border.at(i).second, crosspoint);
			if (!(crosspoint[0]<0 || crosspoint[1]<0 || crosspoint[0]>width || crosspoint[1]>height))
			{
				srcTri[i + j * 2].x = crosspoint[0]-orix;//0 2
				srcTri[i + j * 2].y = crosspoint[1]-oriy;//1 3
				//理想状况下0位于左上，1左下，2右上，3右下
			}

		}
	}
	int dstWidth = 0, dstHeight = 0;
	if (srcTri[0].x > srcTri[2].x)
	{
		//匹配到理想情况
		//分别交换0 2,1 3
		Point2f temp;
		temp = srcTri[0];		srcTri[0] = srcTri[2];		srcTri[2] = temp;
		temp = srcTri[1];		srcTri[1] = srcTri[3];		srcTri[3] = temp;
	}

	if((srcTri[2].x - srcTri[0].x)>(srcTri[3].x - srcTri[1].x))//目标宽高取原位置的宽高中较大的
		dstWidth = srcTri[2].x - srcTri[0].x;
	else
		dstWidth = srcTri[3].x - srcTri[1].x;
	if ((srcTri[1].y - srcTri[0].y)>(srcTri[3].y - srcTri[2].y))
		dstHeight = srcTri[1].y - srcTri[0].y;
	else
		dstHeight = srcTri[3].y - srcTri[2].y;

	dstTri[0].x = 0; dstTri[0].y = 0;//对应srctri中的位置
	dstTri[1].x = 0; dstTri[1].y = dstHeight;
	dstTri[2].x = dstWidth; dstTri[2].y = 0;
	dstTri[3].x = dstWidth; dstTri[3].y = dstHeight;
	Mat warp_mat;
	warp_mat = Mat::zeros(3, 3, CV_32FC2);
	warp_mat=getPerspectiveTransform(srcTri, dstTri);//计算变换矩阵
	warpPerspective(src, dst, warp_mat, Size(dstWidth, dstHeight));//截取变换图并输出
}

