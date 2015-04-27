#pragma warning(disable:4496)
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <list>

using namespace std;
using namespace cv;

class FindContours
{
public:
	FindContours();
	~FindContours();
private:
	// Find contours.
	void findContours();
	// Delete same contours.
	void deleteContours();
	// Put all contours to list.
	void allContours();
	// 
	void recAllContours(CvSeq* current);

	// is circle
	bool isCircle(CvSeq *seq);
	// is rectangle
	bool isRectangle(CvSeq *seq);
	// is rhomb
	bool isRhomb(CvSeq *seq);
	// is flag like - |<
	bool isFlagIn(CvSeq *seq);
	// is flag like - |>
	bool isFlagOut(CvSeq *seq);
	//
	int countTrue(bool isWidth, int number, bool **array, int size);


	// Black And White
	IplImage* imageBW = 0;
	// Image
	IplImage* image = 0;
	// First in contours
	CvSeq* contours = 0;
	// Contours
	IplImage *imageContour = 0;
	// Image of result
	IplImage *result = 0;

	// All contours as image
	list<IplImage*> *all;
	// All contours
	list<CvSeq *> *allContoursSeq = 0;

};

