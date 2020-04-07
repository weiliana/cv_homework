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

VideoProcess::VideoProcess()
{

}

void VideoProcess::videoCheckSynchronized(QString imagePath, QString videoPath)
{
    // 测试ORB
    Mat target = imread(Utils::qstr2str(imagePath));
    //Mat frame = imread("E:/picture/ORBtest_scene.png");
    VideoCapture capture(Utils::qstr2str(videoPath));
    String winName = "ORB output";
    namedWindow(winName, WINDOW_AUTOSIZE);
    // 帧率
    double vfps = capture.get(CAP_PROP_FPS);   //读取视频的帧率
    //qDebug()<<"video fps: "<<vfps;
    char string[10];
    String fpsStr("");
    double fps;
    double t = 0;
    // 特征检测
    vector<KeyPoint> keypoints_target;
    Mat descriptors_target;
    Ptr<ORB> detector = ORB::create();
    detector->detectAndCompute(target, Mat(), keypoints_target, descriptors_target);
    while(1)
    {
        t = (double)getTickCount();
        Mat frame;  // 存储每一帧图像
        capture >> frame;
        if (frame.empty())
            break;
        // 特征点检测
        vector<KeyPoint> keypoints_scene;
        Mat descriptors_scene;
        detector->detectAndCompute(frame, Mat(), keypoints_scene, descriptors_scene);
        // 特征点匹配
        vector<DMatch> matches;
        Ptr<DescriptorMatcher> matcher = makePtr<FlannBasedMatcher>(makePtr<flann::LshIndexParams>(12,20,2));
        matcher->match(descriptors_target, descriptors_scene, matches);
        // 发现匹配
        vector<DMatch> goodMatches;
        //qDebug()<<"total match points: " << matches.size();
        float maxdist = 0;
        for (unsigned int i = 0; i < matches.size(); i++) {
            maxdist = max(maxdist, matches[i].distance);
        }
        for (unsigned int i = 0; i < matches.size(); i++) {
            if(matches[i].distance < maxdist*RATIO)
                goodMatches.push_back(matches[i]);
        }
        Mat dst;
        drawMatches(target, keypoints_target, frame, keypoints_scene, goodMatches, dst);
        putText(dst, fpsStr, Point(5,20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,255,255));
        imshow(winName, dst);
        waitKey(10);  // 延迟
        if(getWindowProperty(winName,0) == -1)
        {
            qDebug()<<"video window closed.";
            break;
        }
        t = ((double)getTickCount() - t) / getTickFrequency();
        fps = 1.0 / t;
        sprintf(string, "%.2f", fps);
        fpsStr = String("FPS:") + string;

    }
    capture.release();
}

void VideoProcess::RANSAC(QImage image)
{

}
