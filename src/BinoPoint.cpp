#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <BinoCamera.h>
#include <memory>
using namespace std;

int main(int argc, char **argv){

	if(argc != 4 )
	{
		if(argc != 3){
			cerr << endl << "Usage: ./BinoPoint /dev/video* path_to_intrinsics path_to_extrinsics  OR ./stereo_bino /dev/video* path_to_fisheye  "  << endl;
			return 1;
		}
	}

	BinoCameraParameterList paraList;

	paraList.devPath = argv[1];
	if(argc == 4){
		paraList.intParameterPath = argv[2];
		paraList.extParameterPath = argv[3];
	}else if(argc == 3){
		paraList.fisheyeParameterPath = argv[2];
	}

	int image_w = 1000;
	int image_h = 1000;
	WindowsParams params; 
	params.size.x = 0.25;
	params.size.y = 0.40;
	params.size.z = 20.0;
	params.pitch = 0;
	NearestPoints points[360];
	cv::Point3d nearpoint;
	std::shared_ptr<BinoCamera> camera(new BinoCamera(paraList));
	for(;;){
		camera->Grab();
	    camera->getNearestPoint(params, points, nearpoint);
		cv::Mat PointImage(image_h, image_w, CV_8UC4, cv::Scalar::all(0));
		//PointImage.clear();
		camera->getPointImage(points, nearpoint, PointImage);
		cv::imshow("line_image", PointImage);
		int key = cv::waitKey(1);
		if(key == 'q')
            break;
	}
	return 0;
}
