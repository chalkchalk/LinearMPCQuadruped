cmake_minimum_required(VERSION 3.10.1)
project(quadruped_control)
set(CMAKE_BUILD_TYPE "Release")
# set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "-std=c++14")
# set(CMAKE_CXX_FLAGS "-std=c++14 -DEIGEN_USE_MKL_ALL")
set(CMAKE_CXX_STANDARD 14)



# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

#check environmental variables
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(catkin REQUIRED COMPONENTS
        roscpp
        rospy
        roslib
        std_msgs
        geometry_msgs
        nav_msgs
        unitree_legged_msgs
        quadruped_msgs
        tf
        )

find_package(Eigen3)
include_directories(
        ${EIGEN3_INCLUDE_DIR}
)

catkin_package(
        #  INCLUDE_DIRS include
        #  LIBRARIES rc_sim_oub
        #  CATKIN_DEPENDS roscpp rospy std_msgs
        #  DEPENDS system_lib
)

include_directories(
        ${catkin_INCLUDE_DIRS}
)

# to find osqp.h
include_directories(/usr/local/include/osqp)

find_package(OsqpEigen REQUIRED)

# add unitree hardware library, these flags must be correctly set
include_directories(
        $ENV{UNITREE_LEGGED_SDK_PATH}/include
        src/grf_controller
        src/utils
        src/utils/spdlog
        src/legKinematics
        src/leg_controller
        src/perception
        src/dog
        src/instance
        src
)

link_directories($ENV{UNITREE_LEGGED_SDK_PATH}/lib)



# if($ENV{UNITREE_PLATFORM} STREQUAL "arm64")
        # set(EXTRA_LIBS -pthread libunitree_legged_sdk_arm64.so lcm)
# else()
#         set(EXTRA_LIBS -pthread libunitree_legged_sdk_amd64.so lcm)
# endif()


add_library(grf_lib
        src/grf_controller/base_controller.h
        src/grf_controller/qp_controller.cpp
        src/grf_controller/mpc_controller.cpp
        # src/grf_controller/mpc_compact_controller.cpp
)

add_library(leg_ctrl_lib
        src/leg_controller/swing_leg_controller.cpp
)

add_library(gait_lib
        src/gait/base_gait.h
        src/gait/walk_gait.cpp
)

add_library(perception_lib
        src/perception/StateEstimate.cpp
)

add_library(utils_lib
        src/dog/dog_state.cpp
        src/dog/dog_params.h
        src/utils/Utils.cpp
        src/utils/PIDController.cpp
        src/utils/FrictionCompensation.cpp
        src/utils/RobotLog.cpp
        
        
)


add_library(kinematic_lib
        src/legKinematics/leg_kinematics.cpp
)

target_link_libraries(utils_lib 
                ${catkin_LIBRARIES}
                kinematic_lib
                ${EXTRA_LIBS})



add_library(dog_lib
        ${catkin_LIBRARIES}
        src/dog/dog_params.h
        src/dog/dog_control.cpp
        src/dog/dog_state.cpp
        src/utils/InfoToROS.cpp
)


target_link_libraries(dog_lib
        grf_lib
        leg_ctrl_lib
        gait_lib
        perception_lib
        utils_lib
        kinematic_lib
        ${catkin_LIBRARIES} 
        OsqpEigen::OsqpEigen
        )


# add_library(real_dog_lib
#         src/instance/real_dog.cpp
#         )

add_library(gazebo_dog_lib
        src/instance/gazebo_dog.cpp
        )


# target_link_libraries(real_dog_lib dog_lib ${EXTRA_LIBS})
target_link_libraries(gazebo_dog_lib dog_lib)


# add_executable(torque_shutdown src/utils/TorqueShutdown.cpp)
# target_link_libraries(torque_shutdown
#         ${catkin_LIBRARIES}
#         real_dog_lib
#         )

# add_executable(test_pd_ctrl_node src/test/test_pd_ctrl.cpp)
# target_link_libraries(test_pd_ctrl_node
#         ${catkin_LIBRARIES}
#         real_dog_lib
#         )


# add_executable(realdog_node src/instance/real_dog_node.cpp)
# target_link_libraries(realdog_node
#         ${catkin_LIBRARIES}
#         real_dog_lib
#         )

add_executable(gazebodog_node src/instance/gazebo_dog_node.cpp)
target_link_libraries(gazebodog_node
        ${catkin_LIBRARIES}
        gazebo_dog_lib
        )



add_executable(mpc_debug src/test/mpc_debug.cpp)
target_link_libraries(mpc_debug
        ${catkin_LIBRARIES}
        gazebo_dog_lib
        )




add_executable(spdlog_test src/test/spdlog_test.cpp)
target_link_libraries(spdlog_test
        utils_lib
        ${catkin_LIBRARIES}
        )

        