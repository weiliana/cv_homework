#include "imageprocess.h"
#include <QDebug>


ImageProcess::ImageProcess()
{

}

void ImageProcess::point2point(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian)
{
    Mat cvImg1,cvImg2,resultImg;
    cvImg1=Utils::QImage2cvMat(qimg1);
    cvImg2=Utils::QImage2cvMat(qimg2);
    vector<KeyPoint> keyPoints1,keyPoints2;
    vector<DMatch> matches;
    commonProcess(featureSelectionIndex,featureMatchIndex,cvImg1,cvImg2,resultImg,Hessian,keyPoints1,keyPoints2,matches);
    if(!RANSAC_on)
    {
        drawMatches(cvImg1,keyPoints1,cvImg2,keyPoints2,matches,resultImg);
    }
    else
    {
        vector<KeyPoint> rand_keyPoints1,rand_keyPoints2;
        vector<DMatch> goodMatches,rand_matches;
        vector<Point2f> points_1,points_2;
        RANSAC(keyPoints1,keyPoints2,matches,goodMatches,rand_keyPoints1,rand_keyPoints2,rand_matches,points_1,points_2);
        drawMatches(cvImg1,rand_keyPoints1,cvImg2,rand_keyPoints2,rand_matches,resultImg);
    }
    imshow("Point to point",resultImg);
}

void ImageProcess::geometricCorrection(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian)
{
    Mat cvImg1,cvImg2,resultImg;
    cvImg1=Utils::QImage2cvMat(qimg1);
    cvImg2=Utils::QImage2cvMat(qimg2);
    vector<KeyPoint> keyPoints1,keyPoints2;
    vector<DMatch> matches;
    commonProcess(featureSelectionIndex,featureMatchIndex,cvImg1,cvImg2,resultImg,Hessian,keyPoints1,keyPoints2,matches);
    if(!RANSAC_on)
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
         vector <KeyPoint> good_keyPoints1, good_keyPoints2;
         for (size_t i = 0; i < goodMatches.size(); i++)
         {
             good_keyPoints1.push_back(keyPoints1[goodMatches[i].queryIdx]);
             good_keyPoints2.push_back(keyPoints2[goodMatches[i].trainIdx]);
          }
          vector<Point2f> points_1,points_2;
          for (size_t i = 0; i < goodMatches.size(); i++)
          {
              points_1.push_back(good_keyPoints1[i].pt);
              points_2.push_back(good_keyPoints2[i].pt);
          }
          Mat Homo = findHomography(points_1, points_2, 0);
          warpPerspective(cvImg2, resultImg, Homo, Size(cvImg1.size().width, cvImg1.size().height));
    }
    else
    {
        vector<KeyPoint> rand_keyPoints1,rand_keyPoints2;
        vector<DMatch> goodMatches,rand_matches;
        vector<Point2f> points_1,points_2;
        RANSAC(keyPoints1,keyPoints2,matches,goodMatches,rand_keyPoints1,rand_keyPoints2,rand_matches,points_1,points_2);
        Mat Homo = findHomography(points_2, points_1, FM_RANSAC);
        warpPerspective(cvImg2, resultImg, Homo, Size(cvImg1.size().width, cvImg1.size().height));
    }
    imshow("Geometric Correction",resultImg);
}

