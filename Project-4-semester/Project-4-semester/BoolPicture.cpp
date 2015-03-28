#include "BoolPicture.h"

BoolPicture::BoolPicture(string nameOfPicture)
{
	Mat imageColor = imread(nameOfPicture);
	width = imageColor.cols;
	height = imageColor.rows;
	Mat imageBW = convertToBlackAndWhite(imageColor);
	namedWindow("window");
	imshow("window", imageBW);
	int k = 0;
	while (k != '0')
		k = cvWaitKey();
	picture = convertBWToBool(imageBW);

}

bool **BoolPicture::getPicture() {
	return picture;
}

int BoolPicture::getHeight() {
	return height;
}

int BoolPicture::getWidth() {
	return width;
}

Mat BoolPicture::convertToBlackAndWhite(Mat imageColor) {
	// imageColor -> imageGrey -> imageBlackAndWhite. 
	Mat imageGrey;
	cvtColor(imageColor, imageGrey, CV_RGB2GRAY);
	return imageGrey > 128;
}

bool **BoolPicture::convertBWToBool(Mat picture) {
	// Picture -> uchar -> bool.
	uchar **array = new uchar*[picture.rows];
	bool **arrayBool = new bool*[picture.rows];
	for (int i = 0; i < picture.rows; i++) {
		array[i] = new uchar[picture.cols];
		arrayBool[i] = new bool[picture.cols];
	}
	for (int i = 0; i < picture.rows; i++) {
		array[i] = picture.ptr(i);
		for (int j = 0; j < picture.cols; j++)
			arrayBool[i][j] = array[i][j];
	}

	return arrayBool;
}

BoolPicture::~BoolPicture()
{
	for (int i = 0; i < height; i++) {
		delete picture[i];
	}
}
