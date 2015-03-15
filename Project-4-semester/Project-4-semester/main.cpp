#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace cv;

/// Convert imageColor to black and white image.
Mat convertToBlackAndWhite(Mat imageColor) {
	// imageColor -> imageGrey -> imageBlackAndWhite. 
	Mat imageGrey;
	cvtColor(imageColor, imageGrey, CV_RGB2GRAY);
	return imageGrey > 128;
}

int main() {
	// imageColor -> imageGrey -> imageBW. 
	Mat imageColor = imread("../00.jpg");
	Mat imageBW = convertToBlackAndWhite(imageColor);
	namedWindow("window");
	imshow("window", imageBW);
	int k = 0;
	while (k != '0')
		k = cvWaitKey();
	return 0;
}