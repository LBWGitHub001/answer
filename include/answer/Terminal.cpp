//
// Created by liang on 24-2-16.
//

#include "Terminal.h"

Terminal::Terminal() : Node("Terminal"){
    Publisher_ = create_publisher<geometry_msgs::msg::Point32>(
            "/click_position",
            10);
    Subscription_ = create_subscription<sensor_msgs::msg::Image>(
            "/raw_image", 10, std::bind(&Terminal::callback, this, _1));
}

void Terminal::callback(sensor_msgs::msg::Image msg) {
    int bias = 15;
    cv_bridge::CvImagePtr cvImage;
    cvImage = cv_bridge::toCvCopy(msg, msg.encoding);
    cv::Mat image = cvImage->image;
    cv::Mat binImage;
    cv::cvtColor(image,binImage,cv::COLOR_BGR2GRAY);
    cv::threshold(binImage,binImage,100,255,cv::THRESH_BINARY);

    int rows = image.rows;
    int cols = image.cols;

    //通过边境获取判定线的两个端点，从而确定这条直线
    cv::Mat leftCorr(binImage,cv::Range(0,rows),cv::Range(0,1));
    cv::Point Point(0,0);

    cv::minMaxLoc(leftCorr,0,0,0,&Point);

    cv::Mat lineFound = cv::Mat::zeros(rows,cols,CV_8UC1);
    if(Point.x+Point.y!=0) {
        cv::line(lineFound, Point, cv::Point(cols - 1, Point.y), 255, bias, 8);

        std::vector<cv::Mat> Channel;
        cv::split(image, Channel);
        cv::threshold(Channel[2], Channel[2], 200, 255, cv::THRESH_BINARY);
        cv::threshold(Channel[0], Channel[0], 200, 255, cv::THRESH_BINARY);

        cv::Mat clickFound;
        cv::bitwise_xor(Channel[2], Channel[0], clickFound);
        cv::bitwise_and(clickFound, Channel[0], clickFound);
        cv::Mat andResult;
        cv::bitwise_and(clickFound, lineFound, andResult);
        cv::Point touchPoint;
        cv::Mat Result(andResult, cv::Range(Point.y - bias, Point.y + bias), cv::Range(0, cols));
        cv::minMaxLoc(Result, 0, 0, 0, &touchPoint);

        geometry_msgs::msg::Point32 output;
        output.x = touchPoint.x+10;
        output.y = touchPoint.y;
        Publisher_->publish(output);
    }


}

