cmake_minimum_required(VERSION 3.5)
project(yolo_demo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# OpenCV
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
set(OpenCV_DIR ${PARENT_DIR}/opencv-4.8.0_static_gcc5.4.0/lib/cmake/opencv4)
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
# OpenCV end

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/yolov8)

# Include directories
include_directories(${CMAKE_CURRENT_LIST_DIR}/yolov8)
# target
add_executable(multi_thread_test ${CMAKE_CURRENT_LIST_DIR}/mult_thread_test.cpp)

target_link_libraries(multi_thread_test yolov8_demo ${OpenCV_LIBS})
