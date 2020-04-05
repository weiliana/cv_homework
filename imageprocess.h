#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
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
    static void suft();
    static void bruteForce();
    static void FLANN();
    static void RANSAC();
    static Mat QImage2cvMat(QImage image);      //将QImage类型图片转换为cv可以处理的矩阵类型
    static QImage cvMat2QImage(const Mat& mat); //将MAT类型图片转换为QImage
};

#endif // IMAGEPROCESS_H
