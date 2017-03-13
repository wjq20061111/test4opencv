#include "Segement.h"

int paircompare(const pair<float, float> &p1, const pair<float, float> &p2)
{
	return p1.first < p2.first;
}

void segement(Mat &src, Mat &dst)
{

	Mat imgCanny;
	GaussCanny(src, imgCanny, 3);

	//vector<Vec4i> lines;//概率霍夫直线检测 
	//HoughLinesP(imgCanny, lines, 1, CV_PI / 180, 80, 30, 10);
	//cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
	//for (size_t i = 0; i < lines.size(); i++)//显示直线
	//{
	//	Vec4i l = lines[i];
	//	//if(l[0]- l[2]>-20 && l[0] - l[2]<20)
	//	line(imgCanny, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
	//}
	//imshow("line", imgCanny);

	vector<pair<float, float>> horizontal_lines;//筛选线数组
	lineFilterHor(imgCanny, horizontal_lines, 150);

	if (horizontal_lines.size() > 100)
	{	
		cout << "filter failed\n";
		GaussCanny(src, imgCanny, 7);//try again
		horizontal_lines.clear();//清空筛选线数组
		lineFilterHor(imgCanny, horizontal_lines, 150);
	}
	if (horizontal_lines.size() > 100)
	{
		cout << "filter failed again\n";
		return;
	}


	sort(horizontal_lines.begin(), horizontal_lines.end(), paircompare);//从上到下排序
	//cout << "sort done\n";

	//筛选最上和最下边缘
	pair<float, float> lastline = horizontal_lines.at(0);//上条线
	vector<pair<float, float>> edge;
	float dis = 0, delta = 0;//与上条线距离和夹角
	int edgeline = 0;
	for (unsigned int i = 0; i < horizontal_lines.size(); i++)
	{
		dis = horizontal_lines.at(i).first - lastline.first;
		delta = horizontal_lines.at(i).second - lastline.second;
		if ((dis > 10 && dis < 30) && (delta > -1.5* CV_PI / 180 && delta < 1.5* CV_PI / 180))
			//距离间隔在一定范围内同时平行度良好认为是书架边缘
		{
			//float rho = horizontal_lines.at(i).first, theta = horizontal_lines.at(i).second;
			//drawHoughLine(rho, theta, imgCanny);
			if (edgeline < 2)
				//第一次检测到的作为上边缘，最后一次测到的作为下边缘
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
	if (edgeline == 1)
	{
		float zero = 0;
		edge.push_back(make_pair(zero, zero));
	}

	if (edgeline > 0)//至少一个边缘
	{

		int width = src.cols;
		int height = src.rows;
		int range[4]{};
		calBorder(width, height, edge, range);
		//Mat imageROI;
		dst = src(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));
		
	}
	
}


void drawHoughLine(float rho , float theta , Mat &obj)
{
Point pt1, pt2;//显示直线
double a = cos(theta), b = sin(theta);
double x0 = a*rho, y0 = b*rho;
pt1.x = cvRound(x0 + 1000 * (-b));
pt1.y = cvRound(y0 + 1000 * (a));
pt2.x = cvRound(x0 - 1000 * (-b));
pt2.y = cvRound(y0 - 1000 * (a));
line(obj, pt1, pt2, Scalar(55, 100, 195), 1, CV_AA);
}

void GaussCanny(Mat &src, Mat &dst,int psize)
{
	Mat imgGaussBlur;
	GaussianBlur(src, imgGaussBlur, Size(psize, psize), 0, 0);
	//imshow("Gaussshelve", imgGaussBlur);
	Canny(imgGaussBlur, dst, 50, 200, 3);
	//imshow("Canny", imgCanny);
}

void lineFilterHor(Mat &src, vector<pair<float, float>> &horizontal_lines,int threshold)
{
	vector<Vec2f> lines;//霍夫直线检测 
	HoughLines(src, lines, 1, CV_PI / 180, threshold, 0, 0);//threshold=150
	cout << "find " << lines.size() << " lines\n";
	
	//cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
	//vector<pair<float, float>> horizontal_lines;//筛选线数组
	pair<float, float> linepair;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		if ((theta<100 * CV_PI / 180 && theta>80 * CV_PI / 180))//近似水平
		{
			linepair = make_pair(rho, theta);
			horizontal_lines.push_back(linepair);//记录线
			//drawHoughLine(rho, theta, imgCanny);
		}
	}
	cout << "filter HorLine " << horizontal_lines.size() << " lines\n";
}

