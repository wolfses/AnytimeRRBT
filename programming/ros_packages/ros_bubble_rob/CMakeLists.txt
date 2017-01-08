cmake_minimum_required(VERSION 2.8.3)
project(ros_bubble_rob)

find_package(catkin REQUIRED)
find_package(catkin REQUIRED COMPONENTS std_msgs sensor_msgs image_transport vrep_common)

include_directories(${catkin_INCLUDE_DIRS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_executable(rosBubbleRob src/rosBubbleRob.cpp) 
target_link_libraries(rosBubbleRob ${catkin_LIBRARIES})
add_dependencies(rosBubbleRob vrep_common_generate_messages_cpp)