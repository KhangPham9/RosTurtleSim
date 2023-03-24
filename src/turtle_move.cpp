#include <ros/ros.h>
#include <turtlesim/Pose.h>
#include <geometry_msgs/Twist.h>
#include <cmath>

#include "turtle/TurtleMov.h"
#include "turtle/TurtleRot.h"

class Turtle {
    private:
        ros::Publisher cmd_vel_pub;
        ros::Subscriber pose_sub;
        ros::ServiceServer move_turtle;
        ros::ServiceServer rot_turtle;

        bool pose_set = false;
        float x, y, yaw;
    public:
        Turtle(ros::NodeHandle *n) {
            cmd_vel_pub = n->advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 100);
            pose_sub = n->subscribe("/turtle1/pose", 100, &Turtle::pose_callback, this);
            move_turtle = n->advertiseService("move_turtle", &Turtle::move_service, this);
            rot_turtle = n->advertiseService("rotate_turtle", &Turtle::rotate_service, this);

            
            ros::Subscriber initial_pose = n->subscribe("/turtle1/pose", 100, &Turtle::wait_for_initial_pose, this);

            while(!pose_set) {
                ros::spinOnce();
            }


        }

        void wait_for_initial_pose(const turtlesim::Pose::ConstPtr& pose) {
            x = pose->x;
            y = pose->y;

            yaw = pose->theta;
            ROS_INFO("\nTurtle x: %f\nTurtle y: %f\nTurtle yaw: %f\n", x, y, yaw);
            pose_set = true;
        }

        void pose_callback(const turtlesim::Pose::ConstPtr& pose) {
            // ROS_INFO("\nTurtle x: %f\nTurtle y: %f\nTurtle theta: %f\nTurtle linear_velocity: %f\nTurtle angular_velocity: %f\n",
            //         pose->x, pose->y, pose->theta, pose->linear_velocity, pose->angular_velocity);
                    // geometry_msgs::Twist t;
    
                    x = pose->x;
                    y = pose->y;

                    yaw = pose->theta;
                    // ROS_INFO("Yaw: %f", yaw);
                    // cmd_vel_pub.publish(t);
        }

        void move(float speed, float distance) {
            float start_x = x,
                  start_y = y;
                //   start_theta = theta;

            float distance_moved = 0.0;

            ros::Rate move_frequency(62.5);

            geometry_msgs::Twist t;
            t.linear.x = speed;
            t.linear.y = 0;
            t.linear.z = 0;
            t.angular.x = 0;
            t.angular.y = 0;
            t.angular.z = 0;

            while(ros::ok()) {
                if (distance_moved >= distance) {
                    ROS_INFO("\nTurtle x: %f\nTurtle y: %f\n", x, y);
                    ROS_INFO("Finished moving");
                    move_frequency.sleep();
                    break;
                }
                cmd_vel_pub.publish(t);
                distance_moved += std::abs(std::sqrt(std::pow(x - start_x, 2) + std::pow(y - start_y , 2)));
                move_frequency.sleep();


                start_x = x;
                start_y = y;

                ROS_INFO("Turtle moved %f\n", distance_moved);

                ros::spinOnce();
            }

            t.linear.x = 0;
            cmd_vel_pub.publish(t);

        }


        bool move_service(turtle::TurtleMov::Request &req, turtle::TurtleMov::Response &res) {
            ROS_INFO("Beginning Turtle Movement");
            Turtle::move(req.speed, req.distance);
            ROS_INFO("Completed Movement");

            return true;
        }


        void rotate(float speed, float theta) {
            float start_theta = yaw;

            ros::Rate rotate_frequency(62.5);

            geometry_msgs::Twist t;
            t.linear.x = 0;
            t.linear.y = 0;
            t.linear.z = 0;
            t.angular.x = 0;
            t.angular.y = 0;
            t.angular.z = speed;

            float theta_rotated = 0.0;
            while(ros::ok()) {

                if (theta_rotated >= theta) {
                    ROS_INFO("Finished Rotation");
                    break;
                }
                cmd_vel_pub.publish(t);
                theta_rotated += std::abs((start_theta - yaw));
                rotate_frequency.sleep();

                // ROS_INFO("Yaw %f", yaw);
                ROS_INFO("Turtle rotated %f\n", theta_rotated);
                ROS_INFO("Turtle yaw %f\n", yaw);

                start_theta = yaw;
                ros::spinOnce();
            }

            t.angular.z = 0;
            cmd_vel_pub.publish(t);
        }


        bool rotate_service(turtle::TurtleRot::Request &req, turtle::TurtleRot::Response &res) {
            ROS_INFO("Beginning Turtle Rotation");
            Turtle::rotate(req.speed, req.angle);
            ROS_INFO("Completed Rotation");
            return true;
        }
};



int main(int argc, char** argv) {
    ros::init(argc, argv, "robot_move_topic");

    ros::NodeHandle n;

    Turtle t = Turtle(&n);

    ros::Rate loop_rate(10);


    ros::spin();


    // while (ros::ok()) {

    //     ros::spinOnce();
    //     loop_rate.sleep();
    // }

    // t.move(0.1, 5.0);
    // t.rotate(0.4, 1);
    // t.move(1.0, 20.0);

    
    return 0;
}


