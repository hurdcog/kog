# Intro
I was interested in reviving the Eva portion of the opencog/docker repo, and to that end I created the "noetic" folder, in which
the existing "indigo" was updated to use ubuntu 20.04. I dealt with a lot of bit rot and succeeded in getting "eva-ros" to build
and run. To this end I integrated a new webcam implementation with face recognition within the ROS framework. There are issues
with the control of the Eva (Sophia) blender head from ROS node commands. It was not possible to exercise the python api.
The main problem is that trying to run the latest blender with the Sophia image fails to start up: the rendering is failing with 
multiple issues, and the python api doesn't start up.

+ The latest blender is installed by the Dockerfile and runs within the container.
+ The current Sophia.blend file is not rendered properly but the newer blender runs properly in the container.
+ The earlier (2.79b) blender version runs the Sophia.blend perfectly, BUT within a Docker container the rendering is not supported properly by changes to the X11 rendering. (you can run it by hand to see this, read on)

Here is an excerpt of an email thread between myself and reslav.hollos@gmail.com:

<Hi Mark,
>
>I've added "Principled BSDF" node materials and approximated the values to match the look, except the eye's displacement (if you zoom in really close) which I've not figured out yet.
>Shadows/lighting are not tested, I can continue on this in a few days, but hopefully this would be sufficient for you to start getting some visuals.
>
>The blend file is here https://github.com/Radivarig/blender_api/blob/update-materials/Eva_nodes.blend
>
>Best,
>Reslav
>
>
>On Wed, Mar 2, 2022 at 5:23 PM Mark Wigzell <markwigzell@gmail.com> wrote:
>Hi Reslav,
>Hey great! Thanks for your help:
>In order to see issue:
>
>git clone https://github.com/opencog/blender_api.git
>cd blender_api
>blender -y Sophia.blend -P autostart.py
>
>You can download different blender versions from https://download.blender.org/release/
>If you use 2.79b it seems to work best. If you use the next version, 2.80rc3 you'll see a lot of load errors.
>My actual work on Eva (Sophia.blend), is around getting the existing solution to build and run. You can see that here: https://github.com/opencog/docker/tree/master/noetic
>Try building ros-base, ros-blender and ros-eva containers. Finally run the ros-eva container, it comes up using "tmux". There are run errors in some of the nodes, the webcam image comes up, and the current blender (3.?) comes up with Sophia.blend. Of course it looks terrible because the materials haven't been converted.
>I attached an image of the 2.79b running. (If you click the "start animation" button on the right, she goes into action.)
>Apparently Eva should not have been renamed Sophia.blend, that was a mistake 7 years ago or whenever. I'll fix that.
>So running Eva in blender with the "blender_api" in the current working directory is essential to having the ROS node work properly. And having that work properly is essential to debugging the "perception" repo, which won't work without blender ROS
>There is an issue of python namespace when starting up Eva in blender.
>There is also one "cyclic dependency": Dependency cycle detected: deform depends on control through Child Of. control depends on deform through Locked Track.
>On Wednesday, March 2, 2022 at 12:01:22 AM UTC-8 reslav...@gmail.com wrote:
>Hi Mark,
>
>Is your current progress available somewhere? I can look into Blender materials.
>
>Best,
>Reslav

# Discussion on the opencog/googlegroups.com website
Please search the opencog discussion for "Eva Impasse". There is a video of the Sophia.blend head working with the 2.79b blender
(outside of docker). This is the link: [animations] (https://04735475913824401098.googlegroups.com/attach/19054d7814f89/simplescreenrecorder-2022-03-06_10.45.45.mp4?part=0.1&view=1&vt=ANaJVrHp7cB65GdhAIvHyJNuldjPyrEOXJSyx8ksj_Dwg2sxwaZsgqZ09iy8iXt32hsHLb6jaYIejMR00_3ofGsxHs0Bwyu0Ck-jjh9gTzuJHmle7nIBV6o)

# How to run original (working) Eva head
+ Download the 2.79b blender version from the url listed above
+ cd <path to blender>\2.79\python\bin
+ python.exe -m ensurepip
+ Now navigate to the blender_api folder as mentioned above and run it:
+ git clone https://github.com/opencog/blender_api.git
+ cd blender_api
+ blender -y Sophia.blend -P autostart.py

# Issue with the existing Sophia.blend file
+ The 2.79b blender is the last version of blender that will run it.
+ The blender rendering engine was replaced with an alternative in subsequent releases.
+ mods attempted by Reslav succeeded in allowing newer rendering to work, but the underlying python api was broken by the attempt

# Discussion with Nicap in the Blender chat website
This opencog exchange contains link to dscussion with Nicap, see below. It has supporting images that might be useful. You will need to get yourself an account to view.

<Hi Reslav, first let me thank you again for helping out! 
>I think your changes are fine to get us going BUT we need to do them in the right order, on the right version etc. (I think we got our wires crossed slightly because the repo commit I gave you was the untouched AS IS Eva Impasse commit. No fixes whatsoever are done.
>The first thing to do is get synchronized with each other about how to approach this fix.
>Let me outline the steps as I see it, keep in mind, we don't want any errors resulting from using wrong blender version. Also, we MUST start with blender 2.79b, do all the fixes and save it out. ONLY then open with 2.80. I'm attaching a video which I just created, it can be played with VLC. 
>Install blender 2.79b : For my 64bit ARCH I used: https://download.blender.org/release/Blender2.79/blender-2.79b-linux-glibc219-x86_64.tar.bz2 
>clone the blender_api from: https://github.com/opencog/blender_api.git
>Navigate to the installed blender and run: "cd <path to blender>\2.79\python\bin; ./python3.5m -m ensurepip" (this installs the setuptools needed to run the python commands)
>Navigate to the "blender_api" folder and run the correct blender "<path to blender>/blender -y Sophia.blend -P autostart.py". (you should see something like the video I attached)
>Close out and run again without the autostart. (I don't know how to stop the animation once started)
>Click on the drop down on the top menu bar which is currently displaying "Blender Render" and choose  "Cycles Render". 
>See this thread: https://blender.chat/channel/support/thread/7hx7CinRjKa9NzbrT (Its me chatting with Nicap. See the views he sent me. 
>Click bottom left view menu and choose User Preferences/Add Ons, and enable the "Materials: Materials Utils Specials 
>Follow along in my conversation with Nicap to convert the nodes having textures. Note the eyes don't convert. 
>Now please add in your eye materials. I have no idea what you do there! 
>If all is well then save your blend file. 
>Install blender 2.80 and try loading the blend file. (don't forget the "ensurepip" command above to install the setuptools
>Run the 2.80, I have my fingers crossed: If all went well it should load up your converted file and play it. (only if the animation pane on the right side comes up, and the animation can be started, is it a success)

Note that the "fingers crossed" were in vain because fixing the rendering didn't work with the python api: we are missing something
vital about how to convert those blender files, we need an expert!

The approach to solving this is still valid: fix the rendering issue with the Sophia.blend and 2.79b blender: Instead of "Blender Render" and choose  "Cycles Render" in the top bar of the blender UI. 
If you can't get your "blend file" image to load the python cleanly, obviously the conversion didn't work: we need to be able to run the python!!!!!!!!!!!!!
If your converted blend file renders nicely, AND you can run the gestures within the blender api we're in business: save it out and try successive versions of blender to get to the latest one. Then swap your blend file with the one in the "blender_api" repo and make sure docker is loading up the same blender version: This should all start up from within a docker container.


