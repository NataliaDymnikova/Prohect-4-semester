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
	// Result in imageRelations
	void findRelations();
	// Can add new line or not
	bool canAddONot(CvPoint *newLine);
	// Make result picture
	void makeResult();
	// Result in imageFlagsIn
	void findFlags();
	// Result in imageFlagsOut
	//void findFlagsOut();

	// Black And White
	IplImage* imageBW = 0;
	// Image
	IplImage* image = 0;
	// First in contours
	CvSeq* contours = 0;
	// Image with circles
	IplImage *imageCircles = 0;
	// Image with rectangles
	IplImage *imageRectangles = 0;
	// Image with relations
	IplImage *imageRelations = 0;
	// Image of result
	IplImage *result = 0;
	// Image of flagsOutside
	IplImage *imageFlagsOut = 0;
	// Image of flagsInside
	IplImage *imageFlagsIn = 0;

	// All contours
	list<CvSeq *> *allContoursSeq = 0;
	// Rectangles
	list<CvRect *> *rectangles = 0;
	// Circles
	list<CvSeq *> *circles = 0;
	// Lines
	list<vector<int>> *linesList = 0;
	// Flags in
	//list<
	// Flags out

	// Delete same contours.
	void deleteContours();

	void allContours();
	void recAllContours(CvSeq* current);
};

