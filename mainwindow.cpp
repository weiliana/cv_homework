#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprocess.h"
#include "videoprocess.h"
#include "methodvalidate.h"
#include "utils.h"
#include <QDebug>
#include <QImageReader>
#include <QMessageBox>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("CV-Homework");
    UT->ui = ui;
    featureSelectionIndex=ui->featureSelection_cmb->currentIndex();
    featureMatchIndex=ui->featureMatch_cmb->currentIndex();
    videoProcIndex=ui->videoDetectFunc_cmb->currentIndex();
    ui->videoSpeed_slider->setMinimum(1);
    ui->videoSpeed_slider->setMaximum(500);
    ui->videoSpeed_slider->setValue(1);
    Hessian=100;
    is_RANSAC_checked=ui->ransacCheckBox->isChecked();
    videoUseCUDA = false;

    trackerType = ui->trackerType_cmb->currentIndex();
    UT->setMsg("Program running successfully!");
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_fImgOpen_btn_clicked()//打开第一个图片
{
    QString OpenFile,OpenFilePath;
    OpenFile = QFileDialog::getOpenFileName(this, "Please choose an image file", "",
                                                 "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    if(OpenFile!="")
    {
        QImageReader reader(OpenFile);
        reader.setDecideFormatFromContent(true);
        if(reader.canRead())
        {
            reader.read(&image1);
            Mat matImage=Utils::QImage2cvMat(image1);
            imshow("image1",matImage);
            //show file path
            QFileInfo OpenFileInfo;
            OpenFileInfo=QFileInfo(OpenFile);
            OpenFilePath=OpenFileInfo.filePath();
            ui->ImagePath1->setText(OpenFilePath);
            UT->setMsg("first picture has opened.");
        }
    }
}

void MainWindow::on_fImgOpen2_btn_clicked()
{
    QString OpenFile,OpenFilePath;
    OpenFile = QFileDialog::getOpenFileName(this,  "Please choose an image file", "",
                                                 "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    if(OpenFile!="")
    {
        QImageReader reader(OpenFile);
        reader.setDecideFormatFromContent(true);
        if(reader.canRead())
        {
            reader.read(&image2);
            Mat matImage=Utils::QImage2cvMat(image2);
            imshow("image2",matImage);
            //show file path
            QFileInfo OpenFileInfo;
            OpenFileInfo=QFileInfo(OpenFile);
            OpenFilePath=OpenFileInfo.filePath();
            ui->ImagePath2->setText(OpenFilePath);
            UT->setMsg("second picture has opened.");
        }
    }
}

void MainWindow::on_featureSelection_cmb_currentIndexChanged(int index)
{
    qDebug()<<"original featureSelection index is:"<<featureSelectionIndex;
    featureSelectionIndex=index;
    qDebug()<<"current featureSelection index is:"<<featureSelectionIndex;
}

void MainWindow::on_HessianSettor_editingFinished()
{
    qDebug()<<"original hessian is: "<<Hessian;
    Hessian=ui->HessianSettor->text().toInt();
    qDebug()<<"new hessian is: "<<Hessian;
}

void MainWindow::on_clearMsg_btn_clicked()
{
    ui->log->clear();
}

void MainWindow::on_ransacCheckBox_stateChanged()
{
    is_RANSAC_checked=!is_RANSAC_checked;
    qDebug()<<"the state of RANSAC_checkbox is:"<<is_RANSAC_checked;
}

void MainWindow::on_FERNS_btn_clicked()
{
    UT->setMsg("verifying FERNS...");
    switch(featureSelectionIndex)
    {
    case 0:
        //ImageProcess::sift();
        break;
    case 1:
        //ImageProcess::surf();
        break;
    default:
        break;
    }

    MethodValidate::FERNS_Verify();
    UT->setMsg("verifying FERNS is finished!");
}

void MainWindow::on_imageMosaic_btn_clicked()
{
    if(image1.isNull())
    {
        UT->setMsg("Process failed because Image1 is null!Please choose one image.");
        return;
    }
    else if(image2.isNull())
    {
        UT->setMsg("Process failed because Image2 is null!Please choose one image.");
        return;
    }
    else
    {
        UT->setMsg("Image mosaic is running...");
        double beginTime=clock();
        ImageProcess::imageMosaic(featureSelectionIndex,featureMatchIndex,is_RANSAC_checked,image1,image2,Hessian);
        double endTime=clock();
        double ProcessTime=(endTime - beginTime)/CLOCKS_PER_SEC*1000;
        UT->setMsg("Image mosaic is finished!");
        UT->setMsg("Process Time:"+QString::number(ProcessTime,10,2)+"ms");
        QMessageBox messageBox(QMessageBox::NoIcon,QString::fromLocal8Bit("拼接完成"),QString::fromLocal8Bit("结果是否正确"),QMessageBox::Yes|QMessageBox::No);
        int msgBox=messageBox.exec();
        if(msgBox==QMessageBox::No)
        {
            cv::destroyWindow("Image Mosaic");
            beginTime=clock();
            ImageProcess::imageMosaic(featureSelectionIndex,featureMatchIndex,is_RANSAC_checked,image2,image1,Hessian);
            endTime=clock();
            ProcessTime=(endTime - beginTime)/CLOCKS_PER_SEC*1000;
            UT->setMsg("Image mosaic is finished!");
            UT->setMsg("Process Time:"+QString::number(ProcessTime,10,2)+"ms");
            if(!is_RANSAC_checked)
                QMessageBox::information(NULL,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("如果结果仍不正确请勾选RANSAC后再次尝试"));
        }
    }
}

void MainWindow::on_geometricCorrection_btn_clicked()
{
    if(image1.isNull())
    {
        UT->setMsg("Process failed because Image1 is null!Please choose one image.");
        return;
    }
    else if(image2.isNull())
    {
        UT->setMsg("Process failed because Image2 is null!Please choose one image.");
        return;
    }
    else
    {
        UT->setMsg("GeometricCorrection is running...");
        double beginTime=clock();
        ImageProcess::geometricCorrection(featureSelectionIndex,featureMatchIndex,is_RANSAC_checked,image1,image2,Hessian);
        double endTime=clock();
        double ProcessTime=(endTime - beginTime)/CLOCKS_PER_SEC*1000;
        UT->setMsg("GeometricCorrection is finished!");
        UT->setMsg("Process Time:"+QString::number(ProcessTime,10,2)+"ms");
        if(!is_RANSAC_checked)
            QMessageBox::information(NULL,QString::fromLocal8Bit("几何矫正完成"),QString::fromLocal8Bit("如果结果不正确请勾选RANSAC后再次尝试"));
    }
}

void MainWindow::on_targetDetect_btn_clicked()
{
    if(image1.isNull())
    {
        UT->setMsg("Process failed because Image1 is null!Please choose one image.");
        return;
    }
    else if(image2.isNull())
    {
        UT->setMsg("Process failed because Image2 is null!Please choose one image.");
        return;
    }
    else
    {
        UT->setMsg("Target detection is running...");
        double beginTime=clock();
        ImageProcess::targetDetect(featureSelectionIndex,featureMatchIndex,is_RANSAC_checked,image1,image2,Hessian);
        double endTime=clock();
        double ProcessTime=(endTime - beginTime)/CLOCKS_PER_SEC*1000;
        UT->setMsg("Target detection is finished!");
        UT->setMsg("Process Time:"+QString::number(ProcessTime,10,2)+"ms");
    }
}

void MainWindow::on_point2point_btn_clicked()
{
    if(image1.isNull())
    {
        UT->setMsg("Process failed because Image 1 is null!Please choose one image.");
        return;
    }
    else if(image2.isNull())
    {
        UT->setMsg("Process failed because Image 2 is null!Please choose one image.");
        return;
    }
    else
    {
        UT->setMsg("Point2point is running...");
        double beginTime=clock();
        ImageProcess::point2point(featureSelectionIndex,featureMatchIndex,is_RANSAC_checked,image1,image2,Hessian);
        double endTime=clock();
        double ProcessTime=(endTime - beginTime)/CLOCKS_PER_SEC*1000;
        UT->setMsg("Point2point is finished!");
        UT->setMsg("Process Time:"+QString::number(ProcessTime,10,2)+"ms");
    }
}


void MainWindow::on_videoProcess_btn_clicked()
{
    UT->setMsg("video function is running...");
    VideoProcess::setVideoStatus(true);
    if(videoPath.isEmpty() || videoMacthTargetPath.isEmpty())
        UT->setMsg("video or image is null!");
    else
    {
        if(videoUseCUDA)
            VideoProcess::videoORBtoCUDAProc(videoMacthTargetPath, videoPath);
        else
            VideoProcess::videoCheckSynchronized(videoMacthTargetPath, videoPath);
        UT->setMsg("video function is finished!");
    }
}

void MainWindow::on_videoStepProcess_btn_clicked()
{
    // TODO
    VideoProcess::setVideoStatus(true);
    VideoProcess::setVideoStepProc(true);
}

void MainWindow::on_videoContinue_btn_clicked()
{
    bool isVideoOn = VideoProcess::getVideoStatus();
    VideoProcess::setVideoStatus(!isVideoOn);
}

void MainWindow::on_useCamera_btn_clicked()
{
    UT->setMsg("video function is running...");
    VideoProcess::setVideoStatus(true);
    if(videoMacthTargetPath.isEmpty())
        UT->setMsg("video match target image is null!");
    else
    {
        if(videoUseCUDA)
            VideoProcess::videoORBtoCUDAProc(videoMacthTargetPath, videoPath, true);
        else
            VideoProcess::videoCheckSynchronized(videoMacthTargetPath, videoPath, true);
        UT->setMsg("video function is finished!");
    }
}

void MainWindow::on_useVideoCUDA_stateChanged(int arg1)
{
    videoUseCUDA = arg1 == 0? false: true;
}

void MainWindow::on_videoSpeed_slider_valueChanged(int value)
{
    VideoProcess::setVideoSpeed(value);
}

void MainWindow::on_videoDetectFunc_cmb_currentIndexChanged(int index)
{
    qDebug()<<"original video detect function index is:"<<videoProcIndex;
    videoProcIndex=index;
    qDebug()<<"new video detect function index is: "<<videoProcIndex;
}

void MainWindow::on_featureMatch_cmb_currentIndexChanged(int index)
{
    qDebug()<<"original featureMatch index is:"<<featureMatchIndex;
    featureMatchIndex=index;
    qDebug()<<"current featureMatch index is:"<<featureMatchIndex;
}

void MainWindow::on_videoPath_returnPressed()
{
    videoPath=ui->videoPath->text();
    qDebug()<<"video path is:"<<videoPath;
}

void MainWindow::on_videoTargetImg_btn_clicked()
{
    QString openFile,openFilePath;
    openFile = QFileDialog::getOpenFileName(this,  "Please choose an image file", "",
                                                 "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
    if(openFile!="")
    {
        //show file path
        QFileInfo openFileInfo;
        openFileInfo=QFileInfo(openFile);
        openFilePath=openFileInfo.filePath();
        ui->videoTargetImgPath->setText(openFilePath);
        Mat img = imread(Utils::qstr2str(openFilePath));
        videoMacthTargetPath = openFilePath;
        imshow("target image", img);
        UT->setMsg("video file has opened.");

    }
}

void MainWindow::on_videoOpen_btn_clicked()
{
    QString openFile,openFilePath;
    openFile = QFileDialog::getOpenFileName(this,  "Please choose an video file", "",
                                                 "Video Files(*.mp4 *.flv *.avi);;All(*.*)");
    if(openFile!="")
    {
        // TODO::处理视频文件

        //show file path
        QFileInfo OpenFileInfo;
        OpenFileInfo=QFileInfo(openFile);
        openFilePath=OpenFileInfo.filePath();
        ui->videoPath->setText(openFilePath);
        UT->setMsg("video file has opened.");
        videoPath = openFilePath;

        VideoCapture capture(Utils::qstr2str(openFilePath));
        String winName = "读取视频";
        namedWindow(winName, WINDOW_NORMAL);
        while(1)
        {
            Mat frame;  // 存储每一帧图像
            capture >> frame;
            imshow(winName, frame);
            waitKey(30);    // 延迟30ms
            // 判断是否点击窗口关闭按键		窗口关闭返回 -1
            if(getWindowProperty(winName,0) == -1)
            {
                qDebug()<<"video window closed.";
                break;
            }
        }
        capture.release();
    }

}

void MainWindow::on_videoObjLine_checkBox_stateChanged(int arg1)
{
    //qDebug()<<arg1;
    VideoProcess::setVideoLineBox(arg1 == 0? false:true);
}


/**
 * 目标跟踪
 */
// 选择单个视频文件
void MainWindow::on_videoTrack_btn_clicked()
{
    QString openFile,openFilePath;
    openFile = QFileDialog::getOpenFileName(this,  "Please choose an video file", "",
                                                 "Video Files(*.mp4 *.flv *.avi);;All(*.*)");
    if(openFile!="")
    {
        //show file path
        QFileInfo openFileInfo;
        openFileInfo=QFileInfo(openFile);
        openFilePath=openFileInfo.filePath();
        ui->videoTrackPath->setText(openFilePath);
        UT->setMsg("video for track file has opened.");
        videoForTrackPath = openFilePath;
    }
}
// 跟踪器选择
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    trackerType = index;
    qDebug()<<"trackerType:"<<index;
}
// 视频目标跟踪处理
void MainWindow::on_trackerProcess_btn_clicked()
{

}
// 视频目标跟踪暂停/继续
void MainWindow::on_trackingContinue_btn_clicked()
{

}
