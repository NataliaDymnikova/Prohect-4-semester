#pragma warning(disable:4496)
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;

class Contours
{
public:
	Contours();
	~Contours();
	IplImage *findCircles(IplImage *omage);
	IplImage* image = 0;
	IplImage* gray = 0;
	IplImage* bin = 0;
	IplImage* dst = 0;

};

