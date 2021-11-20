cmake_minimum_required(VERSION 2.8.3)
project(imu_publish)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
)


catkin_package(
  INCLUDE_DIRS ros_inc
#  LIBRARIES imu_publish
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)


include_directories(
  ros_inc
  ${catkin_INCLUDE_DIRS}
)

add_executable(imu main.cpp ros_src/config.cpp ros_src/_protocol.cpp  ros_src/serial.cpp ros_src/tool.cpp config_tool/cJSON.c)
target_link_libraries(imu ${catkin_LIBRARIES})

#add_executable(receive src/receive.cpp)
#target_link_libraries(receive ${catkin_LIBRARIES})
