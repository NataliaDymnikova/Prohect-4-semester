#include <iostream>
#include "BoolPicture.h"

using namespace std;

int main() {
	BoolPicture *picture = new BoolPicture("../1.png");
	bool **boolPic = picture->getPicture();
	for (int i = 0; i < picture->getHeight(); i++) {
		for (int j = 0; j < picture->getWidth(); j++) {
			cout << boolPic[i][j] ? "0" : "1";
		}
		cout << endl;
	}
/*	namedWindow("window");
	imshow("window", imageBW);
	int k = 0;
	while (k != '0')
		k = cvWaitKey();
	*/

	delete picture;
	return 0;
}