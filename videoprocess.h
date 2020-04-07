#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H
#include <QImage>
#include <QString>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "utils.h"
using namespace cv;

class VideoProcess
{
public:
    VideoProcess();
    static void videoCheckSynchronized(QString imagePath, QString videoPath);//返回值类型未知，参数类型个数未知..

    // opencv中有可以调用的函数时，用opencv的进行替换
    static void RANSAC(QImage image);
};

#endif // VIDEOPROCESS_H
