#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <BinoCamera.h>
#include <memory>
using namespace std;

int main(int argc, char **argv){

	if(argc != 5 )
	{
		if(argc != 4){
			cerr << endl << "Usage: ./BinoStereo /dev/video* path_to_intrinsics path_to_extrinsics raw/rect/disparity OR ./stereo_bino /dev/video* path_to_fisheye raw/rect/disparity  "  << endl;
			return 1;
		}
	}
	std::string run_para;

	BinoCameraParameterList paraList;

	paraList.devPath = argv[1];
	if(argc == 5){
		paraList.intParameterPath = argv[2];
		paraList.extParameterPath = argv[3];
		run_para = argv[4];
	}else if(argc == 4){
		paraList.fisheyeParameterPath = argv[2];
		run_para = argv[3];
	}

	cv::Mat left;
	cv::Mat right;
	cv::Mat para;
	cv::Mat disp;
	int cnt = 0;
	std::shared_ptr<BinoCamera> camera(new BinoCamera(paraList));
	for(;;){
		camera->Grab();
		if(run_para == "rect"){
			camera->getRectImage(left, right);
			cv::imshow("image_rect_left", left);
			cv::imshow("image_rect_right", right);
			
		}
		else if(run_para == "raw"){
			camera->getOrgImage(left, right);
			cv::imshow("image_left", left);
			cv::imshow("image_right", right);
		}
		else if(run_para == "disparity"){
			camera->getDisparity(disp);
			disp.convertTo( disp, CV_8U, 255.0/(48*16) );
			cv::applyColorMap( disp, disp, cv::COLORMAP_JET );
			cv::imshow("image_disparity", disp);
		}

		int key = cv::waitKey(1);
		if(key == 'q')
            break;
		else if(key == 's'){
			cv::imwrite("/home/li/workspace/Bino_Stereo_SDK/pic/l" + std::to_string(cnt) + ".png", left);
			cv::imwrite("/home/li/workspace/Bino_Stereo_SDK/pic/r" + std::to_string(cnt) + ".png", right);
			cnt++;
		}
	}
	return 0;
}
