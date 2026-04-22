# opencog-to-minecraft

A ROS-based module linking OpenCog and the Minecraft world. This code serves as
a starting point for integrating the two environments. The code is in a 'heavy
experimentation' phase, and will therefore change drastically over the next
couple of months. The steps to set up and run the module are currently rather
complicated, but this will change as the code stabilizes.

## Prerequisite##

#### Ubuntu http://www.ubuntu.com

It's recommended to set up environment under [Ubuntu Trusty](http://releases.ubuntu.com/14.04/).

You can also use the OpenCog docker containers. Follow instruction
[here](https://github.com/opencog/docker/blob/master/opencog/README.md)
The containers have Ubuntu Trusty, OpenCog development dependencies and some
standard tools pre-installed.

#### ROS http://www.ros.org/

Now(20150823) ROS indigo distro is OK and recommended for Minecraft embodiment. Not sure if other distro will work or not.

Install instruction of ROS indigo is [here](http://wiki.ros.org/indigo/Installation/Ubuntu).

After installing ROS you have to create a workspace ([tutorial](http://wiki.ros.org/catkin/Tutorials/create_a_workspace)) and install the minecraft_bot package by the instruction in [official tutorial](http://wiki.ros.org/catkin/Tutorials/CreatingPackage) and [minecraft_bot/README](https://github.com/opencog/opencog-to-minecraft/tree/master/minecraft_bot).

#### Octomap http://wiki.ros.org/octomap

In ubuntu you can directly install Octomap by:

`sudo apt-get install ros-indigo-octomap

(Assume you have setuped the sources.list when you install ROS, or you will fail to find the package.)

If you are not using ROS indigo distro, just change the `ros-indigo-octomap` to `ros-your_distro-octomap`

#### Opencog https://github.com/opencog/opencog

Just follow the README to install this.

#### Minecraft server(official) https://minecraft.net/download

Install instuction of Minecraft server is [here](http://minecraft.gamepedia.com/Tutorials/Setting_up_a_server)

You can also choose other Minecraft server manager you prefer.

#### Spock https://github.com/SpockBotMC/SpockBot

A python API to connect with Minecraft server. Install by running
`sudo python setup.py install` instead of `python3 setup.py install `


## Steps to start the bot

1. add the following to PYTHONPATH:  
   `/usr/local/share/opencog/python`  
   `/your_opencog/build/opencog/cython/`  
   `/your_opencog-to-minecraft/`  

   > If you prefer Pycharm to do Python development, follow the instructions in _**PYCHARM.md**_.  
   > Return back and continue from the next step.

2. setup the ROS environment:
   ```
        roscore &
        source /where_you_create_catkin_ws/devel/setup.bash
   ```

3. Start Minecraft Server
  1. Edit the server.properties file, of your Minecraft server to have
     `online-mode=false` and `difficulty=0`
  2. Start Minecraft Server.
  3. From the server console run `/op your_username`

4. To start all the nodes and spawn the bot, run
   ```
        roscd minecraft_bot/                        
        roslaunch minecraft_bot default.launch      
   ```                                
   You should see the bot appeared in your Minecraft client. Should that not work for any reason, follow instructions in [minecraft_bot](minecraft_bot/README.md).

5. Start Minecraft client
  1. start Multiplayer mode
  2. direct connect to localhost server
  3. change to creative mode by running `/gamemode 1`
  4. run `/tp your_username Bot` to teleport to where the Bot is, or
     `tp Bot your_username` to teleport the Bot to where you are.

6. Put a "Wood" block in front of the bot. You should see the bot stops and
   walks toward the block. The bot is attracted by the target wood block. Then
   the bot leaves the block and keeps going since as time going the
   attention value of block decreases. For now (20150822) that's all behaviors
   of the bot.

##Building Documentation##

The python code for the bot is documented inline using doxygen.  You can build a folder containing the HTML documentation by running doxygen from there.  The commands to do this are below.  To install doxygen on an ubuntu based OS:

    `sudo apt-get install doxygen`

Once doxygen itself is installed you build the docs by doing:

```
    cd doc
    doxygen Doxyfile
```

After the docs are built you can view them by loading the main index page into your browser with a command similar to:

    `firefox html/index.html`

## Step for stopping the bot
To stop all the nodes run `rosnode kill -a` or `rosnode kill name_of_node` for
each ROS node started.

##TODO##

* Document all the code.

* Add more actions in actions ROS node and Opencog: mining, placing block, inventing items and more and more and more.

* Add more percetions in middle ROS node and Opencog: Entity, items, chat message and more and more and more.

* More visibility: For testing now we confine the bot's FOV so it only can get what it sees in near distance(distance < 10). It's great if we can make it see further and more accurate.

* Use the existing [planner](https://github.com/opencog/opencog/blob/master/opencog/embodiment/Control/OperationalAvatarController/OCPlanner.h) and [pathfinding](https://github.com/opencog/opencog/blob/master/opencog/spatial/3DSpaceMap/Pathfinder3D.cc) module in the action generator. Cython binding seems a possible way.

* Use the idmap in minecraft_bot/src/embodiment-testing/ to replace the block id in perception manager. So we can have a useful atom name.

* Pass every message to Opencog with the ROS/Minecraft timestamp to record the timestamp in atomspace/TimeServer correctly.

* add a void block handle in SpaceMap to distinguish the "unknown" block and the "air" block. (For example, if we only use Handle::UNDEFINED to express all non-void block, what does it mean when we get a undefined handle from spaceMap::get_block(position)? Does it mean (1) we've known there's no block in this position? or (2) we haven't record block in this position and we don't know if there's a block in the embodied environment?) To distinguish it, I guess it's better to use different block handle to distinguish void and unknown block.

* write a script for starting all of the things(ROS nodes/Spock/Opencog/Minecraft server) in differenct terminal(using tmux/screen)

* There are more subtle TODOs in the codes...We should move them to the github issues.
