//
// Created by liang on 24-2-16.
//

#ifndef ANSWER_TERMINAL_H
#define ANSWER_TERMINAL_H

#include <iostream>
#include <thread>
#include <memory>
#include <deque>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/point32.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <opencv2/opencv.hpp>

#include <cv_bridge/cv_bridge.h>
using namespace std::chrono_literals;
using std::placeholders::_1;

class Terminal : public rclcpp::Node{
private:
    rclcpp::Publisher<geometry_msgs::msg::Point32>::SharedPtr Publisher_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr Subscription_;
    cv::Mat click;
    static std::queue<cv::Mat> LineSet;
    static int count;

    void callback(sensor_msgs::msg::Image msg);
    void save(std::string name,cv::Mat image);
    void LineProcess();
public:
    Terminal();

};


#endif //ANSWER_TERMINAL_H
