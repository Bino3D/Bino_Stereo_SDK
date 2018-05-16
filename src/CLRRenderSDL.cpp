#include "CLRRenderSDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "CLRRenderSDL.h"

#define WINDOWS_WIDTH  1000
#define WINDOWS_HEIGHT 600
#define UP      0xf0000000
#define DOWN    0x0f000000
#define LEFT    0x00f00000
#define RIGHT   0x00ff0000
#define CENTRE  0x0000ff00
#define pi   3.141593
float unitdata[4];
float right[4]={1.0,0.0,0.0,0.0};
unsigned int get_keys(unsigned int buttons)
{
	unsigned int keys;
	keys = 0;
	/*
	//if((buttons & UP) == (unsigned int)UP){
	if((buttons & UP) != 0){
		keys = keys | (1<<4) ;
	}

	//if((buttons & DOWN) == (unsigned int)DOWN){
	if((buttons & DOWN) != 0){
		keys = keys | (1<<3) ;
	}

	//if((buttons & LEFT) == (unsigned int)LEFT){
	if((buttons & LEFT) != 0){
		keys = keys | (1<<2) ;
	}
	 */
	//if((buttons & RIGHT) == (unsigned int)RIGHT){
	if((RIGHT) != 0){
		keys = keys | (1<<4) ;
		//keys = keys | (1<<1) ;
	}

	if((CENTRE) != 0){//== (unsigned int)CENTRE){
		keys = keys | (1<<0) ;
	}
	return keys;
}

void sdlDie(const char *msg)
{
	printf("%s: %s\n", msg, SDL_GetError());
	SDL_Quit();
	exit(1);
}

void checkSDLError(int line = -1)
{
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}

