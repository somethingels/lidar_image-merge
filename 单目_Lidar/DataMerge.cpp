
#include "DataMerge.h"

DataMerge::DataMerge() {
    // 读取配置文件
    initParam = readInitFile.getParams();
    pcdSrcPath = initParam.pcdSrcPath;
    pcdSavePath = initParam.pcdSavePath;
    imgSrcPath = initParam.imgSrcPath;
    imgSavePath = initParam.imgSavePath;
    startFramePos = initParam.startFramePos;
    // 扫描目录的全部文件
    readPcdRet = Util::scan_one_dir(pcdSrcPath.c_str(), pcdVec);
    readImgRet = Util::scan_one_dir(imgSrcPath.c_str(), imgVec);
    totalFrameNum = pcdVec.size() < imgVec.size() ? pcdVec.size() : imgVec.size();// 比较选择总帧数较小者

    DataMerge::nextPcdFrameFlag = false;
    DataMerge::prePcdFrameFlag = false;
    DataMerge::continuousFrameFlag = false;
    tCloud = boost::shared_ptr<PointCloudT >(new PointCloudT);
    cloudMerge = boost::shared_ptr<PointCloudT >(new PointCloudT);
    cloudMerge2 = boost::shared_ptr<pcl::PointCloud<pcl::PointXYZRGBA> >(new pcl::PointCloud<pcl::PointXYZRGBA>);

    imageWidth = 1280;
    imageHeight = 720;

    this->cameraInit();
    imgFlag = 0;
    cloudFlag = 0;

    if (readPcdRet > 0 && readImgRet > 0) {
        Util::mkdir_r(pcdSavePath.c_str());// 创建文件保存目录
        Util::mkdir_r(imgSavePath.c_str());// 创建文件保存目录
        boost::thread th1(boost::bind(&DataMerge::pcdReadAndView, this));// 开启线程 因为成员函数指针不能直接调用operator(),它必须被绑定到一个对象或指针，然后才能得到this指针进而调用成员函数
        th1.detach();
    }
}

