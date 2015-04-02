#pragma warning(disable:4496)
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <list>

using namespace std;
using namespace cv;

class Contours
{
public:
	Contours();
	~Contours();
	IplImage *findCircles(IplImage *image);
	IplImage* image = 0;
	IplImage* gray = 0;
	IplImage* bin = 0;
	IplImage* dst = 0;
	CvSeq* contours = 0;
	IplImage *imageContour;

	list<IplImage*> *all;

	void allContours(CvSeq *contours);
	void recAllContours(CvSeq* current);
};

