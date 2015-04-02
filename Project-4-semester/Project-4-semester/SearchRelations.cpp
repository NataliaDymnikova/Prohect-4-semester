#include "SearchRelations.h"
#include <cmath>

using namespace std;

// Нужно найти внутренние контуры. и те линии, которые с ними много взаимодейтсвуют - удалить.
// Остальные и будут связями.

SearchRelations::SearchRelations(list<vector<int>> *lines, list<IplImage*> *all) {
	this->lines = lines;
	this->allContours = all;
	
	for each (vector<int> line in *lines) {
		CvSize size;
		size.width = fabs((float)line[3] - line[1]) +1;
		size.height = fabs((float)line[2] - line[0])+1;
		IplImage *linePic = cvCreateImage(size, IPL_DEPTH_8U, 1);
		cvLine(linePic, cvPoint(0,0), cvPoint(size.width,size.height), CV_RGB(0, 0, 0), 1);

		double minDif = -1;
		IplImage *minDifPic = cvCreateImage(size, IPL_DEPTH_8U, 1);
		for each (IplImage *contour in *all) {
			IplImage *contourPic = cvCreateImage(size, contour->depth, contour->nChannels);
			cvSetImageROI(contour, cvRect(line[0], line[1], size.width, size.height));
			cvCopy(contour, contourPic, NULL);
			cvResetImageROI(contour);

			IplImage *dif = cvCreateImage(size, IPL_DEPTH_8U, 1);
			cvAbsDiff(linePic, contourPic, dif);
			
			long currentDif = fabs(countBlackPixels((Mat)contourPic) - countBlackPixels((Mat)linePic));
			if (minDif = -1 || minDif > currentDif) {
				minDif = currentDif;
				minDifPic = contourPic;
			}
		}

		cvDestroyAllWindows();
		cvNamedWindow("line");
		cvNamedWindow("contour");
		cvNamedWindow("dif");
		cvShowImage("line", linePic);
		cvShowImage("contour", minDifPic);
		waitKey(0);
	}
}

double SearchRelations::countBlackPixels(Mat  &image) {
	long res = 0;
	for (int y(0); y < image.rows; ++y) {
		for (int x(0); x < image.cols; ++x)
			if (image.at<unsigned char>(y, x) == 0)
				res++;
	}
	return res;
}

SearchRelations::~SearchRelations() {
}
