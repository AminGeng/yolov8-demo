#include <stdio.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "yolov8_demo.h"
int main_test(int thread_id)
{
    std::string s_id = std::to_string(thread_id);
    std::string image_path = "../../boy_and_girl.jpg";
    const char *model_path = "../../yolov8n.mnn";
    yolov8_demo y8_demo(model_path);
    auto original_image = cv::imread(image_path);
    for (int i = 0; i < 1000; i++)
    {
        std::vector<Bbox_struct> bboxs;
        y8_demo.infer(original_image.data, original_image.rows, original_image.cols, original_image.channels(), bboxs);
        // draw inference result
        auto image = original_image.clone();
        for (int i = 0; i < bboxs.size(); i++)
        {
            cv::rectangle(image, cv::Point(bboxs[i].x1, bboxs[i].y1), cv::Point(bboxs[i].x2, bboxs[i].y2), cv::Scalar(0, 255, 0), 2);
            cv::putText(image, std::to_string(bboxs[i].class_id), cv::Point(bboxs[i].x1, bboxs[i].y1), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        }
        cv::imwrite(s_id +"_" + std::to_string(i)+".jpg", image);
    }
    return 0;
}

int main(int argc, const char *argv[])
{
    int thread_num = std::stoi(argv[1]);
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_num; i++)
    {
        threads.push_back(std::thread(main_test, i));
    }
    for (int i = 0; i < thread_num; i++)
    {
        threads[i].join();
    }
    return 0;
}