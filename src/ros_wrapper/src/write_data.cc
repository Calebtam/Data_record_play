/**
* 
* Caleb_tam@163.com 2023年11月03日15:46:41
*
*/

#include "write_data.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Stereo_Inertial");
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);
    if (argc > 1)
    {
        ROS_WARN ("Arguments supplied via command line are ignored.");
    }

    ros::NodeHandle node_handler;
    std::string node_name = ros::this_node::getName();
    image_transport::ImageTransport image_transport(node_handler);

    std::string settings_file;
    node_handler.param<std::string>(node_name + "/settings_file", settings_file, "file_not_set");
    std::cout << "settings_file path is "  << settings_file << std::endl;

    auto config = std::make_shared<cv::FileStorage>(settings_file, cv::FileStorage::READ);
    if (!config->isOpened()) {
      config = nullptr;
      return 0;
    }
    if (!config->isOpened()) {
      printf("unable to open the configuration file!\n%s\n" , settings_file.c_str());
      std::exit(EXIT_FAILURE);
    }

    std::string cam1Topic, cam2Topic, imuTopic, camTopic;
    config->root()["cam0rostopic"]>>cam1Topic;
    config->root()["cam1rostopic"]>>cam2Topic;
    config->root()["imu0rostopic"]>>imuTopic;
    config->root()["camrostopic"]>>camTopic;
    std::cout << "cam0rostopic path is "  << cam1Topic << std::endl;
    std::cout << "cam1rostopic path is "  << cam2Topic << std::endl;
    std::cout << "imu0rostopic path is "  << imuTopic << std::endl;
    std::cout << "camrostopic path is "  << camTopic << std::endl;

    Sync iSync;
    ImuGrabber imugb(&iSync);
    ImageGrabber igb(&imugb, &iSync);

    ros::Subscriber sub_imu = node_handler.subscribe(imuTopic, 1000, &ImuGrabber::GrabImu, &imugb); 
    ros::Subscriber sub_img = node_handler.subscribe(camTopic, 100, &ImageGrabber::GrabImage, &igb);
    ros::Subscriber sub_img_left = node_handler.subscribe(cam1Topic, 100, &ImageGrabber::GrabImageLeft, &igb);
    ros::Subscriber sub_img_right = node_handler.subscribe(cam2Topic, 100, &ImageGrabber::GrabImageRight, &igb);

    // setup_ros_publishers(node_handler, image_transport);

    auto sync_thread = std::make_shared<thread>(&ImageGrabber::Data_Sync, &igb);
    auto console_thread = std::make_shared<thread>(&Sync::thread, &iSync);

    ros::spin();

    return 0;
}
void ImgWriter::WriterImg(const cv::Mat &img_msg1, const cv::Mat &img_msg2)
{
    factory_id++;
    stringstream stream;
    stream << factory_id;
    string result;
    stream >> result;
    string path = DataSavePath + "/camera/L_" + result + ".jpg";
    imwrite(path, img_msg1);
    path = DataSavePath + "/camera/R_" + result + ".jpg";
    imwrite(path, img_msg2);
}
void ImageGrabber::GrabImage(const sensor_msgs::ImageConstPtr &img_msg)
{
    // std::cout << "GrabImageLeft callback "  << std::endl;
    if(mpSync->check() != -1)
    {
        mBufMuteximg.lock();
        imgBuf.push(img_msg);
        mBufMuteximg.unlock();
    }
    // else
    // {
    //     std::cout << "stop: imgBuf" << std::endl;
    // }
}
void ImageGrabber::GrabImageLeft(const sensor_msgs::ImageConstPtr &img_msg)
{
    // std::cout << "GrabImageLeft callback "  << std::endl;
    if(mpSync->check() != -1)
    {
        mBufMutexLeft.lock();
        imgLeftBuf.push(img_msg);
        mBufMutexLeft.unlock();
    }
    // else
    // {
    //     std::cout << "stop: imgLeftBuf" << std::endl;
    // }    
}

