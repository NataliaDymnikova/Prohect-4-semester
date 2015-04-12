#include <iostream>
#include "BoolPicture.h"
#include "Contours.h"
#include "Lines.h"
#include "SearchRelations.h"

using namespace std;

int main() {
	BoolPicture *picture = new BoolPicture("../4.png");
	bool **boolPic = picture->getPicture();
	/*for (int i = 0; i < picture->getHeight(); i++) {
		for (int j = 0; j < picture->getWidth(); j++) {
			cout << boolPic[i][j] ? "0" : "1";
		}
		cout << endl;
	}*/
/*	namedWindow("window");
	imshow("window", imageBW);
	int k = 0;
	while (k != '0')
		k = cvWaitKey();
	*/
	Contours *cont = new Contours();
	Lines *lines = new Lines(cont->imageContour);
	cvDestroyAllWindows();

	SearchRelations *search = new SearchRelations(lines->linesList, cont->all);

	delete picture;
	delete cont;
	delete lines;
	return 0;
}