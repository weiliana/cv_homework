#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QImage>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc/types_c.h>
#include "utils.h"
using namespace cv;

class ImageProcess
{
public:
    ImageProcess();

    static void point2point(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian);          //点对映射,返回值类型未知，参数个数类型未知
    static void geometricCorrection(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian);  //几何校正,具体有几个参数未知,自行更改..
    static void imageMosaic(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian);          //图像拼接
    static void targetDetect(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian);         //目标检测,返回值类型未知，参数个数类型未知

    // opencv中有可以调用的函数时，用opencv的进行替换
    static void sift(Mat cvImg1,Mat cvImg2,int Hessian,std::vector<KeyPoint> &keyPoints1,std::vector<KeyPoint> &keyPoints2,Mat &imageDesc1,Mat &imageDesc2);                 //在具体功能中根据索引调用具体的特征提取方法和具体的特征匹配方法
    static void surf(Mat cvImg1,Mat cvImg2,int Hessian,std::vector<KeyPoint> &keyPoints1,std::vector<KeyPoint> &keyPoints2,Mat &imageDesc1,Mat &imageDesc2);
    static void bruteForce(Mat imageDesc1,Mat imageDesc2,std::vector<DMatch> &matches);
    static void FLANN(Mat imageDesc1,Mat imageDesc2,std::vector<DMatch> &matches);
    static void RANSAC(vector<KeyPoint> keyPoints1,vector<KeyPoint> keyPoints2,vector<DMatch> matches,vector<DMatch> &goodMatches,vector<KeyPoint> &new_rand_keypoint1,vector<KeyPoint> &new_rand_keypoint2,vector <DMatch> &new_matches,vector<Point2f> &points_1,vector<Point2f> &points_2);
    static void commonProcess(int featureSelectionIndex,int featureMatchIndex,Mat cvImg1,Mat cvImg2,Mat &resultImg,int Hessian,vector<KeyPoint> &keyPoints1,vector<KeyPoint> &keyPoints2,vector<DMatch> &matches);

    static Mat preprocessImg(Mat srcImg);
    static Point2f getTransformPoint(const Point2f originalPoint,const Mat & transformMatrix);
};

#endif // IMAGEPROCESS_H
