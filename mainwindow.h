#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<opencv2/opencv.hpp>
#include<QImage>
using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage image1;
    QImage image2;
    QImage result;
    int featureSelectionIndex;  //指示当前特征提取用的是哪个方法
    int featureMatchIndex;      //指示当前特征提取用的是哪个方法
    int videoProcIndex;
    int Hessian;
    bool is_RANSAC_checked;     //指示要不要去除野点
    QString videoPath;          //视频路径

    void setMsg(QString msg);   //向文本框输出信息


private:
    Ui::MainWindow *ui;

private slots:
    void on_fImgOpen_btn_clicked();
    void on_fImgOpen2_btn_clicked();
    void on_featureSelection_cmb_currentIndexChanged(int index);//修改特征提取方法的索引
    void on_featureMatch_cmb_currentIndexChanged(int index);//调用什么方法进行特征匹配
    void on_HessianSettor_editingFinished();//用来设置一些需要的参数，在实现时自行更加或者添加..这里的hessian只是随便一个例子
    void on_clearMsg_btn_clicked();//清空输出框消息
    void on_ransacCheckBox_stateChanged();//是否要去除野点
    void on_FERNS_btn_clicked();//调用基于分类器的随机阙特征检测方法验证 FERNS的方法
    void on_imageMosaic_btn_clicked();//调用图像拼接方法的槽函数
    void on_geometricCorrection_btn_clicked();//调用实现几何校正的方法的方法
    void on_targetDetect_btn_clicked();//调用目标检测方法的方法
    void on_point2point_btn_clicked();//调用点对映射方法的方法

    void on_videoProcess_btn_clicked();//调用视频实时检测方法的方法
    void on_videoDetectFunc_cmb_currentIndexChanged(int index);//用什么方法处理视频
    void on_videoPath_returnPressed();
    void on_videoOpen_btn_clicked();    //输入视频
    void on_testFeatureSelection_btn_clicked();
    void on_testRANSAC_btn_clicked();
    void on_testFeatureMatch_btn_clicked();
};
#endif // MAINWINDOW_H
