cmake_minimum_required(VERSION 3.5)

set(ProjectName DetectAndTrack)
project(${ProjectName} VERSION 0.1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# OpenCV
find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# !OpenCV

# Eigen
set(EIGEN_VERSION 3.3)
find_package(Eigen3 ${EIGEN_VERSION} QUIET)
if(NOT EIGEN3_FOUND)
  set(BUILD_TESTING OFF CACHE INTERNAL "")
  FetchContent_Declare(eigen
    GIT_REPOSITORY  https://gitlab.com/libeigen/eigen.git
    GIT_TAG         ${EIGEN_VERSION}
    GIT_SHALLOW     ON)
  FetchContent_MakeAvailable(eigen)
  unset(BUILD_TESTING CACHE)
endif()
# !Eigen

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -MMD -Wall -Wextra -Winit-self")

# Check C++17 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  add_definitions(-DCOMPILEDWITHC17)
  message(STATUS "Using flag -std=c++17.")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

file(GLOB DetectFiles ./Detect/*)
file(GLOB TrackFiles ./Track/*)


set(PROJECT_SOURCES
    main.cpp
    json.hpp
    ${DetectFiles}
    ${TrackFiles}
)

add_executable(${ProjectName} 
               ${PROJECT_SOURCES}
)

include_directories(${ProjectName}
    ${CMAKE_CURRENT_SOURCE_DIR}/Track
    )

target_link_libraries(${ProjectName} 
                      ${OpenCV_LIBS}
                      Threads::Threads
                      Eigen3::Eigen)
