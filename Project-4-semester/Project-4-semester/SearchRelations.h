#pragma once
#include <list>
#include <vector>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;
using namespace std;

class SearchRelations
{
public:
	SearchRelations(list<vector<int>> *lines, list<IplImage*> *allContours);
	~SearchRelations();

private:
	list<vector<int>> *lines;
	list<IplImage*> *allContours;
	list<vector<int>> *linesRelations;
	list<IplImage*> *notLineContours;
	double countBlackPixels(Mat  &image);
};

