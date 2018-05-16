# Bino_Camera_SDK
## **注意由于固件更新，2018年5月16日前出货的双目产品已经不能使用此版本SDK。请老客户通过"Bino双目技术支持"群联系我们，望谅解。**

## 1.使用前准备

本SDK目前只支持linux,测试版本为[ubuntu16.04](https://www.ubuntu.com/download/desktop)

#### 1.1安装build工具
```
$sudo apt-get install build-essential cmake git
```
#### 1.2安装OpenCV依赖
```
$sudo apt-get install pkg-config libgtk2.0-dev
```
#### 1.3安装摄像头驱动依赖
```
$sudo apt-get install libssl-dev libv4l-dev v4l-utils
```

#### 1.4安装OpenCV
##### 快速安装
```
$git clone https://github.com/opencv/opencv.git
$cd opencv/
$git checkout tags/3.2.0
$mkdir build
$cd build/
$cmake ..
$make
```
##### [OpenCV详细配置请见OpenCV官方Git](https://github.com/opencv/opencv)

#### 1.5安装OpenGL及SDL2
##### 快速安装
```
$sudo apt-get install libgl1-mesa-dev
$sudo apt-get install freeglut3-dev
$sudo apt-get install libglew-dev libsdl2-dev libsdl2-image-dev libglm-dev libfreetype6-dev
```
##### [OpenGL详细配置请见OpenGL官网](https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/Linux)

##### [SDL2详细配置请见SDL2官网](https://www.libsdl.org/)

## 2.下载Bino_Stereo_SDK
```
$git clone https://github.com/Bino3D/Bino_Stereo_SDK.git
```

## 3.添加Bino_Stereo_SDK到环境变量
```
$echo "export BINO_STEREO_SDK=<sdk directory>" >> ~/.bashrc
$source ~/.bashrc
```

## 4.编译例子程序
```
$cd <sdk directory>
$./build.sh
```
## 5.快速使用例子程序
```
1 将摄像头插入主机
2 查看摄像头设备ID
$ll /dev/video*
3 将sdk目录下run_raw.sh 摄像头设备额ID修改为刚查看到的设备ID
$cd <sdk directory>
$vim run_raw.sh
```
<img src="https://github.com/Bino3D/Bino_Stereo_SDK/tree/master/srv/editDevID.png"
alt="修改设备id的位置" width="640" height="480" border="10" />

```
4 运行run_raw.sh 显示畸变矫正前图像
$./run_raw.sh

5 运行run_rect.sh 显示畸变矫正后图像
$./run_rect.sh

6 运行run_disp.sh 显示视差图
$./run_disp.sh

7 运行run_imu.sh 显示IMU姿态
$./run_imu.sh
```
* 注意：例子程序读取的畸变矫正参数在<sdk directory>/params中，若重新标定了镜头，请覆盖这两个文件再运行例子脚本

## 6.双目标定
```
双目标定由两个功能组成：棋盘格图像采集和双目标定
```
* 注意：示例中使用的是规格为11*7，单格棋盘格边长为0.02m的棋牌格标定板，若使用其他规格的棋盘格标定板，请自行修改src/CalibGetImage.cpp中的boardSize参数以及src/CalibStereo.cpp中的squareSize参数和boardSize参数，然后运行build.sh重新编译。

#### 6.1 棋盘格图像采集

```
1 进入Bino_Stereo_SDK文件夹，运行run_calib_getimage.sh脚本。
$cd <sdk directory>
$./run_calib_getimage.sh

2 点击键盘'S'按键同时采集左右目图片。
3 采集采集左右目图片各35张作为标定使用的图像集。
```

* 注意：
* 1 保证两个摄像头都能拍摄到完整棋盘格：效果应该如'srv/采集图像事例.png'所示.
* 2 保证两个摄像头拍摄的棋盘格的角点计算结果一致: 红色角点在上，紫色角点在下，效果如'srv/采集图像事例.png'。
* 3 采集多种姿态的棋盘格图片：可参照 'srv/棋盘格姿态样例.pdf'

#### 6.2 双目标定
```
进入Bino_Stereo_SDK文件夹，运行run_calib_stereo.sh脚本。
$cd <sdk directory>
$./run_calib_stereo.sh

标定程序分三个阶段
1 采集图片读入及棋盘格角点计算检测;此阶段请检查每张读入图片的棋盘格角点计算顺序是否一致。若一致，按空格键跳转到下一张图片，继续检查;
  若不一致，查看命令行的打印，记住出错图片的编号，退出程序后打开calib文件夹下的stereo_calib.xml文件，将出错编号对应的左右图的索引删掉，然后重新运行程序。
2 直到命令行打印“Running stereo calibration ...”，表示标定程序开始运行，等待标定结束。
3 标定结束后会显示标定后的图像，用户可查看标定的结果是否正确。
```
## 7.API手册
```
使用浏览器打开
<sdk directory>/doc/html/index.html  
查看
```
## 8.购买链接
[链接](https://item.taobao.com/item.htm?spm=a230r.1.14.36.30447e471pexT6&id=562336856228&ns=1&abbucket=1#detail)

## 9.相关论文
* Cai Luo, Leijian Yu, Peng Ren, A Vision-aided Approach to Perching a Bio-inspired Unmanned Aerial Vehicle, IEEE Transactions on Industrial Electronics, DOI: 10.1109/TIE.2017.2764849.
* Cai Luo, Xiu Li, Yipeng Li, Qionghai Dai, Biomimetic Design for Unmanned Aerial Vehicle SafeLanding in Hazardous Terrain, IEEE/ASME Transactions on Mechatronics, 2016.02.01, 21(1):31 541.
* Cai Luo, Xiu Li, Qionghai Dai, Biology’s drones: New and improved, Science, 2014.6.20, 344(6190):1351 1351.
