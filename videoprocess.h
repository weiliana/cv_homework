#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
using namespace cv;

class VideoProcess
{
public:
    VideoProcess();
    static void videoCheckSynchronized();//返回值类型未知，参数类型个数未知..

    // opencv中有可以调用的函数时，用opencv的进行替换
    static void FAST(QImage image);
    static void ORB(QImage image);
    static void RANSAC(QImage image);
};

#endif // VIDEOPROCESS_H
