#include "Contours.h"
#include <stdlib.h>
#include <stdio.h>
#include "SearchRelations.h"
Contours::Contours()
{
	// �������� ��������
	image = cvLoadImage("../3.png", CV_LOAD_IMAGE_ANYCOLOR);

	assert(image != 0);
	/*
	// ������ ������������� ��������
	gray = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	bin = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	// ���������
	dst = cvCloneImage(image);
	// ���� ��� ����������� ��������
	cvNamedWindow("original");
	cvNamedWindow("contours");
	
	// ����������� � �������� ������
	cvCvtColor(image, gray, CV_RGB2GRAY);

	// ����������� � ��������
	cvInRangeS(gray, cvScalar(40), cvScalar(150), bin); // atoi(argv[2])

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = 0;

	// ������� �������
	int contoursCont = cvFindContours(bin, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));

	// �������� �������
	for (CvSeq* seq0 = contours; seq0 != 0; seq0 = seq0->h_next){
		cvDrawContours(dst, seq0, CV_RGB(0, 0, 0), CV_RGB(0, 0, 0), 0, -1, 8); // ������ ������
	}

	// ���������� ��������
	cvShowImage("original", image);
	cvShowImage("contours", dst);
	
	// ��� ������� �������
	cvWaitKey(0);
	*/
	findCircles(image);

	cvWaitKey(0);
}



Contours::~Contours()
{
	// ����������� �������
	cvReleaseImage(&image);
	cvReleaseImage(&gray);
	cvReleaseImage(&bin);
	//cvReleaseImage(&dst);
	// ������� ����
	cvDestroyAllWindows();
}

IplImage *Contours::findCircles(IplImage* _image)
{
	assert(_image != 0);

	cvNamedWindow("real");
	cvShowImage("real", image);

	Mat bin2;
	cvtColor((Mat)image, bin2, CV_RGB2GRAY);
	bin2 = bin2 < 200;
	IplImage *bin = new IplImage(bin2);

	cvNamedWindow("BAndW");
	cvShowImage("BAndW", bin);

	cvCanny(bin, bin, 50, 200);

	// ��������� ������ ��� ��������
	CvMemStorage* storage = cvCreateMemStorage(0);
	
	// ������� �������
	int contoursCont = cvFindContours(bin, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 0, 1);//������������ ��������
	
	assert(contours != 0);
	imageContour = cvCreateImage(cvGetSize(_image), IPL_DEPTH_8U, 1);
	cvDrawContours(imageContour, contours, cvScalar(0, 0, 200), cvScalar(200, 0, 0), 1, 1, 8);

	//int i = 0;
		
	/*
	for (CvSeq* current = contours; current != NULL; current = current->h_next){
		// ��������� ������� � �������� �������
		double area = fabs(cvContourArea(current));
		double perim = cvContourPerimeter(current);
		cvDrawContours(_image, current, cvScalar(0, 0, 200), cvScalar(200, 0, 0), 0, 1, 8);
		
				// ������� �������.
				if (current->v_next != nullptr)
				cvDrawContours(_image, current, cvScalar(0, 0, 200), cvScalar(0, 0, 200), 0, 2, 8);


				// �����
				// 1/4*CV_PI = 0,079577
				if (area / (perim * perim) > 0.06 && area / (perim * perim) < 1.0){
				// �������� �����
				//cvDrawContours(_image, current, cvScalar(200, 0, 0), cvScalar(0, 200, 0), 0, 2, 8);
				}
		i++;
	}*/

	cvNamedWindow("contour");
	cvShowImage("contour", imageContour);
	allContours(contours);
	// ����������� �������
	cvReleaseMemStorage(&storage);
	//cvReleaseImage(&bin);

	return imageContour;
}

void Contours::allContours(CvSeq *contours) {
	IplImage *imageContour = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	all = new list<IplImage*>();

	recAllContours(contours);

}

void Contours::recAllContours(CvSeq* current) {
	if (current == nullptr)
		return;
	CvSeq* current2 = current;
	if (current->h_next != nullptr)
		recAllContours(current->h_next);
	if (current->v_next != nullptr)
		recAllContours(current->v_next);
	IplImage *tmp = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvDrawContours(tmp, current2, cvScalar(0, 0, 200), cvScalar(0, 0, 200), 0, 1, 0);
	all->push_back(tmp);
}