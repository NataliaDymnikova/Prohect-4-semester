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
	// Contours
	IplImage *imageContour = 0;
	// All contours as image
	list<IplImage*> *all;

private:
	// Result on contours
	void findContours();
	// Result on circles
	void findCircles();
	// Result in imageRectangles
	void findRectangles();
	// Is circle or not
	bool isCircle(CvSeq *cont);

	// Image
	IplImage* image = 0;
	// First in contours
	CvSeq* contours = 0;
	// Image with circles
	IplImage *imageCircles = 0;
	// Circles
	CvSeq *circles = 0;
	// Image with rectangles
	IplImage *imageRectangles = 0;
	// All contours
	list<CvSeq *> *allContoursSeq = 0;
	
	// Delete same contours.
	void deleteContours();

	void allContours();
	void recAllContours(CvSeq* current);
};

