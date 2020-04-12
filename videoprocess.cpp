#include "videoprocess.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudabgsegm.hpp>
#include <opencv2/core/cuda_stream_accessor.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/cudawarping.hpp>

#include <vector>
#include <cmath>
#include <QDebug>
using namespace cv;
//using namespace cuda;
using namespace std;

#define RATIO 0.4
static bool IsVideoOn = false;      // 视频是否处于播放状态
static bool IsSetpProc = false;     // 是否逐帧处理
static bool HasLineBox = false;      // 物体线框
static unsigned int waitTime = 1;   // 播放时每一帧延迟时间

VideoProcess::VideoProcess()
{

}

bool VideoProcess::getVideoStatus()
{
    return IsVideoOn;
}

void VideoProcess::setVideoStatus(bool isOn)
{
    IsVideoOn = isOn;
}

void VideoProcess::setVideoSpeed(int wTime)
{
    waitTime = wTime;
}

void VideoProcess::setVideoStepProc(bool isStep)
{
    IsSetpProc = isStep;
}

void VideoProcess::setVideoLineBox(bool hasLineBox)
{
    HasLineBox = hasLineBox;
}

void VideoProcess::videoCheckSynchronized(QString imagePath, QString videoPath, bool useCamera)
{
    QString outStr;
    // 设置窗口
    String winName = "ORB output";
    namedWindow(winName, WINDOW_NORMAL);
    // 帧率
    //double vfps = capture.get(CAP_PROP_FPS);   //读取视频的帧率
    char string[10];
    String fpsStr;
    double fps, t = 0;

    // 获取视频源
    VideoCapture capture;
    if(useCamera)
        capture = VideoCapture(0);  // 打开默认的摄像
    else
        capture = VideoCapture(Utils::qstr2str(videoPath));
    // 读取目标图像
    Mat target;
    target = imread(Utils::qstr2str(imagePath));
    // 物体线框
    vector<Point2f> objCorners(4);
    vector<Point2f> sceneCorners(4);
    objCorners[0] = cvPoint(0, 0);
    objCorners[1] = cvPoint(target.cols, 0);
    objCorners[2] = cvPoint(target.cols, target.rows);
    objCorners[3] = cvPoint(0, target.rows);

    vector<KeyPoint> keypoints_target, keypoints_scene;
    Mat descriptors_target, descriptors_scene;
    Ptr<cv::ORB> detector = cv::ORB::create();
    detector->detectAndCompute(target, Mat(), keypoints_target, descriptors_target);
    // Brute-force匹配
    BFMatcher matcher;

    // 特征检测
    while(1)
    {
        t = (double)getTickCount();
        if(IsVideoOn)
        {
            Mat frame;  // 存储每一帧图像
            capture >> frame;
            if (frame.empty())
                break;
            // 特征点检测
            detector->detectAndCompute(frame, Mat(), keypoints_scene, descriptors_scene);
            // 特征点匹配
            vector<DMatch> matches; // 原始匹配点
            // FLANN匹配
            //Ptr<cv::DescriptorMatcher> matcher = makePtr<FlannBasedMatcher>(makePtr<flann::LshIndexParams>(12,20,2));
            matcher.match(descriptors_target, descriptors_scene, matches);
            outStr.sprintf("Total match points: %d" , matches.size());
            UT->setMsg(outStr);
            // 筛选出较好的匹配点
            // goodMatch
            //goodMatch(matches);
            // RANSAC
            Mat homo;
            mRANSAC(keypoints_target,keypoints_scene,matches, homo);
            outStr.sprintf("Good match points: %d" , matches.size());
            UT->setMsg(outStr);
            // 输出结果图像
            Mat dst;

            drawMatches(target, keypoints_target, frame, keypoints_scene, matches, dst);
            if(HasLineBox)
            {
                perspectiveTransform(objCorners, sceneCorners, homo);
                line(dst, sceneCorners[0] + Point2f(target.cols, 0), sceneCorners[1] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
                line(dst, sceneCorners[1] + Point2f(target.cols, 0), sceneCorners[2] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
                line(dst, sceneCorners[2] + Point2f(target.cols, 0), sceneCorners[3] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
                line(dst, sceneCorners[3] + Point2f(target.cols, 0), sceneCorners[0] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
            }
            // 计算fps
            t = ((double)getTickCount() - t) / getTickFrequency();
            t += (double)waitTime / 1000.0;
            fps = 1.0 / t;
            sprintf(string, "%.2f", fps);
            fpsStr = String("FPS:") + string;
            putText(dst, fpsStr, Point(50,50), FONT_HERSHEY_SIMPLEX, 2, Scalar(255,255,255),2);
            imshow(winName, dst);
            waitKey(waitTime);  // 延迟
            if(IsSetpProc){ IsVideoOn = false; IsSetpProc = false; }
        }

        else waitKey(30);
        // 关闭窗口时
        if(getWindowProperty(winName,0) == -1)
        {
            IsVideoOn = false;
            capture.release();
            break;
        }
    }

}

void VideoProcess::videoORBtoCUDAProc(QString imagePath, QString videoPath, bool useCamera)
{
    QString outStr;
    // 设置窗口
    String winName = "ORB output";
    namedWindow(winName, WINDOW_NORMAL);
    // 帧率
    //double vfps = capture.get(CAP_PROP_FPS);   //读取视频的帧率
    char string[10];
    String fpsStr;
    double fps, t = 0;

    // 获取视频源
    VideoCapture capture;
    if(useCamera)
        capture = VideoCapture(0);  // 打开默认的摄像
    else
        capture = VideoCapture(Utils::qstr2str(videoPath));
    // 读取目标图像
    Mat target;
    target = imread(Utils::qstr2str(imagePath));
    // 物体线框
    vector<Point2f> objCorners(4);
    vector<Point2f> sceneCorners(4);
    objCorners[0] = cvPoint(0, 0);
    objCorners[1] = cvPoint(target.cols, 0);
    objCorners[2] = cvPoint(target.cols, target.rows);
    objCorners[3] = cvPoint(0, target.rows);

    vector<KeyPoint> keypoints_target, keypoints_scene;
    // 初始化gpu内存
    cuda::GpuMat d_img1, d_img2, d_srcL, d_srcR;
    d_img1.upload(target);
    cuda::cvtColor(d_img1, d_srcL, COLOR_BGR2GRAY);
    Ptr<cuda::ORB> d_orb = cuda::ORB::create(500,1.2f,8,31,0,2,0,31,20,true);
    cuda::GpuMat d_keypointsL, d_keypointsR;
    // 特征点检测
    cuda::GpuMat d_descL, d_descR, d_descL_32F, d_descR_32F;
    d_orb->detectAndComputeAsync(d_srcL, cuda::GpuMat(), d_keypointsL, d_descL);
    d_orb->convert(d_keypointsL, keypoints_target);
    d_descL.convertTo(d_descL_32F, CV_32F);
    // Brute-force匹配
    Ptr<cuda::DescriptorMatcher> d_matcher = cuda::DescriptorMatcher::createBFMatcher(NORM_L2);
    vector<DMatch> matches; // 原始匹配点

    // 特征检测
    while(1)
    {
        t = (double)getTickCount();
        if(IsVideoOn)
        {
            Mat frame;  // 存储每一帧图像
            capture >> frame;
            if (frame.empty())
                break;
            // 初始化gpu内存
            d_img2.upload(frame);
            cuda::cvtColor(d_img2, d_srcR, COLOR_BGR2GRAY);

            d_orb->detectAndComputeAsync(d_srcR, cuda::GpuMat(), d_keypointsR, d_descR);
            d_orb->convert(d_keypointsR, keypoints_scene);
            d_descR.convertTo(d_descR_32F, CV_32F);

            d_matcher->match(d_descL_32F, d_descR_32F, matches);

            outStr.sprintf("Total match points: %d" , matches.size());
            UT->setMsg(outStr);

            // 筛选出较好的匹配点
            // goodMatch
            //goodMatch(matches);
            // RANSAC
            Mat homo;
            mRANSAC(keypoints_target,keypoints_scene,matches, homo);
            outStr.sprintf("Good match points: %d" , matches.size());
            UT->setMsg(outStr);
            // 输出结果图像
            Mat dst;

            drawMatches(target, keypoints_target, frame, keypoints_scene, matches, dst);
            if(HasLineBox)
            {
                perspectiveTransform(objCorners, sceneCorners, homo);
                line(dst, sceneCorners[0] + Point2f(target.cols, 0), sceneCorners[1] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
                line(dst, sceneCorners[1] + Point2f(target.cols, 0), sceneCorners[2] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
                line(dst, sceneCorners[2] + Point2f(target.cols, 0), sceneCorners[3] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
                line(dst, sceneCorners[3] + Point2f(target.cols, 0), sceneCorners[0] + Point2f(target.cols, 0), Scalar(0, 255, 0), 4);
            }
            // 计算fps
            t = ((double)getTickCount() - t) / getTickFrequency();
            t += (double)waitTime / 1000.0;
            fps = 1.0 / t;
            sprintf(string, "%.2f", fps);
            fpsStr = String("FPS:") + string;
            putText(dst, fpsStr, Point(50,50), FONT_HERSHEY_SIMPLEX, 2, Scalar(255,255,255),2);
            imshow(winName, dst);
            waitKey(waitTime);  // 延迟
            if(IsSetpProc){ IsVideoOn = false; IsSetpProc = false; }
        }

        else waitKey(30);
        // 关闭窗口时
        if(getWindowProperty(winName,0) == -1)
        {
            IsVideoOn = false;
            capture.release();
            break;
        }
    }
}

void VideoProcess::mRANSAC(vector<KeyPoint> &target, vector<KeyPoint> &scene, vector<DMatch> &matches, Mat& homo)
{
    // RANSAC去除误匹配
    // 坐标转换为float
    if(matches.size() < 20)
        return;
    vector<KeyPoint> RAN_KP1, RAN_KP2;
    for(size_t i = 0; i < matches.size(); i++)
    {
        RAN_KP1.push_back(target[matches[i].queryIdx]);
        RAN_KP2.push_back(scene[matches[i].trainIdx]);
    }
    vector<Point2f> points1, points2;
    for(size_t i = 0; i < matches.size(); i++)
    {
        points1.push_back(RAN_KP1[i].pt);
        points2.push_back(RAN_KP2[i].pt);
    }
    vector<uchar> ransacStatus;
    //Mat fundamental = findFundamentalMat(points1, points2, ransacStatus, FM_RANSAC);
    homo = findHomography(points1, points2, FM_RANSAC, 3.0, ransacStatus, 500);
    // 找出RANSAC后的关键点和匹配
    vector<KeyPoint> rskp1, rskp2;
    vector<DMatch> rsmatches;
    int index = 0;
    for(size_t i = 0; i < matches.size(); i++)
    {
        if(ransacStatus[i] != 0)
        {
            rskp1.push_back(RAN_KP1[i]);
            rskp2.push_back(RAN_KP2[i]);
            matches[i].queryIdx = index;
            matches[i].trainIdx = index;
            rsmatches.push_back(matches[i]);
            index++;
        }
    }
    target = rskp1;
    scene = rskp2;
    matches = rsmatches;
}

void VideoProcess::goodMatch(vector<DMatch> &matches)
{
    vector<DMatch> goodMatches;
    // 贪心法过滤匹配点
    float maxdist = 0;  // 计算匹配结果中距离最大值
    for (unsigned int i = 0; i < matches.size(); i++) {
        maxdist = max(maxdist, matches[i].distance);
    }
    for (unsigned int i = 0; i < matches.size(); i++) {
        if(matches[i].distance < maxdist*RATIO)
            goodMatches.push_back(matches[i]);
    }
    matches = goodMatches;
}
