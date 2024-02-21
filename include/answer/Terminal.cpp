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
    cv::imshow("Get",image);
    //RCLCPP_INFO(this->get_logger(), "[x:%d,y:%d,ch:%d]", msg.height, msg.width, image.channels());
    //RCLCPP_INFO_STREAM(this->get_logger(), "Encoding:" << msg.encoding);
    //RCLCPP_INFO_STREAM(this->get_logger(), "Analysising...");
    cv::Mat grayImage;
    cv::cvtColor(image,grayImage,cv::COLOR_BGR2GRAY);


    cv::Mat binImage;
    cv::threshold(grayImage,binImage,100,255,cv::THRESH_BINARY);

    int rows = image.rows;
    int cols = image.cols;

    std::vector<cv::Vec4f> lines; // 存储检测到的直线

    //检测click和判定线
    HoughLinesP(binImage, lines, 1, CV_PI / 180, 300,200,100); // 应用霍夫直线检测算法
    //save("image",binImage);
    //Lines是图片lines是vector
    cv::Mat judgeLine = image.clone();
    cv::Mat click = image.clone();
    cv::Mat lineFound(image.rows,image.cols,CV_8UC1);

/*
    for (size_t i = 0; i < lines.size(); i++)
    {
        cv::Vec4f l = lines[i];
        cv::Scalar color;
        cv::Point p1(l[0], l[1]);
        cv::Point p2(l[2], l[3]);
        int distance = (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
        int yu = 160000;
        if(distance>yu)
            line(lineFound, p1, p2,255, 12,cv::LINE_AA);//, cv::LINE_AA

    }
//save("LineFound",lineFound);
//寻找直线的范围，以减小运算量
    cv::Point str;cv::Point end;
    str.x=str.y=10000;end.x=end.y=0;
    for(auto& iter : lines){
        if(iter[1]<str.y) str.y = iter[1];
        if(iter[3]<str.y) str.y = iter[3];

        if(iter[1]>end.y) end.y = iter[1];
        if(iter[3]>end.y) end.y = iter[3];
    }
str.y-=10;
end.y+=10;
 */

std::vector<int> str;
std::vector<int> end;
    for(int i = 0;i >= rows;i++){
        if(image.at<int>(0,i) == 255){
            str.push_back(i);
        }

    }
    for(int i = 0;i >= rows;i++){
        if(image.at<int>(cols+1,i) == 255){
            end.push_back(i);
        }
    }
    for(auto& iter : str)
        RCLCPP_INFO_STREAM(get_logger(),iter<<" ");

    std::vector<cv::Mat> Channel;
    cv::Mat blueChannel;
    cv::split(image,Channel);
    blueChannel = Channel[0];
    //cv::imwrite("Blue.png",blueChannel);
    //cv::imwrite("Green.png",Channel[1]);
    cv::threshold(Channel[2],Channel[2],150,255,cv::THRESH_BINARY);
    //cv::imwrite("Red.png",Channel[2]);
    cv::threshold(blueChannel,blueChannel,150,255,cv::THRESH_BINARY);
    //cv::imwrite("Blue1.png",blueChannel);
    cv::Mat clickFound;
    cv::bitwise_xor(Channel[2],blueChannel,clickFound);
    cv::imwrite("clickFound.png",clickFound);
    cv::imwrite("lineFound.png",lineFound);
    //save("lineFound",lineFound);
    //cv::Mat Result(image.rows,image.cols,CV_8UC3);
    cv::Mat xorResult;
    cv::bitwise_and(clickFound,lineFound,xorResult);
    //RCLCPP_INFO_STREAM(get_logger(),"X\n"<<xorResult);
    //save("XOR",xorResult);
    //rangeIn(xorResult,str,end);
    //RCLCPP_INFO_STREAM(get_logger(),"Y");
    //save("Result",xorResult);




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

void Terminal::rangeIn(cv::Mat image,cv::Point str,cv::Point end){
    //对指定的矩阵进行局部遍历
    int i = 1;int cont=0;
    cv::Mat mat(image,cv::Range(0,10),cv::Range(0,image.cols+1));
    for(auto iter = mat.begin<int>();iter != mat.end<int>();iter++){
        if(*iter >= 200){
            cont++;
            if(cont>=60){
                geometry_msgs::msg::Point32 msg;
                msg.x = i / image.rows;
                msg.y = i % image.rows;
                Publisher_->publish(msg);

                RCLCPP_INFO_STREAM(get_logger(),"Find!\n"<<mat);
                save("Find",mat);
            }

        }
        else{
            cont=0;
        }
        i++;
    }

}


/*
 int i=1;int cont=0;
    for(auto iter = xorResult.begin<int>();iter != xorResult.end<int>();++iter){
        if(*iter>=200)
        {
            cont++;
            if(cont >= 60){
                geometry_msgs::msg::Point32 msg;
                msg.x = i%image.rows;
                msg.y = i/image.rows;
                Publisher_->publish(msg);
                RCLCPP_INFO_STREAM(this->get_logger(), "Find!");
                save("Find",xorResult);
                break;
            }

        }
        else{
            cont = 0;
        }
        i++;
    }
 * */


/*
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
            line(lineFound, p1, p2,255, 2,4);//, cv::LINE_AA
        }
        else{
            color = cv::Scalar(0,0,255);//红
            line(click, p1, p2,color, 2,4);//, cv::LINE_AA
        }
*/

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