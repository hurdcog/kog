cmake_minimum_required(VERSION 2.8.3)
project(pi_face_tracker)
find_package(catkin REQUIRED COMPONENTS rospy roscpp ros2opencv std_msgs sensor_msgs geometry_msgs cv_bridge image_transport message_generation)

add_message_files(
  FILES
  FaceEvent.msg
  Face.msg
  Faces.msg
)

add_service_files(
  FILES
  KeyCommand.srv
  SetROI.srv
)

generate_messages(
   DEPENDENCIES geometry_msgs std_msgs sensor_msgs
)

catkin_package(
    DEPENDS rospy roscpp ros2opencv std_msgs sensor_msgs cv_bridge image_transport opencv2
    CATKIN_DEPENDS # TODO
    INCLUDE_DIRS # TODO include
    LIBRARIES # TODO
)
