#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <opencv2/opencv.hpp>
#include <BinoCamera.h>
#include <memory>

using namespace cv;
using namespace std;

static int print_help()
{
    cout <<
            " Given a list of chessboard images, the number of corners (nx, ny)\n"
            " on the chessboards, and a flag: useCalibrated for \n"
            "   calibrated (0) or\n"
            "   uncalibrated \n"
            "     (1: use cvStereoCalibrate(), 2: compute fundamental\n"
            "         matrix separately) stereo. \n"
            " Calibrate the cameras and display the\n"
            " rectified results along with the computed disparity images.   \n" << endl;
    cout << "Usage:\n ./stereo_calib -w board_width -h board_height [-nr /*dot not view results*/] <image list XML/YML file>\n" << endl;
    return 0;
}
static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}

int main(int argc, char** argv)
{
	if(argc != 4 )
	{
		if(argc != 3){
			cerr << endl << "Usage: ./CalibStereo /dev/video* path_to_intrinsics path_to_extrinsics  OR ./stereo_bino /dev/video* path_to_fisheye  "  << endl;
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
    Size boardSize;
    string imagelistfn;
	float squareSize = 0.02;
    boardSize = Size(11, 7);
	std::shared_ptr<BinoCamera> camera(new BinoCamera(paraList));
    if( imagelistfn == "" )
    {
        imagelistfn = "calib/stereo_calib.xml";
    }
    else if( boardSize.width <= 0 || boardSize.height <= 0 )
    {
        cout << "if you specified XML file with chessboards, you should also specify the board width and height (-w and -h options)" << endl;
        return 0;
    }
    vector<string> imagelist;
    bool ok = readStringList(imagelistfn, imagelist);
    if(!ok || imagelist.empty())
    {
        cout << "can not open " << imagelistfn << " or the string list is empty" << endl;
        return print_help();
    }
	cout << "size = " << imagelist.size() << endl; 
	camera->StartStereoCalib(imagelist, boardSize, squareSize);
    return 0;
}
