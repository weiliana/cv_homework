QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    imageprocess.cpp \
    main.cpp \
    mainwindow.cpp \
    methodvalidate.cpp \
    utils.cpp \
    videoprocess.cpp

HEADERS += \
    imageprocess.h \
    mainwindow.h \
    methodvalidate.h \
    utils.h \
    videoprocess.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CUDA_DIR  = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.2"

INCLUDEPATH += \
    D:/new_opencv4.2.0/opencv/include \
    D:/new_opencv4.2.0/opencv/include/opencv2 \
    $$CUDA_DIR/include

QMAKE_LIBDIR += $$CUDA_DIR/lib/x64

LIBS += \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_aruco420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_bgsegm420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_bioinspired420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_calib3d420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_ccalib420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_core420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudaarithm420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudabgsegm420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudacodec420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudafeatures2d420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudafilters420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudaimgproc420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudalegacy420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudaobjdetect420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudaoptflow420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudastereo420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudawarping420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_cudev420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_datasets420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_dnn_objdetect420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_dnn_superres420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_dnn420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_dpm420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_face420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_features2d420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_flann420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_fuzzy420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_gapi420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_hfs420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_highgui420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_img_hash420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_imgcodecs420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_imgproc420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_line_descriptor420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_ml420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_objdetect420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_optflow420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_phase_unwrapping420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_photo420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_plot420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_quality420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_reg420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_rgbd420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_saliency420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_shape420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_stereo420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_stitching420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_structured_light420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_superres420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_surface_matching420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_text420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_tracking420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_video420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_videoio420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_videostab420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_xfeatures2d420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_ximgproc420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_xobjdetect420.lib \
D:\new_opencv4.2.0\opencv\x64\vc15\lib\opencv_xphoto420.lib
