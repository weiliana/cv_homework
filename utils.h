#ifndef UTILS_H
#define UTILS_H
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
public:
    static QString str2qstr(const string str)
    {
        return QString::fromLocal8Bit(str.data());
    }

    static string qstr2str(const QString qstr)
    {
        QByteArray cdata = qstr.toLocal8Bit();
        return string(cdata);
    }

    //将QImage类型图片转换为cv可以处理的矩阵类型
    static Mat QImage2cvMat(QImage image)
    {
        cv::Mat mat;
        switch (image.format())
        {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        }
        return mat;
    }

    //将MAT类型图片转换为QImage
    static QImage cvMat2QImage(const Mat& mat)
    {
        switch (mat.type()) {
        case CV_8UC1:
        {
            QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
            // Set the color table (used to translate colour indexes to qRgb values)
            image.setColorCount(256);
            for(int i = 0; i < 256; i++)
            {
                image.setColor(i, qRgb(i, i, i));
            }
            // Copy input Mat
            uchar *pSrc = mat.data;
            for(int row = 0; row < mat.rows; row ++)
            {
                uchar *pDest = image.scanLine(row);
                memcpy(pDest, pSrc, mat.cols);
                pSrc += mat.step;
            }
            return image;
        }
        // 8-bits unsigned, NO. OF CHANNELS = 3
        case CV_8UC3:
        {
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        case CV_8UC4:
        {
            qDebug() << "CV_8UC4";
            // Copy input Mat
            const uchar *pSrc = (const uchar*)mat.data;
            // Create QImage with same dimensions as input Mat
            QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            return image.copy();
        }
        default:
        {
            qDebug() << "ERROR: Mat could not be converted to QImage.";
            return QImage();
        }
        }
    }
};

#endif // UTILS_H