DataMerge::~DataMerge() {

}
void DataMerge::Save_Merged()
{

}
void DataMerge::pcdReadAndView() {
    boost::shared_ptr<pcl::visualization::PCLVisualizer> view(new pcl::visualization::PCLVisualizer("3D Viewer"));
    view->registerKeyboardCallback(&DataMerge::kb_callback, *this, (void *) NULL);// 键盘回调事件
    view->setBackgroundColor(0, 0, 0);
    view->addCoordinateSystem(1.0);
    view->setShowFPS(true);
    view->initCameraParameters();
    view->setCameraPosition(0, 0, 2, 0, 1, -1, 0);
    view->setSize(960, 480);
    //view->addText(" ", 10, 60, 20, 1.0f, 1.0f, 1.0f, "cxt1", 0);
    //view->addText(" ", 10, 30, 20, 1.0f, 1.0f, 1.0f, "cxt", 0);

    while (startFramePos < totalFrameNum) {

        nextPcdFrameFlag = false;
        prePcdFrameFlag = false;

        pcdFileName = pcdVec.at(startFramePos);
        std::cout << "nowFramePos = " << startFramePos << "   " << pcdSrcPath + pcdFileName << std::endl;
        tCloud->clear();
        pcl::io::loadPCDFile(pcdSrcPath + pcdFileNakme, *tCloud);//读取点云文件到tcloud
        if (tCloud->empty()) {
            std::cout << "empty pcd : " << pcdSrcPath + pcdFileName << std::endl;
        }
        for (int i = 0; i < tCloud->size(); ++i) {
            tCloud->at(i).intensity = 255 - tCloud->at(i).intensity;
        }
        if (!tCloud->empty()) {
            cloudFlag = 0;
            cloudMerge->clear();
            *cloudMerge = *tCloud;
            cloudFlag = 1;
        }

        //std::cout << " " << tCloud->points[0].x << ", " <<tCloud->points[0].y << ", " <<tCloud->points[0].z << ", " << std::endl;
        // 点云着色
        pcl::visualization::PointCloudColorHandlerGenericField<PointT> findColor(tCloud, "intensity"); // tcloud按照I：intensity字段进行渲
        view->addPointCloud<PointT>(DataMerge::tCloud, findColor, "colorPointCloud");
        view->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1,
                                               "colorPointCloud"); // 设置点云大小

        std::string viewText = pcdFileName;
        // 添加文字，帧序
        view->addText(to_string(startFramePos) + " / " + to_string(totalFrameNum), 10, 60, 20, 1.0f, 1.0f, 1.0f,
                      "cxt1");
        // 添加文字，文件名
        view->addText(viewText, 10, 30, 20, 1.0f, 1.0f, 1.0f, "cxt");

        // 读取图像文件名，可设置帧偏移量
        imgFileName = imgVec.at(startFramePos + 3);

        img.release();//释放img内容
        img = cv::imread(imgSrcPath + imgFileName, cv::IMREAD_UNCHANGED); //读入一张图片
        imgFlag = 0;
        imgMerge.release();//释放imgmerge内容
        imgMerge = img.clone();
        imgFlag = 1;
        cv::Mat tempImg;
        cv::resize(img, tempImg, cv::Size(img.cols, img.rows));

        cv::Point p2(10, tempImg.rows - 50);
        cv::Point p1(10, tempImg.rows - 10);
        // 添加文字，帧序
        cv::putText(tempImg, to_string(startFramePos) + " / " + to_string(totalFrameNum), p2, CV_FONT_HERSHEY_SIMPLEX,
                    1, cv::Scalar(250, 0, 0), 3, 8);
        // 添加文字，文件名
        cv::putText(tempImg, imgFileName, p1, CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(250, 0, 0), 3, 8);

        int flag = 0;
        while (!nextPcdFrameFlag && !prePcdFrameFlag) {
            view->spinOnce(100, true);
            boost::this_thread::sleep(boost::posix_time::microseconds(1000));

            if (cloudFlag == 1 && imgFlag == 1 && !cloudMerge->empty() && !imgMerge.empty()) {
                PointCloudTPtr cloud(new PointCloudT);
                *cloud = *cloudMerge;//当前点云数据
                cv::Mat image = imgMerge.clone();

                for (int i = 0; i < cloud->size(); i++) {
                    cv::Point imagePoint, imagePoint2;//点云三维坐标转化为图像坐标
                    cv::Point3f cloudPoint;//坐标
                    pcl::PointXYZRGB temp;//存储当前点云数据
                    cloudPoint.x = temp.x = cloud->at(i).x;
                    cloudPoint.y = temp.y = cloud->at(i).y;
                    cloudPoint.z = temp.z = cloud->at(i).z;
                    temp.r = 255;
                    temp.g = 255;
                    temp.b = 255;

                    this->cloud2Image(cloudPoint, rotateMat, translateVec, newCameraMat, map, imagePoint);
                    if (0 <= imagePoint.x && imagePoint.x < image.cols && 0 <= imagePoint.y &&
                        imagePoint.y < image.rows && temp.y > 0 && temp.y < 200) {
                        cv::Vec3b tempRGB;
                        int dist = sqrt(temp.y * temp.y + temp.x * temp.x) / 100 * 255; //5米按距离显示颜色
                        //渲染
                        if (dist <= 63) {
                            tempRGB = cv::Vec3b(255, 254 - 4 * dist, 0);
                        } else if (dist > 63 && dist <= 127) {
                            tempRGB = cv::Vec3b(510 - 4 * dist, 4 * dist - 254, 0);
                        } else if (dist > 127 && dist <= 191) {
                            tempRGB = cv::Vec3b(0, 255, 4 * dist - 510);
                        } else if (dist > 191 && dist <= 255) {
                            tempRGB = cv::Vec3b(0, 1022 - 4 * dist, 255);
                        }
                        if (!image.empty()) {
                            circle(imdage, imagePoint, 1, cv::Scalar(tempRGB[0], tempRGB[1], tempRGB[2]), -1);//画圆
                        }
                    }
                }
                // 显示图像
                cv::namedWindow("MergeImage", 0);// 0-任意窗口大小,1-固定窗口大小
                cvResizeWindow("MergeImage", image.cols / 2, image.rows / 2);
                imshow("MergeImage", image);//显示当前帧图像
                //cv::waitKey(10);//延时ms
                //image.release();
            }
            // 显示图像
            cv::namedWindow("Camera", 0);// 0-任意窗口大小,1-固定窗口大小
            cvResizeWindow("Camera", tempImg.cols / 2, tempImg.rows / 2);
            imshow("Camera", tempImg);//显示当前帧图像
            cv::waitKey(1);//延时ms

            if (continuousFrameFlag) {
                nextPcdFrameFlag = true;
                startFramePos++;
            }
        }
        view->removeAllPointClouds();
        view->removeAllShapes();
    }
}

