
The catkin (ROS) package for running the Minecraft bot

## Installation

(assuming server, OpenCog, ROS, and SpockBot have been installed as detailed in the main directory)

Run the build_minecraft_bot.sh script in the root folder of this repo.

## Steps to start the bot
For each step follwoing the first one, open a separate terminal and then first
run
```
    source catkin_ws/devel/setup.bash
```

1. Start ros master server by running `roscore`

    roscore

2. Start actionsnode.py by running `rosrun minecraft_bot actionsnode.py`.
   It will start the `action_server` ROS node to publish actions message to
   Spock and provide ROS services for Opencog. Opencog can use the services to
   do action.

3. Start visnode.py by running `rosrun minecraft_bot visnode.py`. It will start
   the `visibility_node` ROS node to calculate what's visible for the bot, and
   publish ROS message for Opencog.

4. Start mapnode.py by running `rosrun minecraft_bot mapnode.py`. It will start
   the `minecraft_map_server` ROS node to receive primary blocks messages, save
   them and provide block information service for visnode.py.

5. Start test_mc_bot.py by running `rosrun minecraft_bot test_mc_bot.py`. It
   initializes the Spock bot and test custom plugins. It will start Spock and
   a ROS node `spock_controller`. This node will only be responsible for
   receiving Minecraft packets and sending raw environment data to the map node.

6. Spawn the bot by running `rosrun minecraft_bot opencog_initializer.py`. It
   will create the `OpenCog_Perception` ROS node. You should see the bot start
   to move randomly.

All source for our custom SpockBot plugins are in spockextras/plugins/helpers. ROS nodes are all in this directory.
