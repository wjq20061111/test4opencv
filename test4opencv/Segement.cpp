#include "Segement.h"

int paircompare(const pair<float, float> &p1, const pair<float, float> &p2)
//�Ƚϻ�����õ����ߣ���rho��С������������sort���Զ�������
//rho=pair.first theta=pair.second
{
	return p1.first < p2.first;
}

void segement(Mat &src, Mat &dst)
//������ˮƽ��ֱ�ߣ��������һ����Χ���ж�Ϊ��ܲ㣬�ָ����ͼ�����ں�������
//����ԭͼ ͨ��reference����ָ�ͼ
{

	Mat imgCanny;
	GaussCanny(src, imgCanny, 3);//��˹�˲�+canny��Ե��ȡ���˲�����СSize(3,3)

	vector<pair<float, float>> horizontal_lines;//ɸѡ����ˮƽ��
	lineFilterHor(imgCanny, horizontal_lines, 150);//ˮƽ��ɸѡ��������ֵ150

	if (horizontal_lines.size() > 100)//ˮƽ��̫�࣬�����˲�ϵ��
	{	
		//cout << "filter failed\n";
		GaussCanny(src, imgCanny, 7);//try again,�˲�����СSize(7,7)
		horizontal_lines.clear();//���֮ǰɸѡ����
		lineFilterHor(imgCanny, horizontal_lines, 150);
	}
	if (horizontal_lines.size() > 100)//ˮƽ�߻�̫�࣬û����
	{
		cout << "filter failed\n";
		return;
	}


	sort(horizontal_lines.begin(), horizontal_lines.end(), paircompare);//��ɸѡ����ˮƽ�ߴ��ϵ�������
	//cout << "sort done\n";

	//ѡ�����Ϻ����±�Ե
	//cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
	pair<float, float> lastline = horizontal_lines.at(0);//������
	vector<pair<float, float>> edge;//��Եat(0)�ϱ߽� at(1)�±߽� �������㲻����
	//rho=pair.first theta=pair.second
	float dis = 0, delta = 0;//�������߾���ͼн� 
	int edgeline = 0;//�����ܲ���߶�����
	for (unsigned int i = 0; i < horizontal_lines.size(); i++)
	{
		dis = horizontal_lines.at(i).first - lastline.first;
		delta = horizontal_lines.at(i).second - lastline.second;
		if ((dis > 10 && dis < 30) && (delta > -1.5* CV_PI / 180 && delta < 1.5* CV_PI / 180))
			//��������һ����Χ�ڣ�10-30pix��ͬʱƽ�ж����ã���1.5deg����Ϊ����ܱ�Ե
		{
			//float rho = horizontal_lines.at(i).first, theta = horizontal_lines.at(i).second;
			//drawHoughLine(rho, theta, imgCanny,0,255);//���ڻ�ͼ
			if (edgeline < 2)
				//��һ�μ�⵽����Ϊ�ϱ�Ե���ڶ����±�Ե���˺�⵽�ĸ����±�Ե�����������⣩
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
	if (edgeline == 1)//ֻ��һ���߽�pair(0,PI/2)������
	{
		float zero = 0;
		edge.push_back(make_pair(zero,  CV_PI / 2));
	}


	if (edgeline > 0)//����һ����Ե
	{

		int width = src.cols;
		int height = src.rows;
		int range[4]{};
		calBorder(width, height, edge, range);//�������ȡ����
		//Mat imageROI;
		dst = src(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));//roi��ȡ���ָ����
		
	}
	
}

void spineSegement(Mat &src, vector<Mat> &dst)
//�����ֱ�߽����鼹�ָ�ָ�����У׼������Ŀ��������
//���뵥�����ͼ ͨ��reference����ָ�����
{
	Mat imgCanny;
	GaussCanny(src, imgCanny, 3);//��˹�˲�+canny��Ե��ȡ���˲�����СSize(3,3)

	vector<pair<float, float>> vertical_lines;//ɸѡ������ֱ��
	lineFilterVer(imgCanny, vertical_lines, 110);//ɸѡ��ֱ�ߣ���ֵ110

	//imshow("canny", imgCanny);

	sort(vertical_lines.begin(), vertical_lines.end(), paircompare);//����������򣬽ӽ���ֱ����������
	//cvtColor(imgCanny, imgCanny, CV_GRAY2BGR);
	pair<float, float> lastline = vertical_lines.at(0);//������

	vector<pair<float, float>> edge;//��ֱ��Ե���ֱ��ʾ����
	//rho=pair.first theta=pair.second
	float zero = 0;
	edge.push_back(make_pair(zero, zero));
	edge.push_back(make_pair(zero, zero));//���ȶ��ô�С

	float dis = 0, delta = 0;//�������߾���ͼн�
	int edgeline = 0;//�ָ��鼹���߶�����
	int width = src.cols, height = src.rows;
	int crosspoint[2];//�����ߵĽ���
	int range[4]{};//�ָ��С
	Mat imagesei;//�ָ����鼹(δУ׼)
	//char name[10];
	Mat imageper;//�ָ����鼹(У׼)
	for (unsigned int i = 0; i < vertical_lines.size(); i++)
	{
		//��ֱ�ߵ�rho���������������ϱ�Ե��ͶӰ����
		calCross(vertical_lines.at(i).first, vertical_lines.at(i).second, 0, CV_PI / 2, crosspoint);
		dis = crosspoint[0];//�ϱ�Ե����
		calCross(lastline.first, lastline.second, 0, CV_PI / 2, crosspoint);
		dis = dis - crosspoint[0];//��һ���ϱ�Ե����

		if (dis < 0)//��֤����0
			dis = -dis;

		delta = vertical_lines.at(i).second - lastline.second;//�ǶȻᳬ��180
		if (delta > CV_PI / 2)//����Ϊ��90deg֮��
			delta = delta - CV_PI;
		if (delta < -CV_PI / 2)
			delta = delta + CV_PI;

		if ((dis > 5 && dis < 70) && (delta > -2.5* CV_PI / 180 && delta < 2.5* CV_PI / 180))
			//��������һ����Χ��(5-70pix)ͬʱƽ�ж����ã���2.5deg����Ϊ�����Ե
		{
			//float rho = vertical_lines.at(i).first, theta = vertical_lines.at(i).second;
			//drawHoughLine(rho, theta, imgCanny);//��ͼ��
			edge.at(0) = lastline;
			edge.at(1) = vertical_lines.at(i);

			calBorder(width, height, edge, range);//�������ȡ����
			imagesei = src(Rect(range[0], range[1], range[2] - range[0], range[3] - range[1]));
			//��ȡͼ������range[0]��range[1]Ϊ��ȡ������ԭ�㣬��У׼Ҫ��
			//sprintf_s(name, "%d", i);			
			//imshow(name, imagesei);

			calPerspect(width, height, range[0], range[1], edge, imagesei, imageper);//͸��У׼���㣬�������ͼ
			//imshow("1", imageper);
			dst.push_back(imageper.clone());//����Ŀ���������ָ��鼹����

			edgeline++;
		}
		lastline = vertical_lines.at(i);
	}

	//imshow("seg2", imgCanny);
}

void drawHoughLine(float rho , float theta , Mat &obj, double R,double G,double B)
//��ָ��ͼ�ϻ���������
//����rho��theta ͨ��������� ������ɫ(Ĭ�Ϻ�ɫ)
{
Point pt1, pt2;//�����ֱ����������
double a = cos(theta), b = sin(theta);
double x0 = a*rho, y0 = b*rho;
pt1.x = cvRound(x0 + 1000 * (-b));
pt1.y = cvRound(y0 + 1000 * (a));
pt2.x = cvRound(x0 - 1000 * (-b));
pt2.y = cvRound(y0 - 1000 * (a));
line(obj, pt1, pt2, Scalar(B, G, R), 1, CV_AA);//����
}

void GaussCanny(Mat &src, Mat &dst,int psize)
//��˹�˲�+canny��Ե��ȡ
//����ԭͼ������Ŀ�ꡢ��˹ģ��ߴ�
{
	Mat imgGaussBlur;
	GaussianBlur(src, imgGaussBlur, Size(psize, psize), 0, 0);
	//imshow("Gaussshelve", imgGaussBlur);
	Canny(imgGaussBlur, dst, 50, 200, 3);//�������������²��ø�
	//imshow("Canny", imgCanny);
}

void lineFilterHor(Mat &src, vector<pair<float, float>> &horizontal_lines,int threshold)
//������ֱ�ߣ�ɸѡˮƽ��
//�����Եͼ(gray) ���Ŀ������ ������ֵ
{
	vector<Vec2f> lines;//����ֱ�߼�� 
	HoughLines(src, lines, 1, CV_PI / 180, threshold, 0, 0);//threshold=150
	//cout << "find " << lines.size() << " lines\n";
	
	//cvtColor(src, src, CV_GRAY2BGR);
	//vector<pair<float, float>> horizontal_lines;//ɸѡ������
	pair<float, float> linepair;
	//rho=pair.first theta=pair.second
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		if ((theta<100 * CV_PI / 180 && theta>80 * CV_PI / 180))//����ˮƽ��80deg<theta<100deg
		{
			linepair = make_pair(rho, theta);
			horizontal_lines.push_back(linepair);//��¼��
			//drawHoughLine(rho, theta, src);//������
		}
	}
	cout << "filter HorLine " << horizontal_lines.size() << " lines\n";
}

