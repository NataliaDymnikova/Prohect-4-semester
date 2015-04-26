#include "Contours.h"
#include <stdlib.h>
#include <stdio.h>
#include "SearchRelations.h"

Contours::Contours()
{
	// получаем картинку
	image = cvLoadImage("../3.png", CV_LOAD_IMAGE_ANYCOLOR);

	assert(image != 0);
	findContours();
	cvShowImage("contours", imageContour);
	findCircles();
	cvShowImage("Circle", imageCircles);
	findRectangles();
	cvShowImage("Rectangles", imageRectangles);
	findRelations();
	cvShowImage("Relations", imageRelations);
	makeResult();
	cvShowImage("Result", result);
	cvWaitKey(0);
	
}



Contours::~Contours()
{
	// освобождаем ресурсы
	cvReleaseImage(&image);
	cvReleaseImage(&imageCircles);
	cvReleaseImage(&imageContour);
	//delete circles;
	//delete contours;
	//all->remove_if(true);
	delete all;
	// удаляем окна
	cvDestroyAllWindows();
}

void Contours::findContours()
{
	assert(image != 0);
	cvShowImage("real", image);

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

	imageContour = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	//cvDrawContours(imageContour, contours, cvScalar(0, 0, 200), cvScalar(200, 0, 0), 8, 1, 8);

	allContours(); 
	
	int i = 0;
	for each (CvSeq *current in *allContoursSeq) {
		cvDrawContours(imageContour, current, cvScalar(0,0,0), cvScalar(0,0,0), 0, 1);
	}
}

void Contours::allContours() {
	all = new list<IplImage*>();
	allContoursSeq = new list<CvSeq *>();
	recAllContours(contours);
	deleteContours();
}

void Contours::recAllContours(CvSeq* current) {
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

void Contours::deleteContours() {
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

void Contours::findCircles() {
	imageCircles = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	circles = new list<CvSeq*>();

	for each (CvSeq *current in *allContoursSeq) {
		// вычисляем площадь и периметр контура
		double area = fabs(cvContourArea(current));
		double perim = cvContourPerimeter(current);
		// 1/4*CV_PI = 0,079577
		if (area / (perim * perim) > 0.065 && area / (perim * perim) < 0.85){
			CvPoint2D32f center;
			float radius;
			cvMinEnclosingCircle(current, &center, &radius);
			cvDrawCircle(imageCircles, cvPoint(center.x, center.y), radius, cvScalar(0, 0, 0));

			circles->push_back(current);
		}
	}
}

bool Contours::isCircle(CvSeq *cont) {
	for each (CvSeq* current in *circles)
		if (current->total == cont->total)
			return true;
	return false;
}

void Contours::findRectangles() {
	imageRectangles = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	rectangles = new list<CvRect *>();
	int i = 0;
	for (CvSeq *current : *allContoursSeq) {
		if (isCircle(current))
			continue;
		CvRect *rect = new CvRect(cvBoundingRect(current, 0));
		i++;
		double area = fabs(cvContourArea(current));
		double area2 = rect->height * rect->width;
		if (area2 < 1.275 * area) {
			rectangles->push_back(rect);
			cvDrawRect(imageRectangles, cvPoint(rect->x, rect->y)
				, cvPoint(rect->x + rect->width, rect->y + rect->height)
				, cvScalar(0, 0, 0), 1);
		}

	}
}

void Contours::findRelations() {
	imageRelations = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	IplImage *imageLines = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCopyImage(imageBW, imageLines);
	linesList = new list<vector<int>>();

	for each (CvSeq *current in *allContoursSeq) {
		CvRect *rect = new CvRect(cvBoundingRect(current, 0));
		double area = fabs(cvContourArea(current));
		double area2 = rect->height * rect->width;
		if (isCircle(current) || area2 < 1.3 * area) {
			cvDrawRect(imageLines, cvPoint(rect->x - 5, rect->y - 5)
				, cvPoint(rect->x + rect->width + 5, rect->y + rect->height + 5)
				, cvScalar(255, 255, 255), CV_FILLED);
		}
	}

	CvMemStorage* storage = cvCreateMemStorage(0);
	Mat bin2 = (Mat)imageLines < 178;
	imageLines = new IplImage(bin2);
	cvCanny(imageLines, imageLines, 0, 256);
	
	CvSeq *lines = cvHoughLines2(imageLines, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 1, 20, 30);
	// нарисуем найденные линии
	for (int i = 0; i < lines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		if (!canAddONot(line))
			continue;
		cvLine(imageRelations, line[0], line[1], CV_RGB(0, 0, 0), 1, CV_AA, 0);
		vector<int> pointers;
		pointers.push_back(line[0].x);
		pointers.push_back(line[0].y);
		pointers.push_back(line[1].x);
		pointers.push_back(line[1].y);
		linesList->push_back(pointers);
	}

	// Если две линии пересекаются - сделать их одной.
	// Сделать 4 линии - оставить самую длинную
}


bool Contours::canAddONot(CvPoint *newLine) {
	for each (vector<int> line in *linesList) {
		if (fabs(fabs((double)newLine[0].x - line[0]) + fabs((double)newLine[0].y - line[1])
			+ fabs((double)newLine[1].x - line[2]) + fabs((double)newLine[1].y - line[3])) < 50)
			return false;
	}
	return true;
}

void Contours::makeResult() {
	result = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCopy(imageCircles, result);
	for each (CvRect *rect in *rectangles)
		cvDrawRect(result, cvPoint(rect->x, rect->y)
			, cvPoint(rect->x + rect->width, rect->y + rect->height)
			, cvScalar(0, 0, 0));
	for each (vector<int> vec in *linesList)
		cvDrawLine(result
			, cvPoint(vec.at(0), vec.at(1)), cvPoint(vec.at(2), vec.at(3))
			, cvScalar(0,0,0));
	
}

void Contours::findFlags() {
	imageFlagsIn = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	
	for each (CvSeq *current in *allContoursSeq) {
		CvRect *rect = new CvRect(cvBoundingRect(current, 0));
		double area = fabs(cvContourArea(current));
		double area2 = rect->height * rect->width;
		if (isCircle(current) || area2 < 1.3 * area) {
			continue;
		}

	}
}
