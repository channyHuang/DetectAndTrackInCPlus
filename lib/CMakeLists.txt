cmake_minimum_required(VERSION 3.15)

project(HGDetectLib)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
	include_directories(${OpenCV_INCLUDE_DIRS})
endif()

find_package(Eigen3 REQUIRED)
if (Eigen3_FOUND)
    include_directories(${EIGEN3_INCLUDE_DIRS})
endif()

set(OUTER_DIR ${CMAKE_SOURCE_DIR}/../)
add_subdirectory(${OUTER_DIR}/Track Track)

include_directories(${PROJECT_NAME} 
include
${OUTER_DIR}
)

file(GLOB HEADERS ./*.h)
file(GLOB CPPS ./*.cpp)

set(PROJECT_SOURCES
    ${HEADERS}
    ${CPPS}
)

if (Lib) 
add_library(${PROJECT_NAME} 
#add_executable(${PROJECT_NAME} 
               ${PROJECT_SOURCES}
)
else()
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)
endif()

link_directories(${PROJECT_NAME}
)

target_link_libraries(${PROJECT_NAME} 
${OpenCV_LIBS} 
pthread
Track
)