void DataMerge::kb_callback(const pcl::visualization::KeyboardEvent &event, void *nothing) {
    unsigned char keyCode = event.getKeyCode();
    bool flag = event.keyDown();
    usleep(100 * 1000);// 100ms
    if ((keyCode == '9') && flag) {
        continuousFrameFlag = true;
    }
    if ((keyCode == '7') && flag) {
        continuousFrameFlag = false;
    }
    // 下一帧，按键盘 n/N 键（next）
    if ((keyCode == 'n' || keyCode == 'N') && flag) {
        DataMerge::nextPcdFrameFlag = true;
        startFramePos++;
    }
    // 下一帧跳过10帧，按键盘 m/M，数字区6/2 键（next）
    if ((keyCode == 'm' || keyCode == 'M' || keyCode == '6' || keyCode == '2') && flag) {
        DataMerge::nextPcdFrameFlag = true;
        startFramePos += 10;
    }
    // 上一帧，按键盘 b/B 键（back）
    if ((keyCode == 'b' || keyCode == 'B') && flag) {
        DataMerge::prePcdFrameFlag = true;
        if (startFramePos > 0) {
            startFramePos--;
        }
    }
    // 上一帧跳过10帧，按键盘 v/V，数字区4/8 键（back）
    if ((keyCode == 'v' || keyCode == 'V' || keyCode == '4' || keyCode == '8') && flag) {
        DataMerge::prePcdFrameFlag = true;
        if (startFramePos > 10) {
            startFramePos -= 10;
        } else {
            startFramePos = 0;
        }
    }
    // 保存、自动下一帧，键盘 s/S 键（save）
    if ((keyCode == 's' || keyCode == 'S') && flag) {

        // 复制文件
        Util::CopyFile((pcdSrcPath + pcdFileName).c_str(), (pcdSavePath + pcdFileName).c_str());
        Util::CopyFile((imgSrcPath + imgFileName).c_str(), (imgSavePath + imgFileName).c_str());
        usleep(100 * 1000);// 100ms
        std::cout << "** saveFramePos = " << startFramePos << " **" << std::endl;

        DataMerge::nextPcdFrameFlag = true;
        startFramePos++;
    }
}

