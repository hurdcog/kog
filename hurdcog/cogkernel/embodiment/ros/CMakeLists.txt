cmake_minimum_required(VERSION 3.0.2)
project(hurdcog_ros)

## Find catkin and required packages
find_package(catkin REQUIRED COMPONENTS
  rospy
  std_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  message_generation
)

## Generate messages
add_message_files(
  FILES
  CognitiveState.msg
  AttentionAllocation.msg
  CognitiveTask.msg
  TaskResult.msg
)

## Generate services
add_service_files(
  FILES
  CognitivePlan.srv
  SubmitTask.srv
  GetAttention.srv
)

## Generate added messages and services
generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
  nav_msgs
)

## Declare catkin package
catkin_package(
  CATKIN_DEPENDS
    rospy
    std_msgs
    geometry_msgs
    nav_msgs
    sensor_msgs
    message_runtime
)

## Install Python scripts
catkin_install_python(PROGRAMS
  cognitive_planner.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

## Install launch files
install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)
