#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" face_tracker.py - Version 1.00 2022-02-15

    Track a face using the OpenCV Haar detector to initially locate
    the face, then track the face features over subsequent frames.

    This is a complete re-write of the original face tracker. The original
    was written in Python2 for ROS indigo and implmmented manual face tracking.
    The newer opencv2 that is available via the ros-noetic-cv-bridge is used in
    combination with the dlib library to do face tracking.
    The basis of this was taken from:
    https://github.com/shantnu/PyEng/blob/master/Image_Video/face_detect.py
    https://github.com/gdiepen/face-recognition.git

    Created for opencog.pi_vision project by markwigzell@gmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.5

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details at:

    http://www.gnu.org/licenses/gpl.html

"""

import roslib
roslib.load_manifest('pi_face_tracker')
import rospy
import cv2
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
from math import *
from pi_face_tracker.srv import *
from pi_face_tracker.msg import Faces
from pi_face_tracker.msg import Face
from pi_face_tracker.msg import FaceEvent
from geometry_msgs.msg import Point
import numpy as np
import time
import dlib
import threading

""" I stole this (see ../../ros2opencv).
    The primary reason was that my IDE (and ROS) is not able to see it.
    I believe something happened to the way that the __init etc. python grunge
    is interpreted by Python 3. 
    My intent is to merge this back to its starting point, but I want to see
    what else needs to be salvaged from the orignal. e.g. I don't have the
    depth_image or display_markers yet. 
    markwigzell@gmail.com