void ImageGrabber::GrabImageRight(const sensor_msgs::ImageConstPtr &img_msg)
{
    // std::cout << "GrabImageRight callback "  << std::endl;
     if(mpSync->check() != -1)
    {
        mBufMutexRight.lock();
        imgRightBuf.push(img_msg);
        mBufMutexRight.unlock();        
    }
    // else
    // {
    //     std::cout << "stop: imgRightBuf" << std::endl;
    // }    
}
void ImuGrabber::GrabImu(const sensor_msgs::ImuConstPtr &imu_msg)
{
    // std::cout << "GrabImu callback "  << std::endl;
    if(mpSync->check() != -1)
    {
        mBufMutexImu.lock();
        imuBuf.push(imu_msg);
        mBufMutexImu.unlock();
    }
    // else
    // {
    //     std::cout << "stop: imu_msg" << std::endl;
    // }
    // return;
}
cv::Mat ImageGrabber::GetImage(const sensor_msgs::ImageConstPtr &img_msg)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvShare(img_msg, sensor_msgs::image_encodings::MONO8);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
    
    if(cv_ptr->image.type()==0)
    {
        return cv_ptr->image.clone();
    }
    else
    {
        std::cout << "Error type" << std::endl;
        return cv_ptr->image.clone();
    }
}
cv_bridge::CvImageConstPtr ImageGrabber::GetImagePtr(const sensor_msgs::ImageConstPtr &img_msg)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvShare(img_msg, sensor_msgs::image_encodings::MONO8);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
    
    if(cv_ptr->image.type()==0)
    {
        return cv_ptr;
    }
    else
    {
        std::cout << "Error type" << std::endl;
        return cv_ptr;
    }
}

// 
void ImageGrabber::Data_Sync()
{
    const double maxTimeDiff = 0.01;
    DataWriter txt_("/home/tam/tmp",3);
    txt_.IsFileExitAccess("/home/tam/tmp/camera");
    ImgWriter imWriter_("/home/tam/tmp");

    while(running)
    {
        if(mpSync->check() == -1 && imgLeftBuf.empty() && imgRightBuf.empty() && mpImuGb->imuBuf.empty())  
        {
            running = false;
            std::cout << "stop: running false" << std::endl;
        }
        cv::Mat imLeft, imRight;
        double tImLeft = 0, tImRight = 0;
        sensor_msgs::ImuConstPtr imu_msg;

        if (!imgLeftBuf.empty()&&!imgRightBuf.empty())
        {
            tImLeft = imgLeftBuf.front()->header.stamp.toSec();
            tImRight = imgRightBuf.front()->header.stamp.toSec();

            this->mBufMutexRight.lock();
            while((tImLeft-tImRight)>maxTimeDiff && imgRightBuf.size()>1)
            {
                imgRightBuf.pop();
                tImRight = imgRightBuf.front()->header.stamp.toSec();
            }
            this->mBufMutexRight.unlock();

            this->mBufMutexLeft.lock();
            while((tImRight-tImLeft)>maxTimeDiff && imgLeftBuf.size()>1)
            {
                imgLeftBuf.pop();
                tImLeft = imgLeftBuf.front()->header.stamp.toSec();
            }
            this->mBufMutexLeft.unlock();

            if((tImLeft-tImRight)<maxTimeDiff && (tImRight-tImLeft)<maxTimeDiff)
            {
                this->mBufMutexLeft.lock();
                imLeft = GetImage(imgLeftBuf.front());
                ros::Time msg_time = imgLeftBuf.front()->header.stamp;
                imgLeftBuf.pop();
                this->mBufMutexLeft.unlock();

                this->mBufMutexRight.lock();
                imRight = GetImage(imgRightBuf.front());
                imgRightBuf.pop();
                this->mBufMutexRight.unlock();

                cv::imshow("imLeft", imLeft);
                cv::imshow("imRight", imRight);
                // cv::waitKey(0); 

                imWriter_.WriterImg(imLeft, imRight);
                cv::waitKey(100);           
                
                std::chrono::milliseconds tSleep(1);
                std::this_thread::sleep_for(tSleep);
            }
        }
        mpImuGb->mBufMutexImu.lock();
        if(!mpImuGb->imuBuf.empty())
        {            // Load imu measurements from buffer
            std::ofstream out(txt_.DataSavePath.at(0), std::ios::app);
            while(!mpImuGb->imuBuf.empty())
            {
                out<<std::fixed<<std::setprecision(2)  << mpImuGb->imuBuf.front()->linear_acceleration.x
                                                    <<"\t\t"<<  mpImuGb->imuBuf.front()->angular_velocity.x
                                                    // <<"\t\t"<<way_points(2)
                                                    << std::endl; 
                mpImuGb->imuBuf.pop();
            }
            out.close();
        }
        mpImuGb->mBufMutexImu.unlock();
    }    
    ros::shutdown();
}

