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
	// Return true if something similar not exist in linesList
	bool canAddONot(CvPoint *newLine);

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
	// Number of elements in array
	int countTrue(bool isWidth, int number, bool **mas, int size);
	//
	void findRelations();
	//
	CvPoint nearestPoint(CvPoint point);
	//
	float distance(CvPoint first, CvPoint second);

	// Black And White
	IplImage* imageBW = 0;
	// Image
	IplImage* image = 0;
	// First in contours
	CvSeq* contours = 0;
	// Contours
	IplImage *imageContour = 0;
	// Copy contours
	IplImage *imageContourCopy = 0;
	// Image of result
	IplImage *result = 0;
	// Image of relations
	IplImage *imageRelations = 0;
	
	// All contours as image
	list<IplImage *> *all = 0;
	// All contours
	list<CvSeq *> *allContoursSeq = 0;
	// All lines - relations
	list<vector<int>> *linesList = 0;

	class ContourAndSize {
	public:
		bool **picture;
		int height;
		int width;
		CvPoint leftTop;
		ContourAndSize(bool **contour, int h, int w, CvPoint corner) 
			: picture(contour), height(h), width(w), leftTop(corner)
		{
		}
	};
	// All new contours as image
	list<ContourAndSize> *allNewContours = 0;


	// Try to find text
	void tryToFindText();
	// Text
	list<Rect *> *allText = 0;

	// Try to delete cycle.
	void tryToDeleteCycle();
	// Main contour
	CvSeq *mainContour = 0;
};

