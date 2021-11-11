cmake_minimum_required(VERSION 3.1)

project(integral_image VERSION 1.0)

find_package( OpenCV REQUIRED )
MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")
include_directories("${OpenCV_INCLUDE_DIRS}")
find_package(Threads REQUIRED)
# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(integral_image integral_image.cpp)
target_link_libraries(integral_image ${OpenCV_LIBS} Threads::Threads)