"""
class ROS2OpenCV:
    def __init__(self, node_name):
        rospy.init_node(node_name)
        rospy.on_shutdown(self.cleanup)

        self.node_name = node_name
        self.input_rgb_image = "input_rgb_image"
        self.input_depth_image = "input_depth_image"
        self.output_image = "output_image"
        self.show_text = rospy.get_param("~show_text", True)
        self.show_features = rospy.get_param("~show_features", True)
        self.image = None
        self.image_size = None
        self.depth_image = None
        self.grey = None
        self.selected_point = None
        self.selection = None
        self.drag_start = None
        self.keystroke = None
        self.key_command = None
        self.detect_box = None
        self.track_box = None
        self.display_box = None
        self.keep_marker_history = False
        self.night_mode = False
        self.auto_face_tracking = True
        self.cps = 0  # Cycles per second = number of processing loops per second.
        self.cps_values = list()
        self.cps_n_values = 20
        self.flip_image = False
        """ Create the display window """
        self.cv_window_name = self.node_name
        cv2.namedWindow(self.cv_window_name, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(self.cv_window_name, 640, 480)

        """ Create the cv_bridge object """
        self.bridge = CvBridge()

        """ Set a call back on mouse clicks on the image window """
        #cv2.SetMouseCallback(self.node_name, self.on_mouse_click, None)

        """ A publisher to output the display image back to a ROS topic """
        self.output_image_pub = rospy.Publisher(self.output_image, Image)

        """ Subscribe to the raw camera image topic and set the image processing callback """
        self.image_sub = rospy.Subscriber(self.input_rgb_image, Image, self.image_callback, queue_size=1)
        #self.depth_sub = rospy.Subscriber(self.input_depth_image, Image, self.depth_callback, queue_size=1)

        rospy.loginfo("Starting " + self.node_name)

    def image_callback(self, data):
        """ Time this loop to get cycles per second """
        self.start = rospy.Time.now()

        """ Convert the raw image to OpenCV format using the convert_image() helper function """
        cv2_image = self.convert_image(data)

        """ Some webcams invert the image """
        if self.flip_image:
            cv2.flip(cv2_image, 0)

        """ Create a few images we will use for display """
        if self.image is None:
            self.image_size = cv2_image.shape # needed to calculate cps text position :-(
            self.image = np.zeros(self.image_size, np.uint8)

        """ Copy the current frame to the global image in case we need it elsewhere"""

        """ Process the image to detect and track objects or features """
        processed_image = self.process_image(cv2_image)

        """ If the result is a greyscale image, convert to 3-channel for display purposes """
        """ Display the user-selection rectangle or point."""
        """ Merge the processed image and the marker image """
        """ Handle keyboard events """
        self.keystroke = cv2.waitKey(5) # must call this for call to cv2.imshow() below to succeed!

        cv2_image = self.calc_cps(processed_image)

        """ Now display the image."""
        cv2.imshow(self.node_name, cv2_image)

        """ Publish the display image back to ROS """
        try:
            """ Convertion for cv2 is needed """
            #cv2_image = numpy.asarray(self.display_image[:, :])
            self.output_image_pub.publish(self.bridge.cv2_to_imgmsg(cv2_image, "bgr8"))
        except CvBridgeError as e:
            print(e)

        """ Process any keyboard commands or command sent via the key_command service """
        if self.key_command:
            self.keystroke = ord(self.key_command)
            self.key_command = None

        """ Disable key commands for multiple face tracking """

    def convert_image(self, ros_image):
        try:
            """ Convert to cv2 image """
            cv2_image = self.bridge.imgmsg_to_cv2(ros_image, "bgr8")
            return cv2_image
        except CvBridgeError as e:
            print(e)

    def process_image(self, cv_image):
        """ abstract call not implemented: was converting image to grey scale and returning it"""
        return 0

    # calculate cycles per sec and display with resolution on image
    def calc_cps(self, image):
        duration = rospy.Time.now() - self.start
        duration = duration.to_sec()
        fps = int(1.0 / duration)
        self.cps_values.append(fps)
        if len(self.cps_values) > self.cps_n_values:
            self.cps_values.pop(0)
        self.cps = int(sum(self.cps_values) / len(self.cps_values))

        if self.show_text:
            fontscale = 1.0
            # (B, G, R)
            color = (256, 256, 0)
            # select font
            fontface = cv2.FONT_HERSHEY_COMPLEX_SMALL

            """ Print cycles per second (CPS) and resolution (RES) at top of the image """
            height = self.image_size[1]
            width = self.image_size[0]
            if height >= 640:
                vstart = 25
                voffset = int(50 + width / 120.)
            elif height == 320:
                vstart = 15
                voffset = int(35 + width / 120.)
            else:
                vstart = 10
                voffset = int(20 + width / 120.)
            cv2.putText(image, "CPS: " + str(self.cps), (10, vstart), fontface, fontscale, color)
            cv2.putText(image, "RES: " + str(height) + "X" + str(width), (10, voffset), fontface, fontscale, color)
        return image

    def cleanup(self):
        print("Shutting down vision node.")
        cv2.destroyAllWindows()

''' Class that trackes a single face. Both 2D and 3D tracking is
    perforrmed.  The 2D tracking is done in camera pixel coords, the
    3D position information is guesstimated from that. NOTE: this is a 
    rewrite of the original class. Its main purpose now is to support 
    the 3D point for publishing the FACE topic. The original manual HAAR tracking is no 
    longer implemented, instead CV2 now takes care of it (see use of CascadeClassifier in FaceTracker).
    Currently the "name" attribute is only used for display purposes.
    markwigzell@gmail.com
'''
class FaceBox():
    # pt1 and pt2 are the top left and bottom right corner points,
    # bounding the location of the face.
    def __init__(self, id, pt1, pt2, name):
        # unique id for this session
        self.face_id = id
        self.name = name

        # Coeff of how much area of the detected box should overlap
        # in order to make same judgement. FFS: we need this? I think not since we're not tracking
        #self.min_area = rospy.get_param('~face_min_area', 0.3)

        #currently reported with fixed value of 1.0 but maybe we can reflect the tracker quality here?
        self.attention = 1.0

        # Camera settings:
        # FOV == Field of View (full angle); 0.625 radians == 36 degrees.
        # My webcam is 2 x 0.3588 == 0.7175 radians == 2 x 20.5 == 41 degrees.
        self.camera_fov_x = rospy.get_param('~camera_fov_x', 0.625)
        # Camera name as format
        self.camera_name = rospy.get_param("~camera_name", 'camera')
        self.camera_width = rospy.get_param(self.camera_name + '/width', 640)
        self.camera_height = rospy.get_param(self.camera_name + '/height', 480)

        # Faces status: new, ok, deleted; FFS: needed?
        self.status = 'new'

        # bounds
        self.pt1 = pt1  # (x1,y1)
        self.pt2 = pt2  # (x2,y2)
        self.track_box = None
        # size of the face to meassure realative distance. Face width is enough
        self.bounding_size = pt2[0] - pt1[0]

        # 3D point
        # Simple exponential decay filter to smooth the 3D location.
        # The goal is to limit the jumpiness of the reported postion.
        # This does introduce some lag, but it shouldn't be more than
        # about 1-2 frames. A much much stronger filer is used for the
        # x-location (distance from camera), as that one is much noisier.
        #
        # TODO To get fancy, this could be replaced by a Kalman filter.
        # Or maybe just an alpha-beta filter would be best.
        #
        # It might be tempting to perform smoothing on the 2D position,
        # but since the tracked object really is slow-moving in 3D, we
        # do the smoothing on the 3D coordinates. In particular, if
        # Kalman was used, it would not be correct to apply it to the 2D
        # coords.
        self.yz_smooth_factor = 0.64
        self.x_smooth_factor = 0.95
        self.loc_3d = Point()

    def __repr__(self):
        return "<Face no.%d @ %s - %s - %s>" % (
            self.face_id,
            self.face_box(),
            self.attention,
            self.status
        )

    def get_box(self):
        return [self.pt1,self.pt2]

    def face_box(self):
        face_width = self.pt2[0] - max(self.pt1[0],0)
        face_height = self.pt2[1] - max(self.pt1[1],0)
        face_box = (max(self.pt1[0],0), max(self.pt1[1],0), face_width, face_height)
        return face_box

    def update_bounding_box(self):
        self.bounding_size = self.pt2[1] - self.pt1[1]

    def get_3d_point(self):
        # TODO will need to be updated:
        # Current camera calibration matrix should be passed.
        # Current camera pose needed (offset, and angle)
        # For now we assume its 36 degrees FOV with the face height of 20 cm
        # Standard 640x480 image used (taken from config)
        # Approx horizontal FOV of camera used (taken from config)
        p = Point()
        # same FOV for both, so calculate the relative distance of one pixel
        dp = 0.22 / float(self.bounding_size) # It should be same in both axis
        w = self.camera_width/2
        h = self.camera_height/2
        # Y is to the left in camera image, Z is to top
        p.x = dp *  (h / tan(self.camera_fov_x/2.0))
        p.y = dp * (w-(self.pt2[0]+self.pt1[0])/2)
        p.z = dp * (h-(self.pt2[1]+self.pt1[1])/2)
        return p

    # Smooth out the 3D location of the face, by using an
    # simple exponential filter.
    def filter_3d_point(self) :
        p = self.get_3d_point()

        # First time ever, initialize to a reasonable value.
        if self.loc_3d.x == 0:
            self.loc_3d = p

        pha = self.yz_smooth_factor
        bet = 1.0 - pha
        p.y = pha * self.loc_3d.y + bet * p.y
        p.z = pha * self.loc_3d.z + bet * p.z

        # x (distance from camera) gets a much stronger filter,
        # since its much noisier.
        pha = self.x_smooth_factor
        bet = 1.0 - pha
        p.x = pha * self.loc_3d.x + bet * p.x

        self.loc_3d = p

    def update_box(self,box):
        self.track_box = box
        if not box is None:
            x,y,w,h = box
            self.pt1 = (x,y)
            self.pt2 = (x+w, y+h)
            self.attention = 1.0
            self.update_bounding_box()
            self.filter_3d_point()

    # Get a smoothed, exponentially filtered version of the 3d point.
    def get_filtered_3d_point(self):
        return self.loc_3d

""" This replaces the original PatchTracker and the FacesRegistry classes.
    Basically used them where possible, so some of the original functions are honoured. 
