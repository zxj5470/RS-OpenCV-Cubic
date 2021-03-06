// OpenCV_Double_Tri.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

Mat lena;
Mat* o;

double S(double x, double a = -0.5) {
	if (x < 0) x = -x;
	double value;
	if (x < 1.0) {
		value = (a + 2) * x * x * x - (a + 3) * x * x + 1;
	} else if (x < 2) {
		value = a * x * x * x - 5 * a * x * x + 8 * a * x - 4 * a;
	} else
		value = 0.0;
	return value;
}

/**
* @brief 三次卷积
*
* @param x 插值点列坐标
* @param y 插值点行坐标
* @param source 影像数据
* @param nWidth 影像数据宽度
* @param nHeight 影像数据高度
*/
double ResampleCubic(double x, double y, Mat& source) {
	int i = x;
	int j = y;
	//边界处理
	if ((i - 1) < 0 || (j - 1) < 0 || (j + 3) > source.rows || (i + 3) > source.cols)
		return 0;
	/*get adjacent 16 values*/
	double values[4][4];
	for (int r = j - 1, s = 0; r <= j + 2; r++, s++) {
		for (int c = i - 1, t = 0; c <= i + 2; c++, t++) {
			values[s][t] = source.at<uchar>(c, r);
		}
	}

	/*calc the coeff*/
	double u = x - i;//取小数部分
	double v = y - j;
	double A[4], C[4];
	for (int distance = 1, s = 0; distance >= -2; distance--, s++) {
		A[s] = S(u + distance);
		C[s] = S(v + distance);
	}

	double dfCubicValue = 0.0;
	for (int s = 0; s < 4; s++) {
		for (int t = 0; t < 4; t++) {
			dfCubicValue += values[s][t] * A[t] * C[s];
			//cout << values[s][t] << "," << A[t] << "," << C[s] << endl;
		}
	}

	return dfCubicValue;
}


void on_mouse1(int event, int x, int y, int flags, void*) {
	if (event == CV_EVENT_LBUTTONDOWN && flags)//鼠标左键按下事件发生    
	{
		CvPoint pt = cvPoint(x, y);//获取当前点的横纵坐标值  
		const int value = lena.at<uchar>(y, x);
		cout << "Old:" << pt.x << "," << pt.y << " : " << value << endl;
	}
}

void on_mouse2(int event, int x, int y, int flags, void*) {
	if (event == CV_EVENT_LBUTTONDOWN && flags)//鼠标左键按下事件发生    
	{
		CvPoint pt = cvPoint(x, y);//获取当前点的横纵坐标值  
		const int v2 = o->at<uchar>(y, x);
		cout << "New:" << pt.x << "," << pt.y << " : " << v2 << endl;

	}
}


int main() {
	lena = imread("lena.jpg", CV_LOAD_IMAGE_GRAYSCALE);//载入灰度图像
	const auto title = "title";
	const auto outTitle = "title2";
	double scale = 1.7;
	Mat outImg(lena.rows * scale, lena.cols * scale, CV_LOAD_IMAGE_GRAYSCALE);// 8位灰度图
	o = &outImg;
	// i j 对应屏幕h w
	for (int i = 0; i < outImg.rows; i++) {
		for (int j = 0; j < outImg.rows; j++) {
			double h = i / scale;//原图对应坐标
			double w = j / scale;
			uchar ret = (uchar)ResampleCubic(h, w, lena);
			outImg.at<uchar>(i, j) = ret;
		}
	}
	cout << outImg.rows << "," << outImg.cols << endl;
	namedWindow(title, 1);
	imshow(title, lena);
	cvSetMouseCallback(title, on_mouse1, nullptr);

	namedWindow(outTitle, 1);
	imshow(outTitle, outImg);
	cvSetMouseCallback(outTitle, on_mouse2, nullptr);
	waitKey();
	return 0;
}
