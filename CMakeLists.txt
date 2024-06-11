cmake_minimum_required(VERSION 3.5)

project(Yolov8CPPInference VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


# OpenCV
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# !OpenCV

set(PROJECT_SOURCES
    main.cpp
    
    inference.h
    inference.cpp
    detectAndTrack.h
    detectAndTrack.cpp
    stopthread.h
)

add_executable(Yolov8CPPInference ${PROJECT_SOURCES})
target_link_libraries(Yolov8CPPInference 
                      ${OpenCV_LIBS}
                      Threads::Threads)
