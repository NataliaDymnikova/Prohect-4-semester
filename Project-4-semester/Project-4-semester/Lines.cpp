#include "Lines.h"


Lines::Lines(IplImage *imageContour)
{
	linesList = new std::list<vector<int>>();
	image = cvLoadImage("../3.png", CV_LOAD_IMAGE_ANYCOLOR);

	assert(image != 0);

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	int i = 0;

	/*Mat bin2;
	cvtColor((Mat)imageContour, bin2, CV_RGB2GRAY);
	bin2 = bin2 > 200;
	dst = new IplImage(bin2);
	// нахождение линий
	*/
	Mat bin2 = (Mat)imageContour;
	//	cvtColor((Mat)imageContour, bin2, CV_RGB2GRAY);
	bin2 = bin2 < 200;
	/*
	dst = cvCreateImage(cvGetSize(image), 8, 1);
	cvCanny(image, dst, 50, 200);*/
	dst = new IplImage(bin2);
	lines = cvHoughLines2(dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 30, 40, 20);
	// нарисуем найденные линии
	for (i = 0; i < lines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		if (!canAddONot(line))
			continue;
		cvLine(image, line[0], line[1], CV_RGB(255, 0, 0), 1, CV_AA, 0);
		
		vector<int> pointers;
		pointers.push_back(line[0].x);
		pointers.push_back(line[0].y);
		pointers.push_back(line[1].x);
		pointers.push_back(line[1].y); 
		linesList->push_back(pointers);
	}

	// показываем
	cvNamedWindow("Source", 1);
	cvShowImage("Source", dst);

	cvNamedWindow("Hough", 1);
	//Mat bin2 = (Mat)imageContour;
	//	cvtColor((Mat)imageContour, bin2, CV_RGB2GRAY);
	//bin2 = bin2 > 200;
	cvShowImage("Hough", image);

	// ждём нажатия клавиши
	cvWaitKey(0);
	cvReleaseMemStorage(&storage);
}


Lines::~Lines()
{
	//cvReleaseImage(&dst);
	cvReleaseImage(&image);
	cvDestroyAllWindows();
}

bool Lines::canAddONot(CvPoint *newLine) {
	for each (vector<int> line in *linesList) {
		if (fabs(fabs((double)newLine[0].x - line[0]) + fabs((double)newLine[0].y - line[1])
			+ fabs((double)newLine[1].x - line[2]) + fabs((double)newLine[1].y - line[3])) < 10)
			return false;
	}
	return true;
}