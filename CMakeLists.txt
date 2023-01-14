
cmake_minimum_required(VERSION 2.8)
project( Assignment1 )
# OpenCV 的 CMake 配置文件目录
set(OpenCV_DIR "$ENV{HOME}/.local/lib/cmake/opencv4")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( Assignment1 Assignment1.cpp )
target_link_libraries( Assignment1 ${OpenCV_LIBS} )