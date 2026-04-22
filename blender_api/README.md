# Sophia Blender Animation API

This repository contains an animated model of the Sophia head, as a
Blender file. A Robot Operating System (ROS) node to control the model
can be found in the `blender_api_msgs` repository. The ROS node is
automatically started when the blender file is loaded.

The rigControl python module contains scripts to drive the model, as
well as defining a public programming API.  The rigAPI module defines an
abstract base class for controlling the rig: the ROS node uses this API,
and rigControl implements it.

# Running

Pre-requisites: [Blender](https://www.blender.org/) must be installed.

This works well with the following versions:
* Version 2.71 on Ubuntu 14.04 (Trusty) from the irie ppa:
  `sudo apt-add-repository ppa:irie/blender`
* Version 2.78 available by default on Debian 9 stable (Stretch).

This does NOT work with:
* Version 2.69 (the default version) on Ubuntu 14.04 (Trusty)
* Version 2.75 or 2.79 on Ubuntu 14.04 (Trusty) from the Thomas Schiex
  ppa: `sudo apt-add-repository ppa:thomas-schiex/blender`
  See issue #76.

Start blender as follows:

```
blender -y Sophia.blend -P autostart.py
```

Sophia can be controlled via buttons in the blender GUI (note the panel
on the right).  A HOWTO guide for manipulating via ROS can be found in
the [Sophia cookbook](https://github.com/opencog/blender_api_msgs/blob/master/cookbook.md)

## Newer versions of Blender
The rig does not work with newer versionf of Blender. There have been
some efforts to port it, but these are incomplete.  See the
[progress report](README_PROGRESS.md) for status and hints.
***Wanted!*** **A Blender expert who can port over the old rig!**

# Design
The programming API is currently in draft stage, here:
[API_v1](docs/API_v1.md). What has actually been implemented does not
match the proposed API; neither is "authoritative", both probably need
revision.

![UML Diagram](docs/evaEmoDesign.png)

* The ROS node listens to and acts on ROS messages.  It uses the
  abstract base class `rigAPI` to communicate with blender.
* Animation messages are queued with the `CommandSource.py` module.
* The `CommandListener` listens to `CommandSource` messages; these
  are `'rigAPI` messages.
* The `command.py` module implements the `rigAPI`
* The `AnimationManager` keeps track of of Eva's internal state.
* The `Actuators` are responsible individual actions of Eva such as
  breathing, blinking and eye movement.

All animation sequences and 3D data are stored in the Blender file.

# Requirements #
The existing API needs to be changed, to allow control of the ramp-in and
ramp-out for animations. The API needs to expose:

* time of ramp start
* time interval until ramp-full
* strength of animation at ramp-full
* duration of animation
* time interval of ramp-out
* spline for above.

To implement the above, the following changes are needed:
* Change the API to include the above parameters, as a part of the animations.
* Change the ROS messages, to pass the above parameters.
* Update the blender rig to respect the API requests.
* Update the behavior trees to issue the new ROS message formats.

To avoid ROS compatibility issues, the new ROS messages should probably
be published on a different set of topics than the existing ones, so that
both the old and the new ROS message formats could be handled for a while.

# Copyright

Copyright (c) 2014,2015,2016,2017 Hanson Robotics

Copyright (c) 2014,2015 Mike Pan
