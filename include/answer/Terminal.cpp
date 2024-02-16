//
// Created by liang on 24-2-16.
//

#include "Terminal.h"
#include <cmath>
Terminal::Terminal() : Node("Terminal"){
    Publisher_ = create_publisher<geometry_msgs::msg::Point32>(
            "/click_position",
            10);
    Subscription_ = create_subscription<sensor_msgs::msg::Image>(
            "/raw_image", 10, std::bind(&Terminal::callback, this, _1));
    click = cv::imread("click.png");//读取文件，还需要修改
}

void Terminal::callback(sensor_msgs::msg::Image msg) {
    cv_bridge::CvImagePtr cvImage;
    cvImage = cv_bridge::toCvCopy(msg, msg.encoding);
    cv::Mat image = cvImage->image;
    std::string outputFilename = "output.png";
    cv::imwrite(outputFilename, image);
    cv::imshow("Get",image);
    RCLCPP_INFO(this->get_logger(), "[x:%d,y:%d,ch:%d]", msg.height, msg.width, image.channels());
    RCLCPP_INFO_STREAM(this->get_logger(), "Encoding:" << msg.encoding);
    RCLCPP_INFO_STREAM(this->get_logger(), "Analysising...");
    cv::Mat grayImage;
    cv::cvtColor(image,grayImage,cv::COLOR_BGR2GRAY);

    cv::imwrite("grayImage.png", grayImage);
    cv::Mat binImage;
    cv::threshold(grayImage,binImage,200,255,cv::THRESH_BINARY);
    cv::imwrite("binImage.png", binImage);

    cv::Mat getLine = binImage.clone();
    std::vector<cv::Vec4f> lines; // 存储检测到的直线
    HoughLinesP(getLine, lines, 1, CV_PI / 180, 600,500,10); // 应用霍夫直线检测算法
    cv::Mat Lines = cv::Mat::zeros(getLine.rows,getLine.cols,CV_8UC3);
    for (size_t i = 0; i < lines.size(); i++)
    {
        cv::Vec4f l = lines[i];
        line(Lines, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]),
             cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }
    LineSet.push(Lines);                //向队列中添加
    LineProcess();

    cv::Mat grayClick;
    cv::threshold(click,grayClick,200,255,cv::THRESH_BINARY);

    count++;
}

void Terminal::save(std::string name, cv::Mat image) {
    std::string XorName = name;
    XorName += std::to_string(count);
    XorName += ".png";
    cv::imwrite(XorName, image);
}

void Terminal::LineProcess() {
    cv::Mat first = LineSet.front();
    LineSet.pop();
    save("Lines",first);
    LineSet.front() += first;

}
