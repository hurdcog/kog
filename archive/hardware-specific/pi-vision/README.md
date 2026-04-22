pi_vision
=========

Pi Vision ported to ROS Noetic, together with various enhancements:
* Its been catkinized.
* Can track multiple faces.
* Attempts to perform localization in 3D, so that face positons are now
  published in 3D coordinates.

This is based on the original Pi Vision package, taken from
http://wiki.ros.org/pi_vision, which had been abandonded after groovy.
This package includes both http://wiki.ros.org/pi_face_tracker
and http://wiki.ros.org/pi_face_tracker_gui

The plain, unenhanced  port of `pi_vision` to Indigo is tagged with the
tag `orig-indigo-port`. The final version of the Indigo port is tagged
with `ros-indigo`. The `master` branch is current for Noetic.


Installation
============

```
apt-get install ros-noetic-cv-bridge ros-noetic-image-transport
```

The node must be built from git:

```
git clone https://github.com/opencog/pi_vision
cd catkin; catkin build
source devel/setup.bash
```

Run
===

To change camera settings, modify the `usb_cam.launch` file.

```
roslaunch ros2opencv usb_cam.launch
roslaunch pi_face_tracker face_tracker_usb_cam.launch
```

ROS Topics
==========

## Faces /face_locations
Publishes a list of human faces being tracked. Each is given an ID number,
and a 3D coordinate.  The coordinate frame used is the usual ROS
'engineering' frame: `x` is straight ahead, `y` is the the left, and `z`
is up.  Units are in meters.

## FaceEvent /face_event
Publishes face tracking events. Currently, the only events published are
`new_face` and `lost_face`. The first indicates a newly-acquired face to
track, the second, that a face is no longer visible.

TODO
====
1. Implement face recognition on a "new_face".
2. face_tracker.py has own copy of ROS2OpenCV class from ros2opencv package: 
merge the own copy back and fix the reference so it works again. (Python3 changed
the way other python modules are bound) 

