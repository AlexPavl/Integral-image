cmake_minimum_required(VERSION 3.1)
project(Integral_image_tests)

# Setup testing
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Threads REQUIRED)

add_definitions(-DTESTING)

add_executable(runTests
    main.cpp
    ./../integral_image.cpp)

target_link_libraries(runTests ${OpenCV_LIBS} GTest::gtest GTest::gtest_main Threads::Threads)
add_test(runTests main.cpp)