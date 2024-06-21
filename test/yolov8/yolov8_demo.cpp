#include <stdio.h>
#include <mutex>
#include <iostream>
#include <MNN/ImageProcess.hpp>
#include <MNN/expr/Module.hpp>
#include <MNN/expr/Executor.hpp>
#include <MNN/expr/ExprCreator.hpp>
#include <MNN/expr/Executor.hpp>
#include <MNN/expr/ExecutorScope.hpp>
#include <cv/cv.hpp>
#include "yolov8_demo.h"
using namespace MNN;
using namespace MNN::Express;
using namespace MNN::CV;

class yolov8
{
private:
    int thread = 1;
    int precision = 0;
    int forwardType = MNN_FORWARD_CPU;
    ScheduleConfig sConfig;
    BackendConfig bConfig;
    std::shared_ptr<Module> Net;
    int img_size = 640;

public:
    yolov8(const char *model_path)
    {
        sConfig.type = static_cast<MNNForwardType>(forwardType);
        sConfig.numThread = 1;

        bConfig.precision = static_cast<BackendConfig::PrecisionMode>(precision);
        sConfig.backendConfig = &bConfig;
        std::mutex g_mtx;
        g_mtx.lock();
        Net = std::shared_ptr<Module>(Module::load(std::vector<std::string>{}, std::vector<std::string>{}, model_path));
        g_mtx.unlock();
    }

    void  infer(const VARP& original_image, std::vector<Bbox_struct> &bboxs)
    {
        // preprocess
        auto dims = original_image->getInfo()->dim;
        int img_h = dims[0];
        int img_w = dims[1];
        int len = img_h > img_w ? img_h : img_w;
        float scale = (float)len / img_size;
        int resize_h = round(img_h / scale);
        int resize_w = round(img_w / scale);
        MNN::Express::VARP input_img = MNN::CV::resize(original_image, MNN::CV::Size(resize_w, resize_h));
        std::vector<int> padvals{0, img_size - resize_h, 0, img_size - resize_w, 0, 0};
        MNN::Express::VARP pads = MNN::Express::_Const(static_cast<void *>(padvals.data()), {3, 2}, MNN::Express::NCHW, halide_type_of<int>());
        input_img = MNN::Express::_Pad(input_img, pads, MNN::Express::CONSTANT);
        input_img = MNN::CV::resize(input_img, MNN::CV::Size(img_size, img_size), 0, 0, MNN::CV::INTER_LINEAR, -1, {0., 0., 0.}, {1. / 255., 1. / 255., 1. / 255.});
        auto input = MNN::Express::_Unsqueeze(input_img, {0});
        input = MNN::Express::_Convert(input, MNN::Express::NC4HW4);

        // infer
        std::shared_ptr<Executor> executor(Executor::newExecutor(sConfig.type, bConfig, 1));
        ExecutorScope scope(executor);
        std::unique_ptr<Module> net(Module::clone(Net.get()));
        auto outputs = net->onForward({input});

        // postprocess
        auto output = _Convert(outputs[0], NCHW);
        output = _Squeeze(output);
        // output shape: [84, 8400]; 84 means: [cx, cy, w, h, prob * 80]
        auto cx = _Gather(output, _Scalar<int>(0));
        auto cy = _Gather(output, _Scalar<int>(1));
        auto w = _Gather(output, _Scalar<int>(2));
        auto h = _Gather(output, _Scalar<int>(3));
        std::vector<int> startvals{4, 0};
        auto start = _Const(static_cast<void *>(startvals.data()), {2}, NCHW, halide_type_of<int>());
        std::vector<int> sizevals{-1, -1};
        auto size = _Const(static_cast<void *>(sizevals.data()), {2}, NCHW, halide_type_of<int>());
        auto probs = _Slice(output, start, size);
        // [cx, cy, w, h] -> [y0, x0, y1, x1]
        auto x0 = cx - w * _Const(0.5);
        auto y0 = cy - h * _Const(0.5);
        auto x1 = cx + w * _Const(0.5);
        auto y1 = cy + h * _Const(0.5);
        auto boxes = _Stack({x0, y0, x1, y1}, 1);
        auto scores = _ReduceMax(probs, {0});
        auto ids = _ArgMax(probs, 0);
        auto result_ids = _Nms(boxes, scores, 100, 0.45, 0.25);
        auto result_ptr = result_ids->readMap<int>();
        auto box_ptr = boxes->readMap<float>();
        auto ids_ptr = ids->readMap<int>();
        auto score_ptr = scores->readMap<float>();
        for (int i = 0; i < 100; i++)
        {
            auto idx = result_ptr[i];
            if (idx < 0)
                break;
            auto x0 = box_ptr[idx * 4 + 0] * scale;
            auto y0 = box_ptr[idx * 4 + 1] * scale;
            auto x1 = box_ptr[idx * 4 + 2] * scale;
            auto y1 = box_ptr[idx * 4 + 3] * scale;
            auto class_idx = ids_ptr[idx];
            auto score = score_ptr[idx];
            printf("### box: {%f, %f, %f, %f}, class_idx: %d, score: %f\n", x0, y0, x1, y1, class_idx, score);
            bboxs.push_back({x0, y0, x1, y1, score, class_idx});
        }
        net.reset();
    }
};

yolov8_demo::yolov8_demo(const char *model_path){
    y8 = std::make_shared<yolov8>(model_path);
}

void yolov8_demo::infer(unsigned char *image_data, int ih, int iw, int ic, std::vector<Bbox_struct> &bboxs){
    VARP img_bgr = _Const(image_data, {ih, iw, 3}, NHWC, halide_type_of<u_int8_t>());
    std::static_pointer_cast<yolov8>(y8)->infer(img_bgr, bboxs);
}