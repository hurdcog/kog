cmake_minimum_required(VERSION 2.8.3)
project(ros2opencv)
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs sensor_msgs cv_bridge image_transport)

catkin_python_setup()

catkin_package(
    DEPENDS roscpp rospy std_msgs sensor_msgs cv_bridge image_transport opencv2
    CATKIN_DEPENDS # TODO
    INCLUDE_DIRS # TODO include
    LIBRARIES # TODO
)
