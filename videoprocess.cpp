#include "videoprocess.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/xfeatures2d.hpp>
#include <cmath>
#include <QDebug>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

#define RATIO 0.4
static bool IsVideoOn = false;

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

void VideoProcess::videoCheckSynchronized(QString imagePath, QString videoPath, bool useCamera)
{
    // 测试ORB
    Mat target;
    target = imread(Utils::qstr2str(imagePath));
    //Mat frame = imread("E:/picture/ORBtest_scene.png");
    VideoCapture capture;
    if(useCamera)
        capture = VideoCapture(0);  // 打开默认的摄像
    else
        capture = VideoCapture(Utils::qstr2str(videoPath));
    String winName = "ORB output";
    namedWindow(winName, WINDOW_AUTOSIZE);
    // 帧率
    //double vfps = capture.get(CAP_PROP_FPS);   //读取视频的帧率
    //qDebug()<<"video fps: "<<vfps;
    char string[10];
    String fpsStr("");
    double fps;
    double t = 0;
    // 特征检测
    while(1)
    {
        //qDebug()<<"running...";
        qDebug()<<"video is on: "<<IsVideoOn;
        t = (double)getTickCount();
        if(IsVideoOn)
        {
        //capture>>target;
        Mat frame;  // 存储每一帧图像
        capture >> frame;
        if (frame.empty())
            break;
        // 特征点检测
        vector<KeyPoint> keypoints_target;
        vector<KeyPoint> keypoints_scene;
        Mat descriptors_target;
        Mat descriptors_scene;
        Ptr<ORB> detector = ORB::create();
        detector->detectAndCompute(target, Mat(), keypoints_target, descriptors_target);
        detector->detectAndCompute(frame, Mat(), keypoints_scene, descriptors_scene);
        // 特征点匹配
        vector<DMatch> matches; // 原始匹配点
        // FLANN匹配
        Ptr<DescriptorMatcher> matcher = makePtr<FlannBasedMatcher>(makePtr<flann::LshIndexParams>(12,20,2));
        // Brute-force匹配
        //BFMatcher matcher;
        matcher->match(descriptors_target, descriptors_scene, matches);
        //qDebug()<<"total match points: " << matches.size();
        // 筛选出较好的匹配点
        // goodMatch
        goodMatch(matches);
        // RANSAC
        //mRANSAC(keypoints_target,keypoints_scene,matches);
        qDebug()<<"good match points: " << matches.size();
        Mat dst;
        drawMatches(target, keypoints_target, frame, keypoints_scene, matches, dst);

        t = ((double)getTickCount() - t) / getTickFrequency();
        fps = 1.0 / t;
        sprintf(string, "%.2f", fps);
        fpsStr = String("FPS:") + string;
        putText(dst, fpsStr, Point(5,20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255));
        imshow(winName, dst);
        waitKey(1);  // 延迟
        }
        else waitKey(10);
        t++;
        if(getWindowProperty(winName,0) == -1)
        {
            IsVideoOn = false;
            capture.release();
            qDebug()<<"video window closed.";
            break;
        }
    }

}

void VideoProcess::mRANSAC(vector<KeyPoint> &target, vector<KeyPoint> &scene, vector<DMatch> &matches)
{
    // RANSAC去除误匹配
    // 坐标转换为float
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
    Mat fundamental = findFundamentalMat(points1, points2, ransacStatus, FM_RANSAC);
    //Mat homo = findHomography(scene, target, FM_RANSAC, 3.0, ransacStatus, 100);
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
