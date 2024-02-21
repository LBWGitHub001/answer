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
    //click = cv::imread("click.png");//读取文件，还需要修改
}

void Terminal::callback(sensor_msgs::msg::Image msg) {
    cv_bridge::CvImagePtr cvImage;
    cvImage = cv_bridge::toCvCopy(msg, msg.encoding);
    cv::Mat image = cvImage->image;
    std::string outputFilename = "output.png";
    cv::imwrite(outputFilename, image);
    cv::Mat grayImage;
    cv::cvtColor(image,grayImage,cv::COLOR_BGR2GRAY);

    cv::Mat binImage;
    cv::threshold(grayImage,binImage,100,255,cv::THRESH_BINARY);

    int rows = image.rows;
    int cols = image.cols;

    //Lines是图片lines是vector

std::vector<int> str;
std::vector<int> end;
    //RCLCPP_INFO_STREAM(get_logger(),"image:rows:" << rows<<" cols:"<<cols);
    for(int i = 0;i <= rows;i++){
        if(binImage.at<uchar>(i,0) >= 200){
            str.push_back(i);
        }

    }
    for(int i = 0;i <= rows;i++){
        if(binImage.at<uchar>(i,cols-1) > 200){
            end.push_back(i);
        }
    }

    cv::Point pLU;pLU.x=0;pLU.y=(str[0]+str[str.size()-1])/2;
    cv::Point pRD;pRD.x=cols-1;pRD.y=(end[0]+end[end.size()-1])/2;
    //RCLCPP_INFO_STREAM(get_logger(),"Size:"<<str.size()<<" "<<end.size());
    //RCLCPP_INFO_STREAM(get_logger(),"Line:"<<pLU.y<<" "<<pRD.y);
    cv::Mat lineFound = cv::Mat::zeros(image.rows,image.cols,CV_8UC1);
    int bias=60;
    //设置线的宽度，用来解决延迟问题
    line(lineFound,pLU,pRD,255,bias,cv::LINE_AA);
    //
    std::vector<cv::Mat> Channel;
    cv::split(image,Channel);
    cv::threshold(Channel[2],Channel[2],200,255,cv::THRESH_BINARY);
    cv::threshold(Channel[0],Channel[0],200,255,cv::THRESH_BINARY);

    cv::Mat clickFound;
    cv::bitwise_xor(Channel[2],Channel[0],clickFound);
    cv::bitwise_and(clickFound,Channel[0],clickFound);
    //cv::imwrite("clickFound.png",clickFound);
    //cv::imwrite("lineFound.png",lineFound);

    cv::Mat andResult;
    cv::bitwise_and(clickFound,lineFound,andResult);

    cv::Mat show;
    line(image,cv::Point(pLU.x+500,pLU.y),pRD,cv::Scalar(0,0,255),
         bias,8);
    //cv::imwrite("show.png",image);
    //cv::imwrite("find.png",andResult);
    pLU.y-=bias;pRD.y+=bias;

    cv::Mat Found(andResult,cv::Range(pLU.y,pRD.y)
                  ,cv::Range(0,cols));
    int index=0;int count=0;
    for(auto iter = Found.begin<uchar>();
    iter != Found.end<uchar>();++iter){
        if(*iter >= 200 && count >= 50){
            geometry_msgs::msg::Point32 msg;
            msg.x = index % cols;
            msg.y = pLU.y+index/cols;
            Publisher_->publish(msg);
            cv::Mat search = cvImage->image;
            line(search,cv::Point(msg.x,msg.y),cv::Point(msg.x+10,msg.y),
                 cv::Scalar(0,255,0),8);
            //save("find",search);
            break;
        }
        else if(count < 50){
            count++;
        }
        index++;
    }



}

void Terminal::save(std::string name, cv::Mat image) {
    std::string XorName = name;
    XorName += std::to_string(count);
    XorName += ".png";
    cv::imwrite(XorName, image);
    count++;

}
