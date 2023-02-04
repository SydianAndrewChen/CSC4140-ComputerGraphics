
cmake_minimum_required(VERSION 2.8)
project( Assignment2 )
# OpenCV 的 CMake 配置文件目录
set(OpenCV_DIR "$ENV{HOME}/.local/lib/cmake/opencv4")
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable( Assignment2 main.cpp rasterizer.cpp Triangle.cpp )
target_link_libraries( Assignment2 ${OpenCV_LIBS} )
target_link_libraries (Assignment2 Eigen3::Eigen)