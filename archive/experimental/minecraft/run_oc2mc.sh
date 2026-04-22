#!/bin/bash

if [ -d catkin_ws/src ] ; then
    echo "Starting ROS Core"
else
    echo "Building Minecraft Bot Src..."
    ./build_minecraft_bot.sh
fi

roscore &
source catkin_ws/devel/setup.bash

echo "Run: tmuxinator start oc2mc"
