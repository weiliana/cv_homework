#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QImage>
#include <opencv2/opencv.hpp>
using namespace cv;

class ImageProcess
{
public:
    ImageProcess();

    static void point2point();          //点对映射,返回值类型未知，参数个数类型未知
    static void geometricCorrection();  //几何校正,具体有几个参数未知,自行更改..
    static void imageMosaic();          //图像拼接
    static void targetDetect();         //目标检测,返回值类型未知，参数个数类型未知

    // opencv中有可以调用的函数时，用opencv的进行替换
    static void sift();                 //在具体功能中根据索引调用具体的特征提取方法和具体的特征匹配方法
    static void surf();
    static void bruteForce();
    static void FLANN();
    static void RANSAC();
};

#endif // IMAGEPROCESS_H