void ImageProcess::imageMosaic(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian)
{
    Mat cvImg1,cvImg2,resultImg;
       cvImg1=Utils::QImage2cvMat(qimg1);
       cvImg2=Utils::QImage2cvMat(qimg2);
       vector<KeyPoint> keyPoints1,keyPoints2;
       vector<DMatch> matches;
       commonProcess(featureSelectionIndex,featureMatchIndex,cvImg1,cvImg2,resultImg,Hessian,keyPoints1,keyPoints2,matches);
       if(!RANSAC_on)
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
            vector <KeyPoint> good_keyPoints1, good_keyPoints2;
            for (size_t i = 0; i < goodMatches.size(); i++)
            {
                good_keyPoints1.push_back(keyPoints1[goodMatches[i].queryIdx]);
                good_keyPoints2.push_back(keyPoints2[goodMatches[i].trainIdx]);
             }
             vector<Point2f> points_1,points_2;
             for (size_t i = 0; i < goodMatches.size(); i++)
             {
                 points_1.push_back(good_keyPoints1[i].pt);
                 points_2.push_back(good_keyPoints2[i].pt);
             }
             Mat Homo = findHomography(points_1, points_2, 0);
             Mat MosaicMat=(Mat_<double>(3, 3) << 1.0, 0, cvImg1.cols, 0, 1.0, 0, 0, 0, 1.0);
             Mat MosaicHomo=MosaicMat*Homo;
             Point2f originalLinkPoint,targetLinkPoint,basedImgPoint;
             originalLinkPoint = keyPoints1[goodMatches[0].queryIdx].pt;
             targetLinkPoint = getTransformPoint(originalLinkPoint, MosaicHomo);
             basedImgPoint = keyPoints2[goodMatches[0].trainIdx].pt;
             warpPerspective(cvImg1, resultImg, MosaicHomo, Size(cvImg2.cols + cvImg1.cols + 10, cvImg2.rows));
             Mat ROIMat = cvImg2(Rect(Point(basedImgPoint.x, 0), Point(cvImg2.cols, cvImg2.rows)));
             ROIMat.copyTo(Mat(resultImg, Rect(targetLinkPoint.x, 0, cvImg2.cols - basedImgPoint.x + 1, cvImg2.rows)));
       }
       else
       {
           vector<KeyPoint> rand_keyPoints1,rand_keyPoints2;
           vector<DMatch> goodMatches,rand_matches;
           vector<Point2f> points_1,points_2;
           RANSAC(keyPoints1,keyPoints2,matches,goodMatches,rand_keyPoints1,rand_keyPoints2,rand_matches,points_1,points_2);
           Mat Homo = findHomography(points_2, points_1, FM_RANSAC);
           Mat MosaicMat=(Mat_<double>(3, 3) << 1.0, 0, cvImg1.cols, 0, 1.0, 0, 0, 0, 1.0);
           Mat MosaicHomo=MosaicMat*Homo;
           Point2f originalLinkPoint,targetLinkPoint,basedImgPoint;
           originalLinkPoint = keyPoints1[goodMatches[0].queryIdx].pt;
           targetLinkPoint = getTransformPoint(originalLinkPoint, MosaicHomo);
           basedImgPoint = keyPoints2[goodMatches[0].trainIdx].pt;
           warpPerspective(cvImg1, resultImg, MosaicHomo, Size(cvImg2.cols + cvImg1.cols + 10, cvImg2.rows));
           Mat ROIMat = cvImg2(Rect(Point(basedImgPoint.x, 0), Point(cvImg2.cols, cvImg2.rows)));
           ROIMat.copyTo(Mat(resultImg, Rect(targetLinkPoint.x, 0, cvImg2.cols - basedImgPoint.x + 1, cvImg2.rows)));
       }
       imshow("Image Mosaic",resultImg);
}

