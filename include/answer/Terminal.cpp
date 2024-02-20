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

    //cv::imwrite("grayImage.png", grayImage);
    cv::Mat binImage;
    cv::threshold(grayImage,binImage,100,255,cv::THRESH_BINARY);
    //cv::imwrite("binImage.png", binImage);
    std::vector<cv::Vec4f> lines; // 存储检测到的直线
/*
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::morphologyEx(binImage,binImage,cv::MORPH_CLOSE,kernel);
    cv::imwrite("BIN.bng",binImage);*/
    //检测click和判定线
    cv::Mat corrider;
    cv::Canny(binImage,corrider,50,150);
    cv::imwrite("Canny.png",corrider);

    HoughLinesP(binImage, lines, 1, CV_PI / 180, 100,60,3); // 应用霍夫直线检测算法
    //save("image",binImage);
    //Lines是图片lines是vector
    cv::Mat judgeLine = image.clone();
    cv::Mat click = image.clone();
    for (size_t i = 0; i < lines.size(); i++)
    {
        cv::Vec4f l = lines[i];
        cv::Scalar color;
        cv::Point p1(l[0], l[1]);
        cv::Point p2(l[2], l[3]);
        int distance = (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
        int yu = 160000;
        if(distance > yu){
            color = cv::Scalar(255,0,0);//蓝
            line(judgeLine, p1, p2,color, 4,8);//, cv::LINE_AA
        }
        else{
            color = cv::Scalar(0,0,255);//红
            line(click, p1, p2,color, 2,4);//, cv::LINE_AA
        }

    }

    save("JudgeLine",judgeLine);
    save("dClick",click);
    /*
    //LineSet.push(Lines);                //向队列中添加

    cv::Mat grayLine;
    cv::cvtColor(Lines,grayLine,CV_RGB2GRAY);
    cv::threshold(grayLine,grayLine,20,255,cv::THRESH_BINARY);
    //cv::imwrite("grayLine.png",grayLine);

    cv::bitwise_xor(grayLine,binImage,binImage);
    //cv::imwrite("binImageE.png", binImage);
    cv::Mat grayClick;
    cv::threshold(click,grayClick,200,255,cv::THRESH_BINARY);
    cv::Mat resultClick;
    //cv::matchTemplate(binImage,grayClick,resultClick,CV_TM_SQDIFF);
    cv::Point point;
    cv::minMaxLoc(resultClick,0,0,&point);
    line(binImage, cv::Point(point.x, point.y), cv::Point(point.x+click.cols,point.y),
         cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    cv::imwrite("binImageF.png", binImage);
    */

}

void Terminal::save(std::string name, cv::Mat image) {
    std::string XorName = name;
    XorName += std::to_string(count);
    XorName += ".png";
    cv::imwrite(XorName, image);
    count++;
    //geometry_msgs::msg::Point32 msg;msg.x=count;msg.y=count;
    //Publisher_->publish(msg);
}

void Terminal::LineProcess() {
    cv::Mat first = LineSet.front();
    LineSet.pop();
    save("Lines",first);
    LineSet.front() += first;

}
