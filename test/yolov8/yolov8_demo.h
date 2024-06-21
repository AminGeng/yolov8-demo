#pragma once

struct Bbox_struct {
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    int class_id;
};

class yolov8_demo {
private:
    std::shared_ptr<void> y8;
public:
    yolov8_demo(const char *model_path);

    void infer(unsigned char *image_data, int ih, int iw, int ic, std::vector<Bbox_struct> &bboxs);
};