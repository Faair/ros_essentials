#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <aruco_identification/aruco_identification.hpp>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/aruco/dictionary.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <geometry_msgs/Pose.h>

static const std::string OPENCV_WINDOW = "Image window";

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  ros::Publisher pubGoal;

public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscribe to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/camera1/image_raw", 1,
      &ImageConverter::imageCallback, this);
    image_pub_ = it_.advertise("/camera1/image_raw_opencv", 1);
    pubGoal = nh_.advertise<geometry_msgs::Pose>("/goal",1);
    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }
  
  void imageCallback(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    // Draw an example circle on the video stream
    if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
      cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));

    bir::Aruco arucoIn(cv::aruco::DICT_ARUCO_ORIGINAL);
    bir::Aruco::marks mark;
    mark = arucoIn(cv_ptr->image);
    cv::aruco::drawDetectedMarkers(cv_ptr->image, mark.corner, mark.id);
    // Update GUI Window
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);

    // Output modified video stream
    //Convert this message to a ROS sensor_msgs::Image message. 
    image_pub_.publish(cv_ptr->toImageMsg());

    if(mark == 998){
      geometry_msgs::Pose goal;
      goal.position.x = 0.4;
      goal.position.y = 0.6;
      goal.position.z = 0.2;
      goal.orientation.w = 1.0;
      pubGoal.publish(goal);
      //image_sub_.shutdown();
    }
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}