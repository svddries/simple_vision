cmake_minimum_required(VERSION 2.8.3)
project(drone_vision)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(test_vision test/test_vision.cpp)
target_link_libraries(test_vision ${OpenCV_LIBRARIES})

