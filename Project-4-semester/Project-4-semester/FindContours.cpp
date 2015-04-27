#define _USE_MATH_DEFINES
#include "FindContours.h"

#include <math.h>

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
	/*findCircles();
	cvShowImage("Circle", imageCircles);
	findRectangles();
	cvShowImage("Rectangles", imageRectangles);
	findRelations();
	cvShowImage("Relations", imageRelations);
	makeResult();
	cvShowImage("Result", result);
	*/
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

	imageContour = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	result = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	for (CvSeq *current : *allContoursSeq) {
		cvDrawContours(imageContour, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0, 1);
		if (isCircle(current)) {
			CvPoint2D32f center;
			float radius;
			cvMinEnclosingCircle(current, &center, &radius);
			cvDrawCircle(result, cvPoint(center.x, center.y), radius, cvScalar(0, 0, 0));
		}
		else if (isRectangle(current)) {
			CvRect *rect = new CvRect(cvBoundingRect(current, 0));
			cvDrawRect(result, cvPoint(rect->x, rect->y)
				, cvPoint(rect->x + rect->width, rect->y + rect->height)
				, cvScalar(0, 0, 0), 1);
		}
		else if (isRhomb(current)) {
			// хранилище памяти для контуров
			CvMemStorage* storageRhomb = cvCreateMemStorage(0);
			CvBox2D rhomb = cvMinAreaRect2(current, storageRhomb);
			CvPoint2D32f pointsf[4];
			cvBoxPoints(rhomb, pointsf);
			CvPoint pointsi[4];
			for (int i = 0; i<4; i++) {
				pointsi[i] = cvPointFrom32f(pointsf[i]);
			}
			CvPoint* countours[1] = { pointsi };

			int countours_n[1] = { 4 };

			cvDrawPolyLine(result, countours, countours_n, 4, 1, cvScalar(0, 0, 0));
		}
		/*else if (isFlagIn(current)) {
			cvDrawContours(result, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
		}
		else if (isFlagOut(current)) {
			cvDrawContours(result, current, cvScalar(0, 0, 0), cvScalar(0, 0, 0), 0);
		}*/
	}
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
	for each (CvSeq *current in *allContoursSeq) {
		CvRect rect1 = cvBoundingRect(current);
		flag = false;
		j = 0;
		for each (CvSeq *current2 in *allContoursSeq) {
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
	Mat seq;
	cvCvtSeqToArray(current, &seq);
	bool **seqBool = BoolPicture::convertBWToBool(seq);
	int width = seq.cols;
	int height = seq.rows;

	/*uchar **array = new uchar*[height];
	bool **seqBool = new bool*[height];
	for (int i = 0; i < height; i++) {
		array[i] = new uchar[width];
		seqBool[i] = new bool[width];
	}
	for (int i = 0; i < height; i++) {
		//array[i] = seq.ptr<uchar>(i);
		for (int j = 0; j < width; j++) {
			array[i][j] = seq.at<uchar>(i, j);
			seqBool[i][j] = array[i][j];
		}
	}
	*/
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
	Mat *seq = (Mat *)current;
	CvRect rect = cvBoundingRect(current, 0);
	bool **seqBool = BoolPicture::convertBWToBool(*seq);
	int width = rect.width;
	int height = rect.height;
	for (int i = 0; i < width; i++)
		if (countTrue(true, i, seqBool, height) > 2) {
			return true;
		}
	for (int i = 0; i < height; i++)
		if (countTrue(false, i, seqBool, width) > 2) {
			return true;
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
