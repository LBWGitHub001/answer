//
// Created by liang on 24-2-15.
//
#include "answer/Terminal.h"
int Terminal::count = 0;
std::queue<cv::Mat> Terminal::LineSet;
int main(int argc, char **argv)
{
    rclcpp::init(argc,argv);
    rclcpp::spin(std::make_shared<Terminal>());
    rclcpp::shutdown();
    return 0;
}