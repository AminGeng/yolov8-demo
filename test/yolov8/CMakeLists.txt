cmake_minimum_required(VERSION 3.5)
project(yolo_demo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_CURRENT_LIST_DIR})

# MNN begin
get_filename_component(GRANDPARENT_DIR ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
set(MNN_DIR ${GRANDPARENT_DIR}/MNN)
include_directories(${MNN_DIR}/include)
link_directories(${MNN_DIR}/lib)
# MNN end

# source files
set(SOURCE_FILES yolov8_demo.cpp)

# generate the shared library from the sources
add_library(yolov8_demo SHARED ${SOURCE_FILES})

# link the library with the MNN library
find_library(MNN_LIB MNN PATHS ${MNN_DIR}/lib NO_DEFAULT_PATH)
if(NOT MNN_LIB)
    message(FATAL_ERROR "MNN library not found")
endif()

# link the library with the MNN library
target_link_libraries(yolov8_demo ${MNN_LIB} pthread)