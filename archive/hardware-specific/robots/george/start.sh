#!/bin/bash
echo "Starting..."
# kill old session if exists
tmux kill-session -t george > /dev/null 2>&1
rfcomm release 0 > /dev/null 2>&1
rfcomm release 1 > /dev/null 2>&1
sleep 1;
#Bluetooth devices
tmux new-session -n 'BT Head'  -d -s george 'rfcomm connect 0 00:06:66:6C:A5:38 1;'
tmux new-window -n 'BT Body' 'rfcomm connect 1 30:14:10:10:06:51 1;'
sleep 8;
tmux new-window -n 'george' 'roslaunch robots_config robot.launch name:=george mini:=True; $SHELL'
tmux attach;
echo "Started"
