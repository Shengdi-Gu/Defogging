#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat myBlur(Mat a, int r) {
	int row = a.rows;
	int col = a.cols;
	Mat b = a.clone();
	int cols = col * 3;
	for (int i = 0; i < row; i++) {
		float* data1 = a.ptr<float>(i);
		float* data2 = b.ptr<float>(i);
		float sum = 0;
		float* temp = data1;
		for (int k = 0; k < 2 * r + 1; k++) {
			sum = sum + temp[0];
			temp += 3;
		}
		for (int j = 0; j < col; j++) {
			if (j > r && (col - j) > r) {
				float v = sum / (2 * r + 1);
				data2[0] = data2[1] = data2[2] = v;
				sum = sum - (data1 - 3 * r)[0];
				sum = sum + (data1 + 3 * r)[0];
			}
			else
			{
				data2[0] = data2[1] = data2[2] = sum / (2 * r + 1);
			}
			data1 += 3;
			data2 += 3;
		}
	}
	return b;

}

void main()
{
	int r;
	float qwl;
	float A = 0.0;
	string s;
	cout << "请输入图片名 " << endl;
	cin >> s;
	s = s + ".jpg";
	//Mat originalPicture=imread(s);	
	cout << "请输入滤波半径（建议10-25）" << endl;
	cin >> r;
	cout << "请输入去雾率（0.9-1）" << endl;
	cin >> qwl;
	//int count=0;
	double sum = 0;
	//while(count<100){
	//	count++;
	Mat originalPicture = imread(s);
	double time0 = static_cast<double>(getTickCount());
	int row = originalPicture.rows;
	int col = originalPicture.cols;
	int cols = col * 3;
	originalPicture.convertTo(originalPicture, CV_32FC3, 1 / 255.0);//原始图片归一化
	Mat grayPicture(row, col, CV_32FC3);//初始化灰度图
	Mat darkChannels(row, col, CV_32FC3);//初始化暗通道图
	Mat tranPicture(row, col, CV_32FC3);//初始化透射率图（trancmittivity）
	Mat finalPicture(row, col, CV_32FC3);//
	float* data1 = originalPicture.ptr<float>(0);
	float* data2 = grayPicture.ptr<float>(0);
	for (int i = 0; i < row*cols; i += 3)
	{
		data2[i] = data2[i + 1] = data2[i + 2] = (data1[i] < data1[i + 1] ? data1[i] : data1[i + 1]) < data1[i + 2] ? (data1[i] < data1[i + 1] ? data1[i] : data1[i + 1]) : data1[i + 2];
	}
	/*for (int i = 0; i < row*cols; i += 3) {
	data2[i] = data2[i + 1] = data2[i + 2] = data1[i] * 0.11 + data1[i + 1] * 0.59 + data1[i + 2] * 0.3;
	}*/
	Mat p;
	Mat ele = getStructuringElement(MORPH_RECT, Size(r * 2 + 1, r * 2 + 1));
	erode(grayPicture, p, ele);
	imshow("antong", p);
	//imshow("grayPicture", grayPicture);
	//导向滤波
	Mat meanI, meanP, corrI, corrIP, varI, covIP, meana, meanb, meanII, meanIP, a, b, q, out, out2;//导向滤波所需变量
	meanI = myBlur(originalPicture, r);
	meanP = myBlur(grayPicture, r);
	meanIP = myBlur(originalPicture.mul(grayPicture), r);
	covIP = meanIP - meanI.mul(meanP);
	meanII = myBlur(originalPicture.mul(originalPicture), r);
	varI = meanII - meanI.mul(meanI);
	a = covIP / (varI + 0.0001);
	b = meanP - a.mul(meanI);
	meana = myBlur(a, r);
	meanb = myBlur(b, r);


	darkChannels = meana.mul(originalPicture) + meanb;
	//darkChannels = grayPicture;
	//imshow("darkChannels", darkChannels);

	//求最大光度值
	//float* data3 = darkChannels.ptr<float>(0);
	//for (int i = 0; i < row*cols; i++)
	//{
	//	if (A < data3[i])A = data3[i];
	//}
	//cout << "A=" << A << endl;
	//if (A > 230 / 255.0)A = 230 / 255.0;
	A = 220 / 255.0;
	Mat temp(row, col, CV_32FC3, Scalar(1.0, 1.0, 1.0));
	tranPicture = temp - qwl*darkChannels;
	Mat temp2(row, col, CV_32FC3, Scalar(A, A, A));
	finalPicture = (originalPicture - temp2) / tranPicture + temp2;
	float* data3 = finalPicture.ptr<float>(0);
	for (int i = 0; i < row*cols; i++)
	{
		if (data3[0] > 1)data3[0] = 1;
		if (data3[0] < 0)data3[0] = 0;
	}

	time0 = ((double)getTickCount() - time0) / getTickFrequency();
	sum = sum + time0;
	cout << "用时=" << time0 << endl;
	imshow("原图", originalPicture);
	imshow("暗通道图", darkChannels);
	imshow("投射率图", tranPicture);
	imshow("去雾图像", finalPicture);
	waitKey(0);
}