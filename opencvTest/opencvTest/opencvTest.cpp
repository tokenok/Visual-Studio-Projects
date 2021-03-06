#include <iostream>
#include <opencv2/opencv.hpp>

#include "C:\CPPlibs\common\f\common.h"

using namespace std;
using namespace cv;

int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) 
		cout << "Cam failed\n";
	else {
		cv::namedWindow("Image");	

		while ((char)(waitKey(1)) != 'q' && cap.isOpened()) {
			Mat frame;
			cap >> frame;

			cv::imshow("Image", frame);
		}
	}

    return 0;
}