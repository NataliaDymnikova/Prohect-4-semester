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

	//cvNamedWindow("real");
	cvShowImage("real", image);

	Mat bin2;
	cvtColor((Mat)image, bin2, CV_RGB2GRAY);
	bin2 = bin2 < 200;
	IplImage *bin = new IplImage(bin2);

	cvCanny(bin, bin, 50, 200);
	// хранилище памяти для контуров
	CvMemStorage* storage = cvCreateMemStorage(0);
	
	// находим контуры
	int contoursCont = cvFindContours(bin, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 0, 1);//Апроксимация контуров
	
	assert(contours != 0);
	imageContour = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	//cvDrawContours(imageContour, contours, cvScalar(0, 0, 200), cvScalar(200, 0, 0), 8, 1, 8);

	allContours(); 
	// освобождаем ресурсы
	//cvReleaseMemStorage(&storage);
	//cvReleaseImage(&bin);
	int i = 0;
	for each (CvSeq *current in *allContoursSeq) {
		cvDrawContours(imageContour, current, cvScalar(0,0,0), cvScalar(0,0,0), 0, 1);
		//cvShowImage("contours", imageContour);
	}
}

void Contours::findCircles() {
	imageCircles = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	
	for each (CvSeq *current in *allContoursSeq) {
		// вычисляем площадь и периметр контура
		double area = fabs(cvContourArea(current));
		double perim = cvContourPerimeter(current);
		
		// круги
		// 1/4*CV_PI = 0,079577
		if (area / (perim * perim) > 0.06 && area / (perim * perim) < 1.0){
			// нарисуем контур
			cvDrawCircle(imageCircles, /*!!!center!!!*/cvPoint(350,140), (int)(perim/2/CV_PI), cvScalar(0,0,0), 1, 8);
			if (circles == nullptr) {
				circles = new CvSeq(*current);
				circles->h_next = circles->h_prev = circles->v_next = circles->v_prev = nullptr;
			}
			else {
				CvSeq *temp = circles->h_next;
				circles->h_next = current;
				circles->h_next = temp;
			}
		}
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

void Contours::findRectangles() {
	if (imageRectangles == nullptr) 
		imageRectangles = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	// хранилище памяти для контуров
	int i = 0;
	for each (CvSeq *current in *allContoursSeq) {
		if (isCircle(current))
			continue;
		// Если граница внутренняя
		CvRect rect = cvBoundingRect(current, 0);
		i++;
		cvDrawRect(imageRectangles, cvPoint(rect.x, rect.y), cvPoint(rect.x+rect.width, rect.y+rect.height), cvScalar(0,0,0), 2);
	}
 }

bool Contours::isCircle(CvSeq *cont) {
	for (CvSeq* current = circles; current != NULL; current = current->h_next)
		if (current->total == cont->total)
			return true;
	return false;
}

void Contours::deleteContours() {
	list<CvSeq *> *newSeq = new list<CvSeq *>();
	bool flag = false;
	int i = 0;
	int j = 0;
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