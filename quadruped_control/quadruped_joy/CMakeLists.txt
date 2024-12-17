cmake_minimum_required(VERSION 3.0.2)
project(quadruped_joy)

set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "-std=c++14")
set(CMAKE_CXX_STANDARD 14)


find_package(catkin REQUIRED COMPONENTS
  quadruped_msgs
  roscpp
  rospy
  std_msgs
  sensor_msgs
)

catkin_package( #MUST add this to build the executable file in /lib for rosrun!!!!!!
        #  INCLUDE_DIRS include
        #  LIBRARIES rc_sim_oub
        #  CATKIN_DEPENDS roscpp rospy std_msgs
        #  DEPENDS system_lib
)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(quadruped_joy_node
  src/joy_to_cmd.cpp
  src/joy_to_cmd.h
  )

target_link_libraries(quadruped_joy_node
        ${catkin_LIBRARIES}
        )
