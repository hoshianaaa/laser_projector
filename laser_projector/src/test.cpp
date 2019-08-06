#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/PointCloud.h>
#include <laser_geometry/laser_geometry.h>
#include <tf/transform_listener.h>

class laser_projector
{
public:
    laser_projector();
private:
    ros::NodeHandle nh_;
    ros::Subscriber sub_;
    ros::Publisher pub_;
    laser_geometry::LaserProjection projector_;
    tf::TransformListener listener_;
    void scanCallback(const sensor_msgs::LaserScanPtr& msg);
};

laser_projector::laser_projector(){
    sub_ = nh_.subscribe("scan", 1000, &laser_projector::scanCallback,this);
    pub_ = nh_.advertise<sensor_msgs::PointCloud>("test_cloud", 2, true);
}

void laser_projector::scanCallback(const sensor_msgs::LaserScanPtr& msg){
    sensor_msgs::PointCloud cloud;
    //projector_.projectLaser(*msg, cloud);
    if(!listener_.waitForTransform(
        msg->header.frame_id,
        "/base_link",
        msg->header.stamp + ros::Duration().fromSec(msg->ranges.size()*msg->time_increment),
        ros::Duration(1.0))){
        return;
    }

    projector_.transformLaserScanToPointCloud("/base_link", *msg, cloud, listener_);
    pub_.publish(cloud);
}

int main(int argc, char** argv){
    ros::init(argc, argv, "test");
    laser_projector lp;
    ros::spin();
    return 0;
}
