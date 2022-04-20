#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/io/pcd_io.h>
#include <boost/thread/thread.hpp>
#include <opencv2/opencv.hpp>
#include "Util.h"
#include "ReadInitFile.h"

#define PointT pcl::PointXYZI
#define PointCloudT pcl::PointCloud<PointT>
#define PointCloudTPtr pcl::PointCloud<PointT>::Ptr

class DataMerge {
public:
    DataMerge();

    virtual ~DataMerge();

    void pcdReadAndView();

    bool nextPcdFrameFlag;// 下一帧标志
    bool prePcdFrameFlag;// 上一帧标志
    bool continuousFrameFlag;// 连续显示标志

    std::string pcdSrcPath;// 源文件路径
    std::string pcdSavePath;// 保存目标路径
    std::string pcdFileName;// 文件名

    std::string imgSrcPath;// 源文件路径
    std::string imgSavePath;// 保存目标路径
    std::string imgFileName;// 文件名

    int totalFrameNum;// 文件总帧数
    int startFramePos;// 起始帧位置

    cv::Mat cameraMat;// 相机内参
    cv::Mat distMat;// 相机畸变系数
    double rotateAngle[3];// 旋转角度，度数
    double transDist[3];// 平移距离，米
private:

    PointCloudTPtr tCloud;
    PointCloudTPtr cloudMerge;
    pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloudMerge2;
    cv::Mat img;
    cv::Mat imgMerge;
    int imgFlag;
    int cloudFlag;

    void kb_callback(const pcl::visualization::KeyboardEvent &event, void *nothing);// 读取键盘事件
    void cameraInit();

    void cloud2Image(cv::Point3f cloudPoint, cv::Mat rotateMat, cv::Mat translateVec, cv::Mat newCameraMat, cv::Mat map,
                     cv::Point &imagePoint);

    cv::Mat rotateMat;
    cv::Mat translateVec;
    cv::Mat map, tmpMap, newCameraMat;
    int imageWidth;
    int imageHeight;

    ReadInitFile readInitFile;// 读取配置文件类对象
    InitParam initParam;// 初始化参数
    std::vector<std::string> pcdVec;// pcd文件列表
    std::vector<std::string> imgVec;// 文件列表
    int readPcdRet;// 文件读取成功与否
    int readImgRet;// 文件读取成功与否

    int count;

};


