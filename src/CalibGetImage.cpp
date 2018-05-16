#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <memory>
#include <functional>
#include <atomic>
#include <stdio.h>
#include <BinoCamera.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
std::int32_t timestamp=61735;
int main(int argc, char **argv)
{
	if(argc != 4)
	{
		cerr << endl << "usage: ./CalibGetImage /dev/video* path_to_intrinsics path_to_extrinsics" << endl;
		return 1;
	}
	BinoCameraParameterList paraList;
	paraList.devPath = argv[1];
	paraList.intParameterPath = argv[2];
	paraList.extParameterPath = argv[3];
	std::shared_ptr<BinoCamera> camera(new BinoCamera(paraList));
	cv::Mat imageLeft;
	cv::Mat imageRight;
	cv::Size boardSize = cv::Size(11, 7);
	bool isFoundLeft, isFoundRight;
	int cnt = 0;
	for(;;) {
		camera->Grab();
		camera->getOrgImage(imageLeft, imageRight);
		std::vector<cv::Point2f> corenrsL;
		std::vector<cv::Point2f> corenrsR;
		cv::Mat showMatLeft(imageLeft.rows, imageLeft.cols, CV_8UC3, cv::Scalar::all(0));
		cv::Mat showMatRight(imageRight.rows, imageRight.cols, CV_8UC3, cv::Scalar::all(0));
		isFoundLeft = cv::findChessboardCorners(imageLeft, boardSize, corenrsL, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		isFoundRight = cv::findChessboardCorners(imageRight, boardSize, corenrsR, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

		if(isFoundLeft && isFoundRight){
			cv::cvtColor(imageLeft, showMatLeft, CV_GRAY2BGR);
			cv::cvtColor(imageRight, showMatRight, CV_GRAY2BGR);
			cv::drawChessboardCorners(showMatLeft, boardSize, corenrsL, true);
			cv::drawChessboardCorners(showMatRight, boardSize, corenrsR, true);
		}else{
			showMatLeft = imageLeft.clone();
			showMatRight = imageRight.clone();
		}
		cv::imshow("image_left", showMatLeft);
		cv::imshow("image_right", showMatRight);
		int key = cv::waitKey(1);
		if(key == 'q')
			break;
		else if(key == 's'){
			stringstream num;
			num << format("%02d", cnt);;
			cv::imwrite("calib/left" + num.str() + ".jpg", imageLeft);
			cv::imwrite("calib/right" + num.str() + ".jpg", imageRight);
			cout << "save image" << cnt << " success!" << endl;
			cnt++;
		}

	}
	return 0;
}