void ImageProcess::targetDetect(int featureSelectionIndex,int featureMatchIndex,bool RANSAC_on,QImage qimg1,QImage qimg2,int Hessian)
{
    Mat cvImg1,cvImg2,resultImg;
    cvImg1=Utils::QImage2cvMat(qimg1);
    cvImg2=Utils::QImage2cvMat(qimg2);
    if (cvImg1.cols > cvImg2.cols&&cvImg1.rows > cvImg2.rows)
    {
        Mat temp=cvImg1;
        cvImg1=cvImg2;
        cvImg2=temp;
    }
    vector<KeyPoint> keyPoints1,keyPoints2;
    vector<DMatch> matches;
    commonProcess(featureSelectionIndex,featureMatchIndex,cvImg1,cvImg2,resultImg,Hessian,keyPoints1,keyPoints2,matches);
    if(!RANSAC_on)
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
       vector <KeyPoint> good_keyPoints1, good_keyPoints2;
       for (size_t i = 0; i < goodMatches.size(); i++)
       {
           good_keyPoints1.push_back(keyPoints1[goodMatches[i].queryIdx]);
           good_keyPoints2.push_back(keyPoints2[goodMatches[i].trainIdx]);
        }
        vector<Point2f> points_1,points_2;
        for (size_t i = 0; i < goodMatches.size(); i++)
        {
            points_1.push_back(good_keyPoints1[i].pt);
            points_2.push_back(good_keyPoints2[i].pt);
        }
        Mat Homo = findHomography(points_1, points_2, 0);
        vector<Point2f> objCorners(4);
        objCorners[0] = cvPoint(0, 0);
        objCorners[1] = cvPoint(cvImg1.cols, 0);
        objCorners[2] = cvPoint(cvImg1.cols, cvImg1.rows);
        objCorners[3] = cvPoint(0, cvImg1.rows);
        vector<Point2f> sceneCorners(4);
        perspectiveTransform(objCorners, sceneCorners, Homo);
        drawMatches(cvImg1, keyPoints1, cvImg2, keyPoints2, goodMatches, resultImg);

        line(resultImg, sceneCorners[0] + Point2f(cvImg1.cols, 0), sceneCorners[1] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
        line(resultImg, sceneCorners[1] + Point2f(cvImg1.cols, 0), sceneCorners[2] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
        line(resultImg, sceneCorners[2] + Point2f(cvImg1.cols, 0), sceneCorners[3] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
        line(resultImg, sceneCorners[3] + Point2f(cvImg1.cols, 0), sceneCorners[0] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
    }
    else
    {
        vector<KeyPoint> rand_keyPoints1,rand_keyPoints2;
        vector<DMatch> goodMatches,rand_matches;
        vector<Point2f> points_1,points_2;
        RANSAC(keyPoints1,keyPoints2,matches,goodMatches,rand_keyPoints1,rand_keyPoints2,rand_matches,points_1,points_2);
        Mat Homo = findHomography(points_1, points_2, FM_RANSAC);
        vector<Point2f> objCorners(4);
        objCorners[0] = cvPoint(0, 0);
        objCorners[1] = cvPoint(cvImg1.cols, 0);
        objCorners[2] = cvPoint(cvImg1.cols, cvImg1.rows);
        objCorners[3] = cvPoint(0, cvImg1.rows);
        vector<Point2f> sceneCorners(4);
        perspectiveTransform(objCorners, sceneCorners, Homo);
        drawMatches(cvImg1, rand_keyPoints1, cvImg2, rand_keyPoints2, rand_matches, resultImg);

        line(resultImg, sceneCorners[0] + Point2f(cvImg1.cols, 0), sceneCorners[1] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
        line(resultImg, sceneCorners[1] + Point2f(cvImg1.cols, 0), sceneCorners[2] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
        line(resultImg, sceneCorners[2] + Point2f(cvImg1.cols, 0), sceneCorners[3] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
        line(resultImg, sceneCorners[3] + Point2f(cvImg1.cols, 0), sceneCorners[0] + Point2f(cvImg1.cols, 0), Scalar(0, 255, 0), 4);
    }
    imshow("Target Detection", resultImg);
}

void ImageProcess::sift(Mat cvImg1,Mat cvImg2,int Hessian,vector<KeyPoint> &keyPoints1,vector<KeyPoint> &keyPoints2,Mat &imageDesc1,Mat &imageDesc2)
{
    Mat cvImg1_gray=preprocessImg(cvImg1);
    Mat cvImg2_gray=preprocessImg(cvImg2);

    Ptr<xfeatures2d::SIFT> siftDetector = xfeatures2d::SIFT::create(Hessian);
    siftDetector->detect(cvImg1_gray,keyPoints1);
    siftDetector->detect(cvImg2_gray,keyPoints2);

    Ptr<xfeatures2d::SIFT> siftDescriptor = xfeatures2d::SIFT::create();
    siftDescriptor->compute(cvImg1_gray, keyPoints1, imageDesc1);
    siftDescriptor->compute(cvImg2_gray, keyPoints2, imageDesc2);
}

void ImageProcess::surf(Mat cvImg1,Mat cvImg2,int Hessian,vector<KeyPoint> &keyPoints1,vector<KeyPoint> &keyPoints2,Mat &imageDesc1,Mat &imageDesc2)
{
    Mat cvImg1_gray=preprocessImg(cvImg1);
    Mat cvImg2_gray=preprocessImg(cvImg2);

    Ptr<xfeatures2d::SURF> surfDetector = xfeatures2d::SURF::create(Hessian);
    surfDetector->detect(cvImg1_gray,keyPoints1);
    surfDetector->detect(cvImg2_gray,keyPoints2);

    Ptr<xfeatures2d::SURF> surfDescriptor = xfeatures2d::SURF::create();
    surfDescriptor->compute(cvImg1_gray, keyPoints1, imageDesc1);
    surfDescriptor->compute(cvImg2_gray, keyPoints2, imageDesc2);
}

void ImageProcess::bruteForce(Mat imageDesc1,Mat imageDesc2,vector<DMatch> &matches)
{
    BFMatcher bfmatcher;
    bfmatcher.match(imageDesc1, imageDesc2, matches, Mat());
}

void ImageProcess::FLANN(Mat imageDesc1,Mat imageDesc2,vector<DMatch> &matches)
{
    FlannBasedMatcher FLANNmatcher;
    FLANNmatcher.match(imageDesc1, imageDesc2, matches, Mat());
}

void ImageProcess::RANSAC(vector<KeyPoint> keyPoints1,vector<KeyPoint> keyPoints2,vector<DMatch> matches,vector<DMatch> &goodMatches,vector<KeyPoint> &new_rand_keypoint1,vector<KeyPoint> &new_rand_keypoint2,vector <DMatch> &new_matches,vector<Point2f> &points_1,vector<Point2f> &points_2)
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
    for (size_t m = 0; m < matches.size(); m++)
    {
         if (matches[m].distance < 2 * min_dist)
         {
            goodMatches.push_back(matches[m]);
        }
    }

    //RANSAC匹配过程
    vector<DMatch> old_matches=goodMatches;
    //坐标转换为float类型
    vector <KeyPoint> rand_keypoint1, rand_keypoint2;
    //size_t是标准C库中定义的，应为unsigned int，在64位系统中为long unsigned int,在C++中为了适应不同的平台，增加可移植性。
    for (size_t i = 0; i < goodMatches.size(); i++)
    {
        rand_keypoint1.push_back(keyPoints1[goodMatches[i].queryIdx]);
        rand_keypoint2.push_back(keyPoints2[goodMatches[i].trainIdx]);
    }
    //坐标变换
    for (size_t i = 0; i < goodMatches.size(); i++)
    {
        points_1.push_back(rand_keypoint1[i].pt);
        points_2.push_back(rand_keypoint2[i].pt);
    }
    vector<uchar> RansacStatus;
    Mat Fundamental = findFundamentalMat(points_1, points_2, RansacStatus, FM_RANSAC);
    //新的关键点new_rand_keypoint和new_matches来存储新的关键点和基础矩阵，通过RansacStatus来删除误匹配点
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
}

void ImageProcess::commonProcess(int featureSelectionIndex,int featureMatchIndex,Mat cvImg1,Mat cvImg2,Mat &resultImg,int Hessian, vector<KeyPoint> &keyPoints1, vector<KeyPoint> &keyPoints2,vector<DMatch> &matches)
{
    Mat imageDesc1,imageDesc2;
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
Point2f ImageProcess::getTransformPoint(const Point2f originalPoint, const Mat &transformMatrix)
{
    Mat originalP,targetP;
    originalP=(Mat_<double>(3,1)<<originalPoint.x,originalPoint.y,1.0);
    targetP=transformMatrix*originalP;
    float x=targetP.at<double>(0,0)/targetP.at<double>(2,0);
    float y=targetP.at<double>(1,0)/targetP.at<double>(2,0);
    return Point2f(x,y);
}

