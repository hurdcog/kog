cmake_minimum_required(VERSION 2.8.3)
project(pi_face_tracker_gui)
find_package(catkin REQUIRED COMPONENTS rospy roscpp pi_face_tracker)

catkin_package(
    DEPENDS rospy roscpp pi_face_tracker
    CATKIN_DEPENDS # TODO
    INCLUDE_DIRS # TODO include
    LIBRARIES # TODO
)
