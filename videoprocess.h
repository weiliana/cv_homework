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
    static bool getVideoStatus();
    static void setVideoStatus(bool isOn);
    static void videoCheckSynchronized(QString imagePath, QString videoPath, bool useCamera = false);//返回值类型未知，参数类型个数未知..

private:
    // opencv中有可以调用的函数时，用opencv的进行替换
    static void mRANSAC(vector<KeyPoint> &target, vector<KeyPoint> &scene, vector<DMatch> &matches);
    static void goodMatch(vector<DMatch> &matches);
};

#endif // VIDEOPROCESS_H