"""
class FaceTracker(ROS2OpenCV):
    TOPIC_EVENT = "face_event"
    TOPIC_FACES = "face_locations"
    EVENT_NEW_FACE = "new_face"
    EVENT_LOST_FACE = "lost_face"

    def __init__(self, node_name):
        ROS2OpenCV.__init__(self, node_name)

        self.node_name = node_name

        self.auto_face_tracking = rospy.get_param("~auto_face_tracking", True)
        self.use_haar_only = rospy.get_param("~use_haar_only", False)
        self.use_depth_for_detection = rospy.get_param("~use_depth_for_detection", False)
        # Classic Kinnect FOV is 57.8° = 1.008 radians
        self.fov_width = rospy.get_param("~fov_width", 1.094)
        self.fov_height = rospy.get_param("~fov_height", 1.094)
        self.max_face_size = rospy.get_param("~max_face_size", 0.28)
        self.use_depth_for_tracking = rospy.get_param("~use_depth_for_tracking", False)
        self.auto_min_features = rospy.get_param("~auto_min_features", True)
        self.min_features = rospy.get_param("~min_features", 50) # Used only if auto_min_features is False
        self.abs_min_features = rospy.get_param("~abs_min_features", 6)
        self.std_err_xy = rospy.get_param("~std_err_xy", 2.5)
        self.pct_err_z = rospy.get_param("~pct_err_z", 0.42)
        self.max_mse = rospy.get_param("~max_mse", 10000)
        self.good_feature_distance = rospy.get_param("~good_feature_distance", 5)
        self.add_feature_distance = rospy.get_param("~add_feature_distance", 10)
        self.flip_image = rospy.get_param("~flip_image", False)
        self.feature_type = rospy.get_param("~feature_type", 0) # 0 = Good Features to Track, 1 = SURF
        self.expand_roi_init = rospy.get_param("~expand_roi", 1.02)
        self.expand_roi = self.expand_roi_init

        self.camera_frame_id = "kinect_depth_optical_frame"

        """ Set up the face detection parameters """
        self.cascade_frontal_alt = rospy.get_param("~cascade_frontal_alt", "")
        # optional for Face detection
        self.cascade_frontal_alt2 = rospy.get_param("~cascade_frontal_alt2", False)
        self.cascade_profile = rospy.get_param("~cascade_profile", False)
        self.cascade_frontal_alt = cv2.CascadeClassifier(self.cascade_frontal_alt)
        if self.cascade_frontal_alt2:
            self.cascade_frontal_alt2 = cv2.CascadeClassifier(self.cascade_frontal_alt2)
        if self.cascade_profile:
            self.cascade_profile = cv2.CascadeClassifier(self.cascade_profile)

        # set up track_next_frame()
        ## The color of the rectangle we draw around the face
        self.rectangleColor = (0, 165, 255)
        ## variables holding the current frame number and the current faceid
        self.frameCounter = 0
        self.currentFaceID = 0
        ## Variables holding the correlation trackers and the name per faceid
        self.faceTrackers = {}
        self.faces = {}
        self.facesLock = threading.Lock()

        #publishing:
        self.publishers = {}
        # Camera name
        self.camera_name = rospy.get_param("~camera_name", 'camera')
        # publish data to camera namespace. Can use multiple cameras in single namespace.
        self.event_pub = rospy.Publisher(
            self.camera_name + "/" + self.TOPIC_EVENT,
            FaceEvent,
            queue_size=10
        )
        self.faces_pub = rospy.Publisher(
            self.camera_name + "/" + self.TOPIC_FACES,
            Faces,
            queue_size=10
        )

        """ A service to handle 'keystroke' commands sent from other nodes """
        self.key_command = None
        rospy.Service('~key_command', KeyCommand, self.key_command_callback)

        """ Wait until the image topics are ready before starting """
        rospy.wait_for_message(self.input_rgb_image, Image)

        if self.use_depth_for_detection or self.use_depth_for_tracking:
            rospy.wait_for_message(self.input_depth_image, Image)

    def _add_entry(self, face_box):
        #Update data structures
        self.facesLock.acquire()
        self.faces[face_box.face_id] = face_box
        self.facesLock.release()
        # Dispatch ROS event.
        print("publish TOPIC_EVENT: EVENT_NEW_FACE " + self.faces[face_box.face_id].name)
        self.event_pub.publish(
            face_event=self.EVENT_NEW_FACE,
            face_id=face_box.face_id
        )

    def _remove_entry(self, face_id):
        # Dispatch ROS event
        print("publish TOPIC_EVENT: EVENT_LOST_FACE " + self.faces[face_id].name)
        self.event_pub.publish(
            face_event=self.EVENT_LOST_FACE,
            face_id=face_id
        )
        # Update data structures
        self.facesLock.acquire()
        del self.faces[face_id]
        self.facesLock.release()

    def publish_faces(self):
        faces =[]
        for f in self.faces.keys():
            #if self.faces[f].is_trackable():
            face = Face()
            face.id = f
            face.point = self.faces[f].get_filtered_3d_point()
            face.attention = self.faces[f].attention
            faces.append(face)
        # Dispatch ROS event
        #print("publish TOPIC_FACES count=" + str(len(faces)))
        msg = Faces()
        msg.faces = faces
        self.faces_pub.publish(msg)

    # We are not doing face recognition yet, but this is where we
    # could do a face lookup against already memorized faces in order
    # to recognize the "new_face".
    def doRecognizePerson(self, fid, box):
        time.sleep(2)
        x, y, w, h = box
        pt1 = (x,y)
        pt2 = (x + w, y + h)
        self._add_entry(FaceBox(fid, pt1, pt2, "Person " + str(fid)))
        print("NEW FACE: " + self.faces[fid].name)

    def start_new_tracker(self, cv_image, x, y, w, h):
        print("Creating new tracker " + str(self.currentFaceID))

        # Create and store the tracker
        tracker = dlib.correlation_tracker()
        tracker.start_track(cv_image,
                            dlib.rectangle(x - 10,
                                           y - 20,
                                           x + w + 10,
                                           y + h + 20))

        self.faceTrackers[self.currentFaceID] = tracker

        # Start a new thread that is used to simulate
        # face recognition. This is not yet implemented in this
        # version :)
        t = threading.Thread(target=self.doRecognizePerson,
                             args=(self.currentFaceID, (x, y, w, h)))
        t.start()

        # Increase the currentFaceID counter
        self.currentFaceID += 1

    def check_is_tracking(self, cv_image, x, y, w, h):
        # calculate the centerpoint
        x_bar = x + 0.5 * w
        y_bar = y + 0.5 * h

        # Variable holding information which faceid we
        # matched with
        matchedFid = None

        # Now loop over all the trackers and check if the
        # centerpoint of the face is within the box of a
        # tracker
        for fid in self.faceTrackers.keys():
            tracked_position = self.faceTrackers[fid].get_position()

            t_x = int(tracked_position.left())
            t_y = int(tracked_position.top())
            t_w = int(tracked_position.width())
            t_h = int(tracked_position.height())

            # calculate the centerpoint
            t_x_bar = t_x + 0.5 * t_w
            t_y_bar = t_y + 0.5 * t_h

            # check if the centerpoint of the face is within the
            # rectangleof a tracker region. Also, the centerpoint
            # of the tracker region must be within the region
            # detected as a face. If both of these conditions hold
            # we have a match
            if ((t_x <= x_bar <= (t_x + t_w)) and
                    (t_y <= y_bar <= (t_y + t_h)) and
                    (x <= t_x_bar <= (x + w)) and
                    (y <= t_y_bar <= (y + h))):
                matchedFid = fid

        # If no matched fid, then we have to create a new tracker
        if matchedFid is None:
            self.start_new_tracker(cv_image, x, y, w, h)
        else:
            if matchedFid in self.faces.keys():
                fb = self.faces[matchedFid]
                fb.pt1 = (x,y)
                fb.pt2 = (x + w, y + h)
                fb.update_box(fb.face_box())

    def draw_face_boxes(self, cv_image):
        # Now loop over all the trackers we have and draw the rectangle
        # around the detected faces. If we 'know' the name for this person
        # (i.e. the recognition thread is finished), we print the name
        # of the person, otherwise the message indicating we are detecting
        # the name of the person
        for fid in self.faceTrackers.keys():
            tracked_position = self.faceTrackers[fid].get_position()

            t_x = int(tracked_position.left())
            t_y = int(tracked_position.top())
            t_w = int(tracked_position.width())
            t_h = int(tracked_position.height())

            cv2.rectangle(cv_image, (t_x, t_y),
                          (t_x + t_w, t_y + t_h),
                          self.rectangleColor, 2)

            if fid in self.faces.keys():
                cv2.putText(cv_image, self.faces[fid].name,
                            (int(t_x + t_w / 2), int(t_y)),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 255, 255), 2)
            else:
                cv2.putText(cv_image, "Detecting...",
                            (int(t_x + t_w / 2), int(t_y)),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 255, 255), 2)

    def detect_faces(self, cv_image):
        # For the face detection, we need to make use of a gray
        # colored image so we will convert the baseImage to a
        # gray-based image
        gray = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
        # Now use the haar cascade detector to find all faces
        # in the image
        faces = self.cascade_frontal_alt.detectMultiScale(gray, 1.3, 5)

        # Loop over all faces and check if the area for this
        # face is the largest so far
        # We need to convert it to int here because of the
        # requirement of the dlib tracker. If we omit the cast to
        # int here, you will get cast errors since the detector
        # returns numpy.int32 and the tracker requires an int
        for (_x, _y, _w, _h) in faces:
            x = int(_x)
            y = int(_y)
            w = int(_w)
            h = int(_h)
            self.check_is_tracking(cv_image, x, y, w, h)

    # Use the DBL library to track faces over time
    def track_next_frame(self, cv_image):
        # STEPS:
        # * Update all trackers and remove the ones that are not
        #   relevant anymore
        # * Every 10 frames:
        #       + Use face detection on the current frame and look
        #         for faces.
        #       + For each found face, check if centerpoint is within
        #         existing tracked box. If so, nothing to do
        #       + If centerpoint is NOT in existing tracked box, then
        #         we add a new tracker with a new face-id

        # Increase the framecounter
        self.frameCounter += 1

        # Update all the trackers and remove the ones for which the update
        # indicated the quality was not good enough
        fidsToDelete = []
        for fid in self.faceTrackers.keys():
            trackingQuality = self.faceTrackers[fid].update(cv_image)

            # If the tracking quality is good enough, we must delete
            # this tracker
            if trackingQuality < 7:
                fidsToDelete.append(fid)

        for fid in fidsToDelete:
            print("LOST FACE: fid " + str(fid) + " from list of trackers")
            self.faceTrackers.pop(fid, None)
            self._remove_entry(fid)

        # Every 10 frames, we will have to determine which faces
        # are present in the frame
        if (self.frameCounter % 10) == 0:
            self.detect_faces(cv_image)

        self.draw_face_boxes(cv_image)

    def process_image(self, cv_image):
        self.track_next_frame(cv_image)

        self.publish_faces()
        return cv_image

    def key_command_callback(self, req):
        self.key_command = req.command
        return KeyCommandResponse()

def main(args):

    """ Fire up the Face Tracker node """
    FT = FaceTracker("pi_face_tracker")

    try:
        rospy.spin()
    except KeyboardInterrupt:
        print("Shutting down face tracker node.")
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main(sys.argv)