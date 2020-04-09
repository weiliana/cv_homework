#include "imageprocess.h"
#include <QDebug>


ImageProcess::ImageProcess()
{

}

void ImageProcess::point2point(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian)
{
    vector<KeyPoint> keyPoints1,keyPoints2;
    Mat cvImg1,cvImg2,imageDesc1,imageDesc2,resultImg;
    vector<DMatch> matches;
    cvImg1=Utils::QImage2cvMat(qimg1);
    cvImg2=Utils::QImage2cvMat(qimg2);
    if(featureSelectionIndex==0)
    {
        sift(cvImg1,cvImg2,Hessian,keyPoints1,keyPoints2,imageDesc1,imageDesc2);
    }
    else
    {
        surf(cvImg1,cvImg2,Hessian,keyPoints1,keyPoints2,imageDesc1,imageDesc2);
    }
    if(featureMatchIndex==0)
    {
        bruteForce(imageDesc1,imageDesc2,matches);
    }
    else
    {
        FLANN(imageDesc1,imageDesc2,matches);
    }
    if(!RANSAC_on)
    {
        drawMatches(cvImg1,keyPoints1,cvImg2,keyPoints2,matches,resultImg);
    }
    else
    {
        vector<KeyPoint> rand_keyPoints1,rand_keyPoints2;
        vector<DMatch> rand_matches;
        RANSAC(keyPoints1,keyPoints2,matches,rand_keyPoints1,rand_keyPoints2,rand_matches);
        drawMatches(cvImg1,rand_keyPoints1,cvImg2,rand_keyPoints2,rand_matches,resultImg);
    }
    imshow("Point to point",resultImg);
}

void ImageProcess::geometricCorrection()
{

}

void ImageProcess::imageMosaic()
{

}

void ImageProcess::targetDetect()
{

}



void ImageProcess::sift(Mat cvImg1,Mat cvImg2,int Hessian,vector<KeyPoint> &keyPoints1,vector<KeyPoint> &keyPoints2,Mat &imageDesc1,Mat &imageDesc2)
{
    Mat cvImg1_gray=preprocessImg(cvImg1);
    Mat cvImg2_gray=preprocessImg(cvImg2);

    Ptr<xfeatures2d::SIFT> siftDetector = xfeatures2d::SIFT::create(Hessian);
    //std::vector<KeyPoint> keyPoints1,keyPoints2;
    siftDetector->detect(cvImg1_gray,keyPoints1);
    siftDetector->detect(cvImg2_gray,keyPoints2);

    Ptr<xfeatures2d::SIFT> siftDescriptor = xfeatures2d::SIFT::create();
    //Mat imageDesc1, imageDesc2;
    siftDescriptor->compute(cvImg1_gray, keyPoints1, imageDesc1);
    siftDescriptor->compute(cvImg2_gray, keyPoints2, imageDesc2);
}

void ImageProcess::surf(Mat cvImg1,Mat cvImg2,int Hessian,vector<KeyPoint> &keyPoints1,vector<KeyPoint> &keyPoints2,Mat &imageDesc1,Mat &imageDesc2)
{
    Mat cvImg1_gray=preprocessImg(cvImg1);
    Mat cvImg2_gray=preprocessImg(cvImg2);

    Ptr<xfeatures2d::SURF> surfDetector = xfeatures2d::SURF::create(Hessian);
    //std::vector<KeyPoint> keyPoints1,keyPoints2;
    surfDetector->detect(cvImg1_gray,keyPoints1);
    surfDetector->detect(cvImg2_gray,keyPoints2);

    Ptr<xfeatures2d::SURF> surfDescriptor = xfeatures2d::SURF::create();
    //Mat imageDesc1, imageDesc2;
    surfDescriptor->compute(cvImg1_gray, keyPoints1, imageDesc1);
    surfDescriptor->compute(cvImg2_gray, keyPoints2, imageDesc2);
}

void ImageProcess::bruteForce(Mat imageDesc1,Mat imageDesc2,vector<DMatch> &matches)
{
    BFMatcher bfmatcher;
    //std::vector<DMatch> matches;  //存储里面的一些点的信息
    bfmatcher.match(imageDesc1, imageDesc2, matches, Mat());
}

void ImageProcess::FLANN(Mat imageDesc1,Mat imageDesc2,vector<DMatch> &matches)
{
    FlannBasedMatcher FLANNmatcher;
    //std::vector<DMatch> matches;
    FLANNmatcher.match(imageDesc1, imageDesc2, matches, Mat());
}

void ImageProcess::RANSAC(vector<KeyPoint>keyPoints1,vector<KeyPoint>keyPoints2,vector<DMatch> matches,vector<KeyPoint> &new_rand_keypoint1,vector<KeyPoint> &new_rand_keypoint2,vector <DMatch> &new_matches)
{
    double min_dist = matches[0].distance, max_dist = matches[0].distance;
    for (size_t m = 0; m < matches.size(); m++)
     {
         if (matches[m].distance < min_dist)
         {
             min_dist = matches[m].distance;
         }
         if (matches[m].distance > max_dist)
        {
            max_dist = matches[m].distance;
        }
    }
    //筛选出较好的匹配点
    vector<DMatch> goodMatches;
    for (size_t m = 0; m < matches.size(); m++)
    {
         if (matches[m].distance < 2 * min_dist)
         {
            goodMatches.push_back(matches[m]);
        }
    }

    //RANSAC匹配过程
    vector<DMatch> old_matches;
    old_matches = goodMatches;
    int ptCount = goodMatches.size();

    //坐标转换为float类型
    vector <KeyPoint> rand_keypoint1, rand_keypoint2;
    //size_t是标准C库中定义的，应为unsigned int，在64位系统中为long unsigned int,在C++中为了适应不同的平台，增加可移植性。
    for (size_t i = 0; i < old_matches.size(); i++)
    {
        rand_keypoint1.push_back(keyPoints1[goodMatches[i].queryIdx]);
        rand_keypoint2.push_back(keyPoints2[goodMatches[i].trainIdx]);
    }
    //坐标变换
    vector <Point2f> points_1, points_2;
    for (size_t i = 0; i < old_matches.size(); i++)
    {
        points_1.push_back(rand_keypoint1[i].pt);
        points_2.push_back(rand_keypoint2[i].pt);
    }
    vector<uchar> RansacStatus;
    Mat Fundamental = findFundamentalMat(points_1, points_2, RansacStatus, FM_RANSAC);
    //重新定义关键点new_rand_keypoint和new_matches来存储新的关键点和基础矩阵，通过RansacStatus来删除误匹配点
    //vector <KeyPoint> new_rand_keypoint1, new_rand_keypoint2;
    //vector <DMatch> new_matches;
    int index = 0;
    for (size_t i = 0; i < old_matches.size(); i++)
    {
        if (RansacStatus[i] != 0)
        {
            new_rand_keypoint1.push_back(rand_keypoint1[i]);
            new_rand_keypoint2.push_back(rand_keypoint2[i]);
            old_matches[i].queryIdx = index;
            old_matches[i].trainIdx = index;
            new_matches.push_back(old_matches[i]);
            index++;
        }
    }
    //Mat Ransac_Result;
    //drawMatches(image1, new_rand_keypoint1, image2, new_rand_keypoint2, new_matches, Ransac_Result);
}

Mat ImageProcess::preprocessImg(Mat srcImg)
{
    if(srcImg.channels()==1)
    {
        return srcImg;
    }
    else
    {
        Mat srcImg_gray;
        cvtColor(srcImg,srcImg_gray,CV_RGB2GRAY);
        return srcImg_gray;
    }
}