void lineFilterVer(Mat &src, vector<pair<float, float>> &vertical_lines, int threshold)
//������ֱ�ߣ�ɸѡ��ֱ��
//�����Եͼ(gray) ���Ŀ������ ������ֵ
{
	vector<Vec2f> lines;//����ֱ�߼�� 
	HoughLines(src, lines, 1, CV_PI / 180, threshold, 0, 0);//threshold=110 ��ֱ�߶�һЩ
	//cout << "find " << lines.size() << " lines\n";

	//cvtColor(src, src, CV_GRAY2BGR);

	pair<float, float> linepair;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		if ((theta<10 * CV_PI / 180 || theta>170 * CV_PI / 180))//���ƴ�ֱtheta<10deg || theta>170deg
		{
			linepair = make_pair(rho, theta);
			vertical_lines.push_back(linepair);//��¼��
			//drawHoughLine(rho, theta, src);
		}
	}
	cout << "filter VerLine " << vertical_lines.size() << " lines\n";

}

void calCross(double rho1, double theta1, double rho2, double theta2, int* crosspoint)
//������ֱ�߽���
//������ֱ�߼�������� �����crosspoint�����У�[1]Ϊx��[2]Ϊy
{
	if (theta1 == theta2)
	{
		crosspoint[0] = -1;
		crosspoint[1] = -1;
		return;//ƽ�������(-1,-1)��Ӱ��
	}
	else
	{
		double det = cos(theta1)*sin(theta2) - cos(theta2)*sin(theta1);
		crosspoint[0] = static_cast <int>(round((sin(theta2)*rho1 - sin(theta1)*rho2) / det));
		crosspoint[1] = static_cast <int>(round((-cos(theta2)*rho1 + cos(theta1)*rho2) / det));
		return;//���㽻��
	}
}


