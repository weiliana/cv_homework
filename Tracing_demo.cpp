// CV_VidoeTracing.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <opencv2/core/utility.hpp>  
#include <opencv2/tracking.hpp>  
#include <opencv2/videoio.hpp>  
#include <opencv2/highgui.hpp>  
#include <iostream>  
#include <cstring>  

using namespace std;
using namespace cv;

int main() {
	// declares all required variables  
	//! [vars]  
	Rect2d roi;
	Mat frame;
	//! [vars]  

	// create a tracker object  
	Ptr<Tracker> tracker = TrackerKCF::create(); //KCF追踪器
	//Ptr<Tracker> tracker = TrackerTLD::create(); //TLD追踪器
	//Ptr<Tracker> tracker = TrackerGOTURN::create(); //GOTURN追踪器，尚不能运行
	//! [create]  

	// set input video  
	//! [setvideo]  
	string video = "E:\\SCUT\\ComputerVision\\Homework2\\CarScale.avi";
	VideoCapture cap(video);
	//cap.open(video);

	//! [setvideo]  

	// get bounding box  
	//! [getframe]  
	cap >> frame;
	//! [getframe]  
	//! [selectroi]选择目标roi以GUI的形式  
	roi = selectROI("tracker", frame);
	//! [selectroi]  

	//quit if ROI was not selected  
	if (roi.width == 0 || roi.height == 0)
		return 0;

	// initialize the tracker  
	//! [init]  
	tracker->init(frame, roi);
	//! [init]  

	// perform the tracking process  
	//printf("Start the tracking process\n");
	for (;; ) {
		// get frame from the video  
		cap >> frame;

		// stop the program if no more images  
		if (frame.rows == 0 || frame.cols == 0)
			break;

		// update the tracking result  
		//! [update]  
		tracker->update(frame, roi);
		//! [update]  

		//! [visualization]  
		// draw the tracked object  
		rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);

		// show image with the tracked object  
		imshow("tracker", frame);
		//! [visualization]  
		//quit on ESC button  
		if (waitKey(1) == 27)
			break;
	}

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
