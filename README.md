# System
- Ubuntu 16.04.7 LTS
- gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.12)
# MNN
- clone MNN Rep date: 2024/06/14
- compile command
    ```shell
    git clone https://github.com/alibaba/MNN.git
    cd MNN
    mkdir build && cd build
    cmake -DMNN_BUILD_OPENCV=ON -DMNN_IMGCODECS=ON -DMNN_BUILD_SHARED_LIBS=OFF ..
    make -j4
    ```
- copy lib and files by `sh` [copy_MNN.sh](copy_MNN.sh) after compiled

# Program
## yolov8_demo
in `test/yolov8` folder.

[yolov8_demo.h](test/yolov8/yolov8_demo.h), [yolov8_demo.cpp](test/yolov8/yolov8_demo.cpp): 
- original codes copy from https://github.com/wangzhaode/mnn-yolo/blob/main/cpp/yolov8_demo.cpp
- I convert model load and infer process into `class`
- [yolov8n.mnn](yolov8n.mnn) download url: https://github.com/wangzhaode/mnn-yolov8/releases/download/v1.0/yolov8n.mnn 

## multi-thread test program
in `test` folder.

[mult_thread_test.cpp](test/mult_thread_test.cpp)

# compile
```
cd test
mkdir build && cd build
cmake ..
make -j4
```

# **Issue**
run `./multi_thread_test 1` no problem. but `n>1` like `./multi_thread_test 12` error!
>This error does not always occur, but the likelihood increases with a higher number of threads. I have tested it on Ubuntu 16 (GCC 5.4), Ubuntu 18 (GCC 7.5), and Ubuntu 22.04 (WSL with GCC 11.4), and the issue persists across all versions.”
## Fix Method
1. using `MNN::CV` in multithreaded scenarios, locking is required

2. using `MNN::Module` api, multithreaded scenarios, reference:
    https://mnn-docs.readthedocs.io/en/latest/inference/module.html#id6