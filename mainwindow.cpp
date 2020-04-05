#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprocess.h"
#include "videoprocess.h"
#include "methodvalidate.h"
#include <QDebug>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("CV-Homework");
    featureSelectionIndex=ui->featureSelection_cmb->currentIndex();
    featureMatchIndex=ui->featureMatch_cmb->currentIndex();
    videoProcIndex=ui->videoDetectFunc_cmb->currentIndex();
    Hessian=100;
    is_RANSAC_checked=ui->ransacCheckBox->isChecked();
    this->setMsg("Program running successfully!");
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
        if(image1.load(OpenFile))
        {
            Mat matImage=ImageProcess::QImage2cvMat(image1);
            imshow("image1",matImage);
            //show file path
            QFileInfo OpenFileInfo;
            OpenFileInfo=QFileInfo(OpenFile);
            OpenFilePath=OpenFileInfo.filePath();
            ui->ImagePath1->setText(OpenFilePath);
            setMsg("first picture has opened.");
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
        if(image2.load(OpenFile))
        {
            Mat matImage=ImageProcess::QImage2cvMat(image2);
            imshow("image2",matImage);
            //show file path
            QFileInfo OpenFileInfo;
            OpenFileInfo=QFileInfo(OpenFile);
            OpenFilePath=OpenFileInfo.filePath();
            ui->ImagePath2->setText(OpenFilePath);
            setMsg("second picture has opened.");
        }
    }
}

void MainWindow::setMsg(QString msg)
{
    QString Msg=ui->log->toPlainText();
    Msg.append(msg);
    Msg.append("\n");
    ui->log->setPlainText(Msg);
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
    setMsg("verifying FERNS...");
    switch(featureSelectionIndex)
    {
    case 0:
        ImageProcess::sift();
        break;
    case 1:
        ImageProcess::surf();
        break;
    }

    MethodValidate::FERNS_Verify();
    setMsg("verifying FERNS is finished!");
}

void MainWindow::on_imageMosaic_btn_clicked()
{
    setMsg("image mosaic is running...");
    ImageProcess::imageMosaic();
    setMsg("image mosaic is finished!");
}

void MainWindow::on_geometricCorrection_btn_clicked()
{
    setMsg("geometricCorrection is running...");
    ImageProcess::geometricCorrection();
    setMsg("geometricCorrection is finished!");
}

void MainWindow::on_targetDetect_btn_clicked()
{
    setMsg("target detect is running...");
    ImageProcess::targetDetect();
    setMsg("target detect is finished!");
}

void MainWindow::on_point2point_btn_clicked()
{
    setMsg("point2point is running...");
    ImageProcess::point2point();
    setMsg("point2point is finished!");
}

void MainWindow::on_videoProcess_btn_clicked()
{
    setMsg("video function is running...");
    VideoProcess::videoCheckSynchronized();
    setMsg("video function is finished!");
}

void MainWindow::on_videoDetectFunc_cmb_currentIndexChanged(int index)
{
    qDebug()<<"original video proc index is:"<<videoProcIndex;
    videoProcIndex=index;
    qDebug()<<"new video proc index is: "<<videoProcIndex;
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

void MainWindow::on_videoOpen_btn_clicked()
{
    QString OpenFile,OpenFilePath;
    OpenFile = QFileDialog::getOpenFileName(this,  "Please choose an video file", "",
                                                 "Video Files(*.mp4 *.flv *.avi);;All(*.*)");
    if(OpenFile!="")
    {
        // TODO::处理视频文件

        //show file path
        QFileInfo OpenFileInfo;
        OpenFileInfo=QFileInfo(OpenFile);
        OpenFilePath=OpenFileInfo.filePath();
        ui->videoPath->setText(OpenFilePath);

        setMsg("video file has opened.");
    }

}