void lineFilterVer(Mat &src, vector<pair<float, float>> &vertical_lines, int threshold)
{
	vector<Vec2f> lines;//霍夫直线检测 
	HoughLines(src, lines, 1, CV_PI / 180, threshold, 0, 0);//threshold=110
	cout << "find " << lines.size() << " lines\n";

	//cvtColor(src, src, CV_GRAY2BGR);

	pair<float, float> linepair;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		if ((theta<10 * CV_PI / 180 || theta>170 * CV_PI / 180))//近似垂直
		{
			linepair = make_pair(rho, theta);
			vertical_lines.push_back(linepair);//记录线
			//drawHoughLine(rho, theta, src);
		}
	}
	cout << "filter VerLine " << vertical_lines.size() << " lines\n";

}

void calCross(double rho1, double theta1, double rho2, double theta2, int* crosspoint)
{
	if (theta1 == theta2)
	{
		crosspoint[0] = -1;
		crosspoint[1] = -1;
		return;
	}
	else
	{
		double det = cos(theta1)*sin(theta2) - cos(theta2)*sin(theta1);
		crosspoint[0] = static_cast <int>(round((sin(theta2)*rho1 - sin(theta1)*rho2) / det));
		crosspoint[1] = static_cast <int>(round((-cos(theta2)*rho1 + cos(theta1)*rho2) / det));
		return;
	}
}


void calBorder(int width,int height, vector<pair<float, float>> &edge,int* range)
{
	int crosspoint[2];
	vector<pair<float, float>> border;
	int minx = width, miny = height, maxx = 0, maxy = 0;
	border.push_back(make_pair(0, 0));
	border.push_back(make_pair(0, CV_PI / 2));
	border.push_back(make_pair(width, 0));
	border.push_back(make_pair(height, CV_PI / 2));
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			calCross(edge.at(j).first, edge.at(j).second, border.at(i).first, border.at(i).second, crosspoint);
			if (!(crosspoint[0]<0 || crosspoint[1]<0 || crosspoint[0]>width || crosspoint[1]>height))
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
{
	int crosspoint[2];
	vector<pair<float, float>> border;
	Point2f srcTri[4], dstTri[4];
	border.push_back(make_pair(0, CV_PI / 2));
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
			}

		}
	}
	int dstWidth = 0, dstHeight = 0;
	if (srcTri[0].x > srcTri[2].x)
	{
		//交换0 2,1 3
		Point2f temp;
		temp = srcTri[0];		srcTri[0] = srcTri[2];		srcTri[2] = temp;
		temp = srcTri[1];		srcTri[1] = srcTri[3];		srcTri[3] = temp;
	}
	if((srcTri[2].x - srcTri[0].x)>(srcTri[3].x - srcTri[1].x))
		dstWidth = srcTri[2].x - srcTri[0].x;
	else
		dstWidth = srcTri[3].x - srcTri[1].x;
	if ((srcTri[1].y - srcTri[0].y)>(srcTri[3].y - srcTri[2].y))
		dstHeight = srcTri[1].y - srcTri[0].y;
	else
		dstHeight = srcTri[3].y - srcTri[2].y;
	dstTri[0].x = 0; dstTri[0].y = 0;
	dstTri[1].x = 0; dstTri[1].y = dstHeight;
	dstTri[2].x = dstWidth; dstTri[2].y = 0;
	dstTri[3].x = dstWidth; dstTri[3].y = dstHeight;
	Mat warp_mat;
	warp_mat = Mat::zeros(3, 3, CV_32FC2);
	warp_mat=getPerspectiveTransform(srcTri, dstTri);
	warpPerspective(src, dst, warp_mat, Size(dstWidth, dstHeight));
}

