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

while debugging, i find that `line 55` in [yolov8_demo.cpp](test/yolov8/yolov8_demo.cpp) is one of the causes of the error. 
```cpp
input_img = MNN::CV::resize(input_img, MNN::CV::Size(img_size, img_size), 0, 0, MNN::CV::INTER_LINEAR, -1, {0., 0., 0.}, {1. / 255., 1. / 255., 1. / 255.});
```

error like
```
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
Segmentation fault (core dumped)
```

```
root@a2a9ec0ed379:~/Amin/libs/MNN_libs/yolov8-demo/test/build# ./multi_thread_test 12
The device support i8sdot:0, support fp16:0, support i8mm: 0
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
### box: {31.553406, 37.296143, 299.393494, 339.611145}, class_idx: 0, score: 0.423997
*** Error in `./multi_thread_test': double free or corruption (fasttop): 0x00007f34f0c38f30 ***
======= Backtrace: =========
/lib/x86_64-linux-gnu/libc.so.6(+0x777f5)[0x7f35135867f5]
/lib/x86_64-linux-gnu/libc.so.6(+0x8038a)[0x7f351358f38a]
/lib/x86_64-linux-gnu/libc.so.6(cfree+0x4c)[0x7f351359358c]
/root/Amin/libs/MNN_libs/yolov8-demo/test/build/yolov8/libyolov8_demo.so(_ZN3MNN20EagerBufferAllocator15getFromFreeListEPSt8multimapImNS_9SharedPtrINS0_4NodeEEESt4lessImESaISt4pairIKmS4_EEEmbm+0x3ea)[0xSegmentation fault (core dumped)
```