void DataMerge::cameraInit() {
    //摄像头参数初始化
    cout << "-------------------------------------------------------" << endl;

    double cameraCoef[9] = {884.77806, 0., 638.84322, 0., 882.16801, 366.08318, 0., 0., 1.};
    double distCoef[5] = {-0.445564, 0.181217, -0.000404, 0.001887, 0};
    //double cameraCoef[9] = {582, 0, 480, 0, 1611, 270, 0, 0, 1};
    //double distCoef[5] = {-0.326, 0.098, 0.0008, -0.0001, 0};

    cv::Mat cameraMat(3, 3, CV_64F, cameraCoef);
    cv::Mat distMat(5, 1, CV_64F, distCoef);
    cameraMat = cameraMat.clone();
    distMat = distMat.clone();
    cout << "cameraMat = " << cameraMat << endl;
    cout << "distMat = " << distMat.t() << endl;

    // 生成带畸变的焦距、光心像素坐标的矩阵
    newCameraMat = getOptimalNewCameraMatrix(cameraMat, distMat, cv::Size(imageWidth, imageHeight), 1,
                                             cv::Size(imageWidth, imageHeight), 0);
    //cout << "newCameraMat = " << newCameraMat << endl;
    initUndistortRectifyMap(cameraMat, distMat, cv::Mat(), newCameraMat, cv::Size(imageWidth, imageHeight),
                            CV_16SC2,
                            map, tmpMap);

    rotateMat = cv::Mat(3, 3, CV_64F, cv::Scalar(0));
    translateVec = cv::Mat(1, 3, CV_64F, cv::Scalar(0));
    //欧拉角微调，再转成旋转矩阵
    rotateAngle[0] = 90.0;
    rotateAngle[1] = -1.0;
    rotateAngle[2] = 0.0;
    cout << "rotateAngle1 = " << rotateAngle[0] << "  " << rotateAngle[1] << "  " << rotateAngle[2] << "  " << endl;
    double pitch = rotateAngle[0] * CV_PI / 180.0;
    double roll = rotateAngle[1] * CV_PI / 180.0;
    double heading = rotateAngle[2] * CV_PI / 180.0;
    rotateMat.at<double>(0, 0) = cos(heading) * cos(roll);
    rotateMat.at<double>(0, 1) = sin(heading) * cos(roll);
    rotateMat.at<double>(0, 2) = -sin(roll);
    rotateMat.at<double>(1, 0) = -sin(heading) * cos(pitch) + cos(heading) * sin(roll) * sin(pitch);
    rotateMat.at<double>(1, 1) = cos(heading) * cos(pitch) + sin(heading) * sin(roll) * sin(pitch);
    rotateMat.at<double>(1, 2) = cos(roll) * sin(pitch);
    rotateMat.at<double>(2, 0) = sin(heading) * sin(pitch) + cos(heading) * sin(roll) * cos(pitch);
    rotateMat.at<double>(2, 1) = -cos(heading) * sin(pitch) + sin(heading) * sin(roll) * cos(pitch);
    rotateMat.at<double>(2, 2) = cos(roll) * cos(pitch);
    transDist[0] = -0.15;
    transDist[1] = 0.0;
    transDist[2] = 0;
    translateVec.at<double>(0) = transDist[0];    //x
    translateVec.at<double>(1) = transDist[1];    //z
    translateVec.at<double>(2) = transDist[2];    //y
    cout << "translateVec = " << translateVec << endl;
}

void DataMerge::cloud2Image(cv::Point3f cloudPoint, cv::Mat rotateMat, cv::Mat translateVec, cv::Mat newCameraMat,
                            cv::Mat map, cv::Point &imagePoint) {
    cv::Mat point(1, 3, CV_64F);
    // 世界坐标系点经旋转、平移，转到相机坐标系下
    double fp[3] = {cloudPoint.x, cloudPoint.y, cloudPoint.z};
    for (int i = 0; i < 3; i++) {
        point.at<double>(i) = translateVec.at<double>(i);
        for (int j = 0; j < 3; j++)
            point.at<double>(i) += double(fp[j]) * rotateMat.at<double>(j, i);
    }

    //有畸变 相机坐标系点转为像素坐标系点（图像左上角为像素坐标系原点，水平x轴，竖直y轴）
    double tmpx = point.at<double>(0) / point.at<double>(2);
    double tmpy = point.at<double>(1) / point.at<double>(2);
    double px = newCameraMat.at<double>(0, 0) * tmpx + newCameraMat.at<double>(0, 2);
    double py = newCameraMat.at<double>(1, 1) * tmpy + newCameraMat.at<double>(1, 2);
    if (cvRound(py) < 0 || cvRound(py) >= map.rows || cvRound(px) < 0 || cvRound(px) >= map.cols) {
        imagePoint.x = -1;
        imagePoint.y = -1;
    } else {
        imagePoint.x = map.at<short>(cvRound(py), 2 * cvRound(px));
        imagePoint.y = map.at<short>(cvRound(py), 2 * cvRound(px) + 1);
    }
}



