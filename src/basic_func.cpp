#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <tf/transform_listener.h>

using namespace std;

//nav_msgs::Odometry odom_var;

class basic_func{
public:
  basic_func(ros::NodeHandle& nh) : StartTime(ros::Time::now()),Duration(0.f){
    srand(time(NULL));
    commandPub = nh.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 1);
    odomSub = nh.subscribe("/odom",1, &basic_func::poseCallback, this);
    //combinedSub = nh.subscribe("/robot_pose_ekf/odom_combined",1, &basic_func::combinedCallback, this);
   // laserSub = nh.subscribe("/base_scan",1, &Bot::laserCallback, this);
   //poseSub = nh.subscribe("/base_pose_ground_truth", 1, &Pose::poseCallback, this);
  };

void poseCallback(const nav_msgs::Odometry::ConstPtr& msg) {
  //ROS_INFO_STREAM("IN ODOM CALLBACK");
   posX = msg->pose.pose.position.x;
   posY = msg->pose.pose.position.y;
   //angle from pi to -pi
   angle = tf::getYaw(msg->pose.pose.orientation);
   angle += M_PI;
  ROS_INFO_STREAM("Posx: " << posX << " Posy: " << posY << " angle:" << angle);
  odom = 1;
};

void print(){
  //if(odomPosition.x > 0.2) {
  //ROS_INFO_STREAM("Odom Positionx: " <<odomPosition.x << "Odom Positiony: " << odomPosition.y);
  //ROS_INFO_STREAM("Combined Position: " <<combinedPosition.x);
}
//}
void combinedCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg) {
//ROS_INFO_STREAM("IN CALLBACK");
combinedPosition = msg->pose.pose.position;
//double x = msg->pose.pose.position.x;
//double y = msg->pose.pose.position.y;
//double theta = tf::getYaw(msg->pose.pose.orientation);
//ROS_INFO_STREAM(combinedPosition.x);  
};

void move(double linearVelMPS, double angularVelRadPS) {
  //ROS_INFO_STREAM("In move");
  geometry_msgs::Twist msg;
 // ROS_INFO_STREAM("Flag 1");
  msg.linear.x = linearVelMPS;
 // ROS_INFO_STREAM("Flag 2");
  msg.angular.z = angularVelRadPS;
 // ROS_INFO_STREAM("Flag 3");
  commandPub.publish(msg);
 // ROS_INFO_STREAM("Flag 4");
 //ROS_INFO_STREAM("linear x:" << msg.linear.x);
}
void translate(double dist) {
 geometry_msgs::Twist msg;
 StartTime = ros::Time::now();
 float timeForTrans = dist/FORWARD_SPEED_MPS; //LFACTOR
 while(ros::Time::now() < StartTime + ros::Duration(timeForTrans)) {
  move(FORWARD_SPEED_MPS,0);
  }
 //ROS_INFO_STREAM(posX);
 move(0.0,0.0);
 //ros::shutdown();
}
int rotate_rel(double angle_input) {
  ROS_INFO_STREAM("In rel");
  //StartTime = ros::Time::now();
   theta = angle + angle_input;
   if(theta > 2*M_PI) {
     theta = theta - 2*M_PI;
   }
     theta = theta - 0.3; //deleting the approximate error
  
  ROS_INFO_STREAM("new angle: " << theta);
  //ROS_INFO_STREAM("new angle: " << theta);
  //float timeForTrans = theta/ROTATE_SPEED_RADPS; 
  //while(ros::Time::now() < StartTime + ros::Duration(timeForTrans)){
    // move(0, ROTATE_SPEED_RADPS);
  //} 
  //move(0,0);
  //ros::shutdown();
}
void rotate_abs(double angle) {
  //ROS_INFO_STREAM("In abs");
  StartTime = ros::Time::now();
  float timeForTrans2 = angle/ROTATE_SPEED_RADPS + RFACTOR; //RFACTOR
  while(ros::Time::now() < StartTime + ros::Duration(timeForTrans2)){
    move(0,ROTATE_SPEED_RADPS);
  }
  //move(0,0);
  //ros::shutdown();
}
void square(){
  ROS_INFO_STREAM(angle);
  for( int i = 0 ; i < 4 ; ++i) {
   // print();
    translate(1);
    rotate_abs(M_PI/2);
  }
  ROS_INFO_STREAM(angle);
  ros::shutdown();
}

void spin() {
 ros::Rate rate(10);
  counter = 0;
  odom = 0;
  squares = 1;
  bool runRelative = false;
  bool runMeter = false;
  bool runSquare = true;
  int signs;
  while(ros::ok()){
    if(runRelative == true){
       
      if(odom == 1) {
        if (counter == 0) {
          rotate_rel(M_PI/6);
          //ROS_INFO_STREAM(angle);
        }
        move(0,ROTATE_SPEED_RADPS);
        if(theta-0.05 < angle && theta+0.05 > angle) {
        break;
        }
        counter++;
      }
      //ROS_INFO_STREAM(angle);
    }
    if(runMeter == true) {
      translate(1);
     // ROS_INFO_STREAM(posX);
      break;
    }
    if(runSquare == true) {
      if(odom == 1){
        square();
      }
    }
    ros::spinOnce();
    rate.sleep();
  }
};
const static double FORWARD_SPEED_MPS = 0.2;
const static double ROTATE_SPEED_RADPS = M_PI/8;
const static double RFACTOR = 0.33;
const static double LFACTOR = 1.9;
geometry_msgs::Point odomPosition;
protected:
geometry_msgs::Point lastpos;
//geometry_msgs::Point odomPosition;
geometry_msgs::Point combinedPosition;
ros::Publisher commandPub;
ros::Subscriber laserSub;
ros::Subscriber odomSub;
ros::Subscriber combinedSub;
ros::Time StartTime;
ros::Duration Duration;
float posX;
double posY;
float angle;
double theta;
double counter;
double odom;
double squares;
};
int main(int argc ,char **argv) { 
 ros::init(argc,argv, "basic_func");
 ros::NodeHandle n;
 //ROS_INFO_STREAM("In main");
 basic_func walk(n);
 walk.spin();
  return 0;

};
