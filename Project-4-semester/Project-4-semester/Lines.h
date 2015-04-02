#pragma warning(disable:4496)
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <list>
#include <vector>

using namespace std;
using namespace cv;

class Lines
{
public:
	Lines(IplImage *imageContour);
	~Lines();

	IplImage* image = 0;
	IplImage* gray = 0;
	IplImage* bin = 0;
	IplImage* dst = 0;
	// beginX, beginY, endX, endY
	list<vector<int>> *linesList;
	bool canAddONot(CvPoint *newLine);
};

