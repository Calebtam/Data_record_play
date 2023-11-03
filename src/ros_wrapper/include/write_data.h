#ifndef WRITE_DATA_H
#define WRITE_DATA_H

#include "common.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <sstream>
#include <vector>

// creat new file
#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "txt_writer.h"
#include "Sync.h"

using namespace std;

class ImgWriter
{
public:
    ImgWriter(const string Path): DataSavePath(Path){}

    void WriterImg(const cv::Mat &img_msg);
    void WriterImg(const cv::Mat &img_msg1, const cv::Mat &img_msg2);
    string DataSavePath;
    long factory_id = 0;
};

class ImuGrabber
{
public:
    ImuGrabber(Sync *pSync): mpSync(pSync){};

    void GrabImu(const sensor_msgs::ImuConstPtr &imu_msg);

    queue<sensor_msgs::ImuConstPtr> imuBuf;
    std::mutex mBufMutexImu;
    Sync *mpSync;
};

class ImageGrabber
{
public:
    ImageGrabber(ImuGrabber *pImuGb, Sync *pSync): mpImuGb(pImuGb), mpSync(pSync){}

    void GrabImageLeft(const sensor_msgs::ImageConstPtr& msg);
    void GrabImageRight(const sensor_msgs::ImageConstPtr& msg);
    void GrabImage(const sensor_msgs::ImageConstPtr &img_msg);

    cv::Mat GetImage(const sensor_msgs::ImageConstPtr &img_msg);
    cv_bridge::CvImageConstPtr GetImagePtr(const sensor_msgs::ImageConstPtr &img_msg);

    void Data_Sync();
    bool running = true;

    queue<sensor_msgs::ImageConstPtr> imgLeftBuf, imgRightBuf, imgBuf;
    std::mutex mBufMutexLeft,mBufMutexRight,mBufMuteximg;
    ImuGrabber *mpImuGb;
    Sync *mpSync;
};

#endif  // WRITE_DATA_H