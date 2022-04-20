
cmake_minimum_required(VERSION 3.9)
set(PRO dataMergeOffline)
project(${PRO})

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(. SRC_LIST)
add_executable(${PRO} ${SRC_LIST})

find_package(PCL 1.8 REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

find_package(OpenCV 3.4.1 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

target_link_libraries (${PRO} ${PCL_LIBRARIES} ${OpenCV_LIBS})