void calBorder(int width,int height, vector<pair<float, float>> &edge,int* range)
//�����ȡ�߽磬������Ŀ��ֱ�����ı��ߵĽ��㣬ȡ����ȫ������ֱ�ߵķ�Χ
//����ͼ���� ������ֱ�� �����Χ�ĶԽ�����(x0,y0,x1,y1)
{
	int crosspoint[2];
	vector<pair<float, float>> border;//ȷ���߽�
	int minx = width, miny = height, maxx = 0, maxy = 0;//�淶Χ��Ĭ�����
	border.push_back(make_pair(0, 0));
	border.push_back(make_pair(0, CV_PI / 2));
	border.push_back(make_pair(width, 0));
	border.push_back(make_pair(height, CV_PI / 2));
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			calCross(edge.at(j).first, edge.at(j).second, border.at(i).first, border.at(i).second, crosspoint);
			//�󽻵�
			if (!(crosspoint[0]<0 || crosspoint[1]<0 || crosspoint[0]>width || crosspoint[1]>height))//����ͼ�񲻿���
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
//����͸��У׼
//������ܲ�ͼ��ߣ������ȡ�鱾ԭ�㣬���������߽磬��ܲ�ͼ�� ����鼹ͼ  
{
	int crosspoint[2];
	vector<pair<float, float>> border;
	Point2f srcTri[4], dstTri[4];//�Ķ�ƥ���
	border.push_back(make_pair(0, CV_PI / 2));//���±�
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
				//����״����0λ�����ϣ�1���£�2���ϣ�3����
			}

		}
	}
	int dstWidth = 0, dstHeight = 0;
	if (srcTri[0].x > srcTri[2].x)
	{
		//ƥ�䵽�������
		//�ֱ𽻻�0 2,1 3
		Point2f temp;
		temp = srcTri[0];		srcTri[0] = srcTri[2];		srcTri[2] = temp;
		temp = srcTri[1];		srcTri[1] = srcTri[3];		srcTri[3] = temp;
	}

	if((srcTri[2].x - srcTri[0].x)>(srcTri[3].x - srcTri[1].x))//Ŀ����ȡԭλ�õĿ���нϴ��
		dstWidth = srcTri[2].x - srcTri[0].x;
	else
		dstWidth = srcTri[3].x - srcTri[1].x;
	if ((srcTri[1].y - srcTri[0].y)>(srcTri[3].y - srcTri[2].y))
		dstHeight = srcTri[1].y - srcTri[0].y;
	else
		dstHeight = srcTri[3].y - srcTri[2].y;

	dstTri[0].x = 0; dstTri[0].y = 0;//��Ӧsrctri�е�λ��
	dstTri[1].x = 0; dstTri[1].y = dstHeight;
	dstTri[2].x = dstWidth; dstTri[2].y = 0;
	dstTri[3].x = dstWidth; dstTri[3].y = dstHeight;
	Mat warp_mat;
	warp_mat = Mat::zeros(3, 3, CV_32FC2);
	warp_mat=getPerspectiveTransform(srcTri, dstTri);//����任����
	warpPerspective(src, dst, warp_mat, Size(dstWidth, dstHeight));//��ȡ�任ͼ�����
}

