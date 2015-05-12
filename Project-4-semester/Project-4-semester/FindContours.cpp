#define _USE_MATH_DEFINES
#include "FindContours.h"

#include <math.h>
#include "opencv\ml.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "BoolPicture.h"

FindContours::FindContours()
{
	// получаем картинку
	image = cvLoadImage("../3.png", CV_LOAD_IMAGE_ANYCOLOR);
	
	assert(image != 0);
	findContours();
	cvShowImage("image", image);
	cvShowImage("contours", imageContour);
	cvShowImage("result", result);
	cvShowImage("lines", imageRelations);

	cvWaitKey(0);
}


FindContours::~FindContours()
{
}

void FindContours::findContours()
{
	assert(image != 0);
	
	// to black and white. 
	Mat bin2;
	cvtColor((Mat)image, bin2, CV_RGB2GRAY);
	bin2 = bin2 > 178;
	imageBW = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	IplImage *bin = new IplImage(bin2);
	cvCopy(bin, imageBW);
	// хранилище памяти для контуров
	CvMemStorage* storage = cvCreateMemStorage(0);

	// находим контуры
	cvFindContours(bin, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 0, 1);//Апроксимация контуров
	assert(contours != 0);

	allContours();
	tryToDeleteCycle();

	imageContour = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	imageContourCopy = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	result = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	allNewContours = new list<ContourAndSize>();

	//tryToFindText();
	//cvWaitKey(0);
	for (CvSeq *current : *allContoursSeq) {

		cvDrawContours(imageContour, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0, 1);
		cvDrawContours(imageContourCopy, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0, 1);
		if (current == mainContour)
			continue;
		
		if (isCircle(current)) {
			CvPoint2D32f center;
			float radius;
			cvMinEnclosingCircle(current, &center, &radius);
			cvDrawCircle(result, cvPoint(center.x, center.y), radius, cvScalar(0, 0, 0));
			
			cvDrawCircle(imageContourCopy, cvPoint(center.x, center.y), radius * 1.1
				, cvScalar(205, 205, 205), CV_FILLED);

			CvRect rect = cvBoundingRect(current, 0);
			IplImage *im = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, 1);
			cvDrawCircle(im, cvPoint(center.x, center.y), radius, cvScalar(0, 0, 0), 0);
			Mat seq = Mat(im) < 128;
			allNewContours->push_back(ContourAndSize(BoolPicture::convertBWToBool(seq)
				, rect.height, rect.width, cvPoint(rect.x, rect.y)));
		}
		else if (isRectangle(current)) {
			CvRect *rect = new CvRect(cvBoundingRect(current, 0));
			cvDrawRect(result, cvPoint(rect->x, rect->y)
				, cvPoint(rect->x + rect->width, rect->y + rect->height)
				, cvScalar(0, 0, 0), 1);
			
			int temp = max(rect->width, rect->height) * 0.09;
			cvDrawRect(imageContourCopy, cvPoint(rect->x - temp, rect->y - temp)
				, cvPoint((rect->x + rect->width + 2 * temp), (rect->y + rect->height + 2* temp))
				, cvScalar(205, 205, 205), CV_FILLED);

			IplImage *im = cvCreateImage(cvSize(rect->width, rect->height), IPL_DEPTH_8U, 1);
			cvDrawRect(im, cvPoint(rect->x, rect->y)
				, cvPoint(rect->x + rect->width, rect->y + rect->height), cvScalar(0, 0, 0), 0);
			Mat seq = Mat(im) < 128;
			allNewContours->push_back(ContourAndSize(BoolPicture::convertBWToBool(seq)
				, rect->height, rect->width, cvPoint(rect->x, rect->y)));
		}
		else if (isRhomb(current)) {
			CvMemStorage* storageRhomb = cvCreateMemStorage(0);
			CvBox2D rhomb = cvMinAreaRect2(current, storageRhomb);
			CvPoint2D32f pointsf[4];
			cvBoxPoints(rhomb, pointsf);
			CvPoint pointsi[4];
			for (int i = 0; i < 4; i++) {
				pointsi[i] = cvPointFrom32f(pointsf[i]);
			}
			CvPoint* countours[1] = { pointsi };
			int countours_n[1] = { 4 };
			cvDrawPolyLine(result, countours, countours_n, 4, 1, cvScalar(0, 0, 0));

			CvRect *rect = new CvRect(cvBoundingRect(current, 0));
			int temp = max(rect->width, rect->height) * 0.09;
			cvDrawRect(imageContourCopy, cvPoint(rect->x - temp, rect->y - temp)
				, cvPoint((rect->x + rect->width + 2 * temp), (rect->y + rect->height + 2 * temp))
				, cvScalar(205, 205, 205), CV_FILLED);

			IplImage *im = cvCreateImage(cvSize(rect->width, rect->height), IPL_DEPTH_8U, 1);
			cvDrawPolyLine(im, countours, countours_n, 4, 1, cvScalar(0, 0, 0), 0);
			Mat seq = Mat(im) < 128;
			allNewContours->push_back(ContourAndSize(BoolPicture::convertBWToBool(seq)
				, rect->height, rect->width, cvPoint(rect->x, rect->y)));
		}
		else if (isFlagIn(current)) {
			cvDrawContours(result, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
			
			CvRect *rect = new CvRect(cvBoundingRect(current, 0));
			int temp = max(rect->width, rect->height) * 0.09;
			cvDrawRect(imageContourCopy, cvPoint(rect->x - temp, rect->y - temp)
				, cvPoint((rect->x + rect->width + 2 * temp), (rect->y + rect->height + 2 * temp))
				, cvScalar(205, 205, 205), CV_FILLED);

			IplImage *im = cvCreateImage(cvSize(rect->width, rect->height), IPL_DEPTH_8U, 1);
			cvDrawContours(im, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
			Mat seq = Mat(im) < 128;
			allNewContours->push_back(ContourAndSize(BoolPicture::convertBWToBool(seq)
				, rect->height, rect->width, cvPoint(rect->x, rect->y)));
		}
		else if (isFlagOut(current)) {
			cvDrawContours(result, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
			
			CvRect *rect = new CvRect(cvBoundingRect(current, 0));
			int temp = max(rect->width, rect->height) * 0.09;
			cvDrawRect(imageContourCopy, cvPoint(rect->x - temp, rect->y - temp)
				, cvPoint((rect->x + rect->width + 2 * temp), (rect->y + rect->height + 2 * temp))
				, cvScalar(205, 205, 205), CV_FILLED);

			IplImage *im = cvCreateImage(cvSize(rect->width, rect->height), IPL_DEPTH_8U, 1);
			cvDrawContours(im, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
			Mat seq = Mat(im) < 128;
			allNewContours->push_back(ContourAndSize(BoolPicture::convertBWToBool(seq)
				, rect->height, rect->width, cvPoint(rect->x, rect->y)));
		}
		else
		{
			cvDrawContours(imageContour, current, cvScalar(205, 205, 205), cvScalar(205, 205, 205), 0, 1);
			cvDrawContours(result, current, cvScalar(255, 0, 0), cvScalar(255, 0, 0), 0);
		}
	}
	findRelations();
}

void FindContours::allContours() {
	all = new list<IplImage*>();
	allContoursSeq = new list<CvSeq *>();
	recAllContours(contours);
	deleteContours();
}

void FindContours::recAllContours(CvSeq* current) {
	if (current == nullptr)
		return;
	CvSeq* current2 = current;
	if (current->h_next != nullptr)
		recAllContours(current->h_next);
	if (current->v_next != nullptr)
		recAllContours(current->v_next);

	allContoursSeq->push_front(current);

	IplImage *tmp = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvDrawContours(tmp, current2, cvScalar(0, 0, 200), cvScalar(0, 0, 200), 0, 1, 0);
	all->push_back(tmp);
}

void FindContours::deleteContours() {
	list<CvSeq *> *newSeq = new list<CvSeq *>();
	bool flag = false;
	int i = 0;
	int j = 0;
	// delete first contour - contour of image
	allContoursSeq->pop_front();
	for (CvSeq *current : *allContoursSeq) {
		CvRect rect1 = cvBoundingRect(current);
		flag = false;
		j = 0;
		for (CvSeq *current2 : *allContoursSeq) {
			if (i <= j)
				continue;
			CvRect rect2 = cvContourBoundingRect(current2);
			if (fabs((double)(rect1.x - rect2.x)) + fabs((double)(rect1.y - rect2.y)) +
				fabs((double)(rect1.height - rect2.height)) + fabs((double)(rect1.width - rect2.width)) < 100)
			{
				flag = true;
				break;
			}
			j++;
		}
		if (!flag)
			newSeq->push_back(current);
		i++;
	}
	allContoursSeq = newSeq;
}

bool FindContours::isCircle(CvSeq *current) {
	double area = fabs(cvContourArea(current));
	double perim = cvContourPerimeter(current);
	// 1/4*CV_PI = 0,079577
	if (area / (perim * perim) > 0.065 && area / (perim * perim) < 0.85){
		CvPoint2D32f center;
		float radius;
		cvMinEnclosingCircle(current, &center, &radius);
		return (M_PI * radius *  radius <= 1.4 * area);
	}
	return false;
}

bool FindContours::isRectangle(CvSeq *current) {
	CvRect rect = cvBoundingRect(current, 0);
	double area = fabs(cvContourArea(current));
	double area2 = rect.height * rect.width;
	return (area2 <= 1.275 * area);
}

bool FindContours::isRhomb(CvSeq *current) {
	CvRect rect = cvBoundingRect(current, 0);
	double area = fabs(cvContourArea(current));
	double area2 = rect.height * rect.width;
	return (area2 <= 1.275 * 2 * area && area2 >= 0.825 * 2 * area);
}

bool FindContours::isFlagIn(CvSeq *current) {
	CvRect rect = cvBoundingRect(current, 0);
	IplImage *im = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, 1);
	cvDrawContours(im, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
	Mat seq = Mat(im) < 128;
	bool **seqBool = BoolPicture::convertBWToBool(seq);
	int width = seq.cols;
	int height = seq.rows;
	
	for (int i = 0; i < width; i++)
		if (countTrue(true, i, seqBool, height) > 2) {
			return false;
		}
	
	for (int i = 0; i < height; i++)
		if (countTrue(false, i, seqBool, width) > 2) {
			return false;
		}
	
	return true;
}

bool FindContours::isFlagOut(CvSeq *current) {
	CvRect rect = cvBoundingRect(current, 0);
	IplImage *im = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, 1);
	cvDrawContours(im, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
	Mat seq = Mat(im) < 128;
	bool **seqBool = BoolPicture::convertBWToBool(seq);
	int width = seq.cols;
	int height = seq.rows;
	
	bool isThree = false;
	for (int i = 0; i < width; i++)
		if (countTrue(true, i, seqBool, height) > 2) {
			if (countTrue(true, i, seqBool, height) > 4)
				return false;
			isThree = true;
		} 

	for (int i = 0; i < height; i++)
		if (countTrue(false, i, seqBool, width) > 2) {
			if (countTrue(true, i, seqBool, height) > 4)
				return false;
			isThree = true;
		}
	
	return false;
}

int FindContours::countTrue(bool isWidth, int number, bool **array, int size) {
	int result = 0;
	for (int i = 0; i < size; i++) {
		if (isWidth) {
			if (array[i][number])
				result++;
		}
		else if (array[number][i])
			result++;
	}
	return result;
}

void FindContours::findRelations() {
	imageRelations = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	linesList = new list<vector<int>>();
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	IplImage *imageLines = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCanny(imageContourCopy, imageLines, 0, 256);

	CvSeq *lines = cvHoughLines2(imageLines, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 1, 25, 20);
	// нарисуем найденные линии
	for (int i = 0; i < lines->total; i++) 
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
 		if (!canAddONot(line))
			continue;
		cvLine(imageRelations, line[0], line[1], CV_RGB(0, 0, 0));
		cvLine(result, nearestPoint(line[0]), nearestPoint(line[1]), CV_RGB(0, 0, 0));
		vector<int> pointers;
		pointers.push_back(line[0].x);
		pointers.push_back(line[0].y);
		pointers.push_back(line[1].x);
		pointers.push_back(line[1].y);
		linesList->push_back(pointers);
	}
}


bool FindContours::canAddONot(CvPoint *newLine) {
	int temp = max(fabs((double)newLine[0].x - newLine[1].x), fabs((double)newLine[0].y - newLine[1].y)) * 5;
	for (vector<int> line : *linesList) {
		if (fabs(fabs((double)newLine[0].x - line[0]) + fabs((double)newLine[0].y - line[1])
			+ fabs((double)newLine[1].x - line[2]) + fabs((double)newLine[1].y - line[3])) < temp)
			return false;
	}
	return true;
}


CvPoint FindContours::nearestPoint(CvPoint point) {
	float minRad = -1;
	CvPoint pointRes = cvPoint(0, 0);
	
	for (ContourAndSize current : *allNewContours) {
		float currentRadius = -1;
		CvPoint curretPoint = cvPoint(0, 0);

		for (int i = 0; i < current.width; i++) {
			for (int j = 0; j < current.height; j++) {
				if (!current.picture[j][i]) {
					float dist = distance(point, cvPoint(i + current.leftTop.x, j + current.leftTop.y));
					if (dist < currentRadius || currentRadius == -1) {
						currentRadius = dist;
						curretPoint = cvPoint(i + current.leftTop.x, j +current.leftTop.y);
					}
				}
			}
		}
		if ((currentRadius < minRad || minRad == -1) && currentRadius != -1) {
			minRad = currentRadius;
			pointRes = curretPoint;
		}
	}

	return pointRes;
}

float FindContours::distance(CvPoint first, CvPoint second) {
	return ((first.x - second.x) * (first.x - second.x) + (first.y - second.y) * (first.y - second.y));
}


/*

*/

void FindContours::tryToFindText() {
	/*Mat large = Mat(image);
	Mat rgb;
	// downsample and use it for processing
	pyrDown(large, rgb);
	Mat small;
	cvtColor(rgb, small, CV_BGR2GRAY);
	// morphological gradient
	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	// binarize
	Mat bw;
	threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	// connect horizontally oriented regions
	Mat connected;
	morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
	morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	// find contours
	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	cv::findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	// filter contours
	for (int idx = 2; idx >= 0; idx = hierarchy[idx][0])
	{
		Rect rect = boundingRect(contours[idx]);
		Mat maskROI(mask, rect);
		maskROI = Scalar(0, 0, 0);
		// fill the contour
		drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		// ratio of non-zero pixels in the filled region
		double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

		if (r > .55
			&&
			(rect.height > 8 && rect.width > 8)
			)
		{
			rectangle(rgb, rect, Scalar(0, 255, 0), 2);
		}
	}
*/
	IplImage *rgb = cvCreateImage(cvGetSize(image), IPL_DEPTH_16U, 1);
	for (CvSeq *current : *allContoursSeq) {
		Rect *rect = new Rect(cvBoundingRect(current, 0)); 
		double r = (double)current->delta_elems / (rect->width * rect->height);
		if (r > 0.03)
			cvDrawRect(rgb, cvPoint(rect->x, rect->y)
			, cvPoint(rect->x + rect->width, rect->y + rect->height), cvScalar(0, 255, 0), 1);
	}

	cvShowImage("aaaaaaaa", rgb);
}

void FindContours::tryToDeleteCycle() {
	int sum = 0;
	int max = -1;
	CvSeq *maxSeq = 0;
	for (CvSeq *current : *allContoursSeq) {
		int c = fabs(cvContourArea(current));
		if (max == -1 || max < c) {
			max = c;
			maxSeq = current;
		}
		sum += c;
	}
	sum -= max;
 	mainContour = maxSeq; 
	int middle = sum / (allContoursSeq->size() - 1);
	list<CvSeq *> *forDelete = new list<CvSeq *>();
	for (CvSeq *current : *allContoursSeq) {
		int c = fabs(cvContourArea(current));
		if (c == max)
			continue;
		if (middle <= 0.5 * c && middle <= c)
			forDelete->push_back(current);
	} 
	for (CvSeq *current : *forDelete) {
		bool flag = false;
		for (CvSeq *current2 : *allContoursSeq) {
			if (current == current2){
				flag = true;
				break;
			}
		}
		if (flag)
			allContoursSeq->remove(current);
	}
}