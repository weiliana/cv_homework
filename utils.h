#ifndef UTILS_H
#define UTILS_H
#include "ui_mainwindow.h"
#include <QString>
#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
using namespace std;
using namespace cv;

class Utils
{
private:
    static Utils _utils;
    Utils() = default;
public:
    Ui::MainWindow *ui = nullptr;

    ~Utils() = default;
    static Utils* GetInstance();
    //向文本框输出信息
    void setMsg(QString msg);

    static QString str2qstr(const string str);

    static string qstr2str(const QString qstr);

    //将QImage类型图片转换为cv可以处理的矩阵类型
    static Mat QImage2cvMat(QImage image);

    //将MAT类型图片转换为QImage
    static QImage cvMat2QImage(const Mat& mat);

};

// 获取单例对象
#define UT (Utils::GetInstance())

#endif // UTILS_H
