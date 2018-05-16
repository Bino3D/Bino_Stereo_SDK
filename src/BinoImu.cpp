#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <BinoCamera.h>
#include <unistd.h>
#include <memory>
#include <thread>
#include <mutex>
#include <pthread.h>
#include "CLRRenderSDL.h"
using namespace std;
float uartQMain[4] = {0};

/***IMU死区***/
float deadmin = -0.8; 
float deadmax = 0.8;

/***IMU加速度计零偏***/
float axerr = 0.0045; 
float ayerr = -0.0266; 
float azerr = -0.0100;

pthread_mutex_t lattice_mutex;
pthread_mutex_t point_mutex;
BinoCameraParameterList paraList;

//读IMU数据
void *getImuData( void *arg ){
	std::vector<ImuData> imudatas;
	ImuData imudata;
	uint32_t current_time=0,old_time=0;
	float timestamp = 0.0;	
	uint32_t time_image = 0;
	float q[4] = {0};
	std::shared_ptr<BinoCamera> camera(new BinoCamera(paraList));
	camera->setImuCorrection(deadmin,deadmax,axerr,ayerr,azerr);
	for(;;){
		camera->Grab();
		camera->getImuRawData(imudatas,time_image);
		size_t size = imudatas.size();
		for(uint8_t i = 0;i < size;i++){
			imudata = imudatas[i];
			current_time = imudata.time;	
			timestamp = (float)((current_time -old_time)/1000.0f);
			camera->ImuRect(imudata,timestamp,q);
			old_time = current_time;
			pthread_mutex_lock(&lattice_mutex);
			for(int i= 0;i<4;i++){
				uartQMain[i]=q[i];
			}
			pthread_mutex_unlock(&lattice_mutex);
		}
		pthread_testcancel();  
	}
	pthread_exit(NULL);
	return 0;
}

//显示
void *drawImuAttitude(void *arg)
{
	float out[4] = {0};
	CLRRenderSDL *renderSDL = new CLRRenderSDL();
	renderSDL->init();
	sleep(1);

	while(1){
		pthread_mutex_lock(&lattice_mutex);

		for(int i= 0;i<4;i++){
			out[i] = uartQMain[i];
		}
		pthread_mutex_unlock(&lattice_mutex);

		if(!renderSDL->renderByInputQ(out))
		{
			break;
		}
		 pthread_testcancel(); 
	}
	pthread_exit(NULL);
	return 0;
}


int main(int argc, char **argv){
	if(argc != 4)
	{
		cerr << endl << "Usage: ./BinoImu /dev/video* path_to_intrinsics path_to_extrinsics" << endl;
		return 1;
	}
	paraList.devPath = argv[1];
	paraList.intParameterPath = argv[2];
	paraList.extParameterPath = argv[3];
	pthread_t threads[2];
	pthread_create(&threads[0],NULL,&getImuData,NULL);
	pthread_create(&threads[1],NULL,&drawImuAttitude,NULL);
	while(1);
	pthread_cancel(threads[0]); 
	pthread_cancel(threads[1]); 
	//pthread_join(tid, &ret);
	pthread_exit(NULL);
	return 0;
}
