# USAGE
# python object_movement.py --video object_tracking_example.mp4
# python object_movement.py

# import the necessary packages
from collections import deque
import numpy as np
import argparse
import serial
import time
import cv2
import imutils.video.videostream as imv

from sys import platform as _platform

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video",
                help="path to the (optional) video file")
ap.add_argument("-b", "--buffer", type=int, default=32,
                help="max buffer size")
args = vars(ap.parse_args())

# define the lower and upper boundaries of the "green"
# ball in the HSV color space
greenLower = (55, 133, 21)
greenUpper = (90, 255, 191)

#ser = serial.Serial(port='/dev/ttyACM0',
#                    baudrate=115200,
#                    parity=serial.PARITY_NONE,
#                    stopbits=serial.STOPBITS_ONE,
#                    bytesize=serial.EIGHTBITS,
#                    timeout=0,
#                    writeTimeout=0)
# initialize the list of tracked points, the frame counter,
# and the coordinate deltas
pts = deque(maxlen=args["buffer"])
counter = 0
direction = ""

# if a video path was not supplied, grab the reference
# to the webcam
if _platform == "linux2":
    video = imv.VideoStream(usePiCamera=True, resolution=(640,480), framerate=20)
else:
    video = imv.VideoStream(resolution=(640,480))
video.start()
# keep looping
time.sleep(5)
while True:
    # grab the current frame
    frame = video.read()

    # resize the frame, blur it, and convert it to the HSV
    # color space
    #frame = imutils.resize(frame, width=600)
    blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

    # construct a mask for the color "green", then perform
    # a series of dilations and erosions to remove any small
    # blobs left in the mask
    mask = cv2.inRange(hsv, greenLower, greenUpper)
    mask = cv2.erode(mask, None, iterations=2)
    mask = cv2.dilate(mask, None, iterations=2)

    # find contours in the mask and initialize the current
    # (x, y) center of the ball
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)[-2]
    center = None

    # only proceed if at least one contour was found
    if len(cnts) > 0:
        # find the largest contour in the mask, then use
        # it to compute the minimum enclosing circle and
        # centroid
        c = max(cnts, key=cv2.contourArea)
        ((x, y), radius) = cv2.minEnclosingCircle(c)
        peri = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.01 * peri, True)
        M = cv2.moments(c)
        center = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))

        # only proceed if the radius meets a minimum size
        if 4 <= len(approx) <= 6:
            (x, y, w, h) = cv2.boundingRect(approx)
            (cX, cY) = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
            (startX, endX) = (int(cX - (w * 0.15)), int(cX + (w * 0.15)))
            (startY, endY) = (int(cY - (h * 0.15)), int(cY + (h * 0.15)))
            cv2.line(frame, (startX, cY), (endX, cY), (0, 0, 255), 3)
            cv2.line(frame, (cX, startY), (cX, endY), (0, 0, 255), 3)
            cv2.drawContours(frame, [approx], -1, (0, 0, 255), 4)
            middleX = frame.shape[1] / 2
            sideError = middleX - cX
            PWL = 1500 + (sideError / 2) * 4
            PWR = 1500 - (sideError / 2) * 4

            PWL = int(min(max(1000, PWL), 2000))
            PWR = int(min(max(1000, PWR), 2000))

            stringen = str(PWL) + ":" + str(PWR)
            print(stringen)
            #ser.write(stringen + "\r\n")
            # draw the circle and centroid on the frame,
            # then update the list of tracked point
    # loop over the set of tracked points
    cv2.imshow("Frame", frame)
    key = cv2.waitKey(1) & 0xFF
    counter += 1
    if key == ord("q"):
        break


#cleanup the camera and close any open windows
video.stop()
#ser.close()
cv2.destroyAllWindows()