void drawBox(float size, GLenum type)
{
	static float n[6][3] =
	{
			{ -1.0, 0.0, 0.0 },
			{ 0.0, 1.0, 0.0 },
			{ 1.0, 0.0, 0.0 },
			{ 0.0, -1.0, 0.0 },
			{ 0.0, 0.0, 1.0 },
			{ 0.0, 0.0, -1.0 }
	};
	static int faces[6][4] =
	{
			{ 0, 1, 2, 3 },
			{ 3, 2, 6, 7 },
			{ 7, 6, 5, 4 },
			{ 4, 5, 1, 0 },
			{ 5, 6, 2, 1 },
			{ 7, 4, 0, 3 }
	};
	float v[8][3];
	int i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

	for (i = 5; i >= 0; i--)
	{
		glBegin(type);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void drawWireCube(float size)
{
	drawBox(size, GL_LINE_LOOP);
}

void drawSolidCube(float size)
{
	drawBox(size, GL_QUADS);
}

void
sdl_orientation_get_quaternion(float *reset_q, float*sample_q,
		float *q0, float *q1, float *q2, float *q3)
{
	//psmove_return_if_fail(orientation != NULL);

	/* first factor (reset quaternion) */
	float a_s = reset_q[0];//orientation->reset_quaternion[0];
	float a_x = reset_q[1];//orientation->reset_quaternion[1];
	float a_y = reset_q[2];//orientation->reset_quaternion[2];
	float a_z = reset_q[3];//orientation->reset_quaternion[3];

	/* second factor (quaternion) */
	float b_s = sample_q[0];//orientation->quaternion[0];
	float b_x = sample_q[1];//orientation->quaternion[1];
	float b_y = sample_q[2];//orientation->quaternion[2];
	float b_z = sample_q[3];//orientation->quaternion[3];

	/**
	 * Quaternion multiplication:
	 * http://lxr.kde.org/source/qt/src/gui/math3d/qquaternion.h#198
	 **/
	float ww = (a_z + a_x) * (b_x + b_y);
	float yy = (a_s - a_y) * (b_s + b_z);
	float zz = (a_s + a_y) * (b_s - b_z);
	float xx = ww + yy + zz;
	float qq = .5f * (xx + (a_z - a_x) * (b_x - b_y));

	/* Result */
	float r_s = qq - ww + (a_z - a_y) * (b_y - b_z);
	float r_x = qq - xx + (a_x + a_s) * (b_x + b_s);
	float r_y = qq - yy + (a_s - a_x) * (b_y + b_z);
	float r_z = qq - zz + (a_z + a_y) * (b_s - b_x);

	if (q0) {
		*q0 = r_s;
	}

	if (q1) {
		*q1 = r_x;
	}

	if (q2) {
		*q2 = r_y;
	}

	if (q3) {
		*q3 = r_z;
	}
}

void
sdl_orientation_reset_quaternion(float *frame_q, float *reset_q )
{
	//psmove_return_if_fail(orientation != NULL);
	float q0 = frame_q[0];//orientation->quaternion[0];
	float q1 = frame_q[1];//orientation->quaternion[1];
	float q2 = frame_q[2];//orientation->quaternion[2];
	float q3 = frame_q[3];//orientation->quaternion[3];

	/**
	 * Normalize and conjugate in one step:
	 *  - Normalize via the length
	 *  - Conjugate using (scalar, x, y, z) -> (scalar, -x, -y, -z)
	 **/
	double length = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	reset_q[0] = (float)(q0 / length);
	reset_q[1] = (float)(-q1 / length);
	reset_q[2] = (float)(-q2 / length);
	reset_q[3] = (float)(-q3 / length);
}
CLRRenderSDL::CLRRenderSDL()
: m_window(NULL)
//m_tracker(tracker)
{
	SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		sdlDie("Unable to initialize SDL");
	}

	m_window = SDL_CreateWindow("BinoIMU",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOWS_WIDTH, WINDOWS_HEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (m_window == NULL)
	{
		sdlDie("Unable to initialize SDL");
	}
	checkSDLError(__LINE__);

	m_glContext = SDL_GL_CreateContext(m_window);
	checkSDLError(__LINE__);

	projection = glm::perspectiveFov<float>(90, WINDOWS_WIDTH, WINDOWS_HEIGHT, 1, 200);
	viewport = glm::vec4(0.,0., WINDOWS_WIDTH, WINDOWS_HEIGHT);
	background_image = (unsigned char *)malloc(WINDOWS_WIDTH*WINDOWS_HEIGHT*3);
	//bt = bluetooth;
	//bt = new CLRMoveBlueToothTest();
	reset_q = false;
	reset_q_array[0] = 1.0;
	reset_q_array[1] = 0.0;
	reset_q_array[2] = 0.0;
	reset_q_array[3] = 0.0;
	frame_q_array[0] = 1.0;
	frame_q_array[1] = 0.0;
	frame_q_array[2] = 0.0;
	frame_q_array[3] = 0.0;
}
CLRRenderSDL::~CLRRenderSDL()
{
	SDL_Quit();
}

void
CLRRenderSDL::init()
{
	glClearColor(0., 0., 0., 1.);

	//glViewport(0, 0, 640, 480);
	glViewport(0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT);

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

bool CLRRenderSDL::render(float *outQ, unsigned int *keys)
{
	//m_tracker.render();
	//外部循环
	if(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT){
			return false;
		}
		switch(e.type){
		case SDL_KEYDOWN:
			printf("reset\n");
			if(e.key.keysym.sym == SDLK_r){

			}
			break;
		}
	}

	if(e.key.keysym.sym == SDLK_q){
		//exit();
		return false;
	}

	unsigned int buttons;

	getQ(sys_q,&buttons);  // |0|0|0|0|-|0|0|0|0| = |上|下|左|右|-|中间|0|0|0|

	*keys = get_keys(buttons);

	outQ[3] = sys_q[7];//0.0; //z
	outQ[2] = sys_q[6];//0.0; //y
	outQ[1] = sys_q[5];//0.0; //x
	outQ[0] = sys_q[4];//1.0; //w
	renderr();
	SDL_GL_SwapWindow(m_window);
	return true;
}

void A_to_Q(float *ang,float *q0, float *q1, float *q2, float *q3,int i)
{
	float qbuff[4]={0.0};
	ang[1]=ang[1]>0?(ang[1]):(-ang[1]);
	ang[2]=ang[2]>0?(ang[2]):(-ang[2]);
	ang[3]=ang[3]>0?(ang[3]):(-ang[3]);
	qbuff[0]=-ang[0]*(ang[i]/(ang[1]+ang[2]+ang[3]));
	qbuff[i]=1.0;
	*q1= qbuff[1] * sin(qbuff[0]/2);
	*q2= qbuff[2] * sin(qbuff[0]/2);
	*q3= qbuff[3] * sin(qbuff[0]/2);
	*q0 = cos(qbuff[0]/2);
}

bool CLRRenderSDL::renderByInputQ(float *inQ)
{

	float inQr[4];
	//外部循环
	if(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT){
			return false;
		}
		switch(e.type){
		case SDL_KEYDOWN:
			if(e.key.keysym.sym == SDLK_r){
				printf("reset\n");
				reset_q = true;
				/*********************校准***************************************/
				float data[4];
				float ang[4];
				float anQ[4];
				memset(right,0,sizeof(right));
				memset(data,0,sizeof(data));
				for(int i=0;i<4;i++)
				{
					data[i]=unitdata[i];
				}

				/**** 将得到的四元数转换为角-轴******/
				ang[0] = 2 * acos(data[0]);
				ang[1] = data[1] / sqrt(1-data[0]*data[0]) ;
				ang[2] = data[2] / sqrt(1-data[0]*data[0]) ;
				ang[3] = data[3]/ sqrt(1-data[0]*data[0]) ;
				/**********************************/

				memset(data,0,sizeof(data));

				/****将得到的角轴转回初始位置********/
				data[0]=-ang[0];
				data[1]=ang[1];
				data[2]=ang[2];
				data[3]=ang[3];
				/***********************************/

				/***转换后的角轴转变为四元数**********/
				anQ[1]= data[1] * sin(data[0]/2);
				anQ[2]= data[2] * sin(data[0]/2);
				anQ[3]=data[3] * sin(data[0]/2);
				anQ[0] = cos(data[0]/2);
				/************************************/
				for(int i=0;i<4;i++)
				{
					right[i]=anQ[i];
				}
			}
			if(e.key.keysym.sym == SDLK_q){
				printf("quit\n");
				exit(0);
				return false;
			}
			break;
		}
	}


	unitdata[3]=inQ[3];
	unitdata[2]=inQ[2];
	unitdata[1]=inQ[1];
	unitdata[0]=inQ[0];
	sdl_orientation_get_quaternion(right,inQ,&inQr[0],&inQr[1],&inQr[2],&inQr[3]);
	sys_q[7] =inQr[3];//0.0; //z
	sys_q[6] =inQr[2];//0.0; //y
	sys_q[5] =inQr[1];//0.0; //x
	sys_q[4] =inQr[0];//1.0; //w
	renderr();
	SDL_GL_SwapWindow(m_window);
	return true;
}


void CLRRenderSDL::getQ(float *a_q,unsigned int *buttons){
}

void CLRRenderSDL::renderr()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);
	float q3 = sys_q[7];//0.0; //z  //opengl的y轴对应mpu的z轴
	float q2 = sys_q[6];//0.0; //y
	float q1 = sys_q[5];//0.0; //x
	float q0 = sys_q[4];//1.0; //w
	glm::quat quaternion1(q1, q3, q2 ,q0);
	//glm::quat quaternion1(q3, q2, q1 ,q0);
	this->modelview = glm::translate(glm::mat4(), glm::vec3(00,00,-10)) * glm::mat4_cast(quaternion1);
	float *modelview2gl = glm::value_ptr(this->modelview);
	glLoadMatrixf(modelview2gl);
	glColor3f(0., 0., 1.);
	//glScalef(1.5, 0.5, 1.0);
	glScalef(1.0, 0.5, 1.5);
	drawWireCube(3.);
}
