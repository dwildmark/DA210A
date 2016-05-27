import cv2
import math
import time
import serial
import serial.tools.list_ports
import imutils.video.videostream as imv
from imutils.video import FPS
import imutils.perspective as perspective
import socket
import numpy as np

looping = True
start_time_missing = 0
arduino_com = ArduinoCom()
analyzer = ImageAnalyzer()
controller = Controller()

while looping:

    (shape_found, distance, side_error) = analyzer.find_object()

    if shape_found:
        (speed_right, speed_left) = controller.calculate_speed(distance, side_error)
        arduino_com.write(str(speed_right) + ":" + str(speed_left) + "\n")
        state = "running"

    elif state == "running" and not shape_found:
        start_time_missing = cv2.getTickCount()
        state = "missing"

    elif state == "missing" and not shape_found:
        if float((cv2.getTickCount() - start_time_missing)) / cv2.getTickFrequency() > maximum_missing_time:
            arduino_com.write("0:0\n")
            state = "stopped"

    elif state == "stopped" and not shape_found:
        pass

video.stop()
cv2.destroyAllWindows()


class Tracker(object):
    def __init__(self):
        super(ClassName, self).__init__()

        self.looping = True
        self.start_time_missing = 0
        self.arduino_com = ArduinoCom()
        self.analyzer = ImageAnalyzer()
        self.controller = Controller()

class ArduinoCom(object):

    def __init__(self):
        super(ClassName, self).__init__()
        self.ser = None

        addr = ''
        for p in list(serial.tools.list_ports.comports()):
            if '/dev/ttyACM' in p[0]:
                addr = str(p[0])
                break
        try:
            self.ser = serial.Serial(
                port=addr, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=0, writeTimeout=0)
        except:
            pass

    def readline(self):
        if self.ser is not None:
            return self.ser.readline()
        else:
            return "No Arduino"

    def write(self, text):
        if self.ser is not None:
            self.ser.write(text)

    def close(self):
        if self.ser is not None:
            try:
                self.ser.close()
            except:
                pass


class ImageAnalyzer(object):
    def __init__(self, screenwidth=320, screenheight=240):
        super(ClassName, self).__init__()

        self.distance_setpoint = 150  # cm
        self.screenrads = math.radians(53.5)
        self.radiansPerPixel = float(self.screenrads) / float(self.screenwidth)
        self.maxDist = 500  # cm
        self.targetWidth = 20  # cm
        self.targetHeight = 20  # cm
        self.minWidth = (targetWidth / float(maxDist)) / radiansPerPixel
        self.minHeight = (targetHeight / float(maxDist)) / radiansPerPixel
        self.module = "No Module attached"
        self.status = ""
        self.status2 = ""

        from sys import platform as _platform
        if _platform == "linux2":
            self.video = imv.VideoStream(usePiCamera=True, resolution=(screenwidth, screenheight), framerate=60)
            # Camera warmup
            time.sleep(1)
        else:
            self.video = imv.VideoStream(resolution=(640, 480))
        self.video.start()

        cv2.namedWindow("Frame", cv2.WND_PROP_FULLSCREEN)
        cv2.setWindowProperty("Frame", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
        cv2.setMouseCallback("Frame", click_and_close)

    def find_triangle(self, image):
        (_, cnts, _) = cv2.findContours(image.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        for c in cnts:
            # approximate the contour
            peri = cv2.arcLength(c, True)
            approx = cv2.approxPolyDP(c, 0.05 * peri, True)
            if 3 == len(approx):
                # compute the solidity of the original contour
                area = cv2.contourArea(c)
                hull_area = cv2.contourArea(cv2.convexHull(c))
                solidity = area / float(hull_area)
                if solidity > 0.8:
                    return True
        return False

    def find_object(self):
        # grab the current frame and initialize the status text
        frame = self.video.read()

        # convert the frame to grayscale, blur it, and detect edges
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        blurred = cv2.GaussianBlur(gray, (7, 7), 0)
        edged = cv2.Canny(blurred, 30, 230)

        # find contours in the edge map
        (_, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        largest_area = -1
        target_distance = 0
        largest_approx = None
        shape_is_found = False
        largest_side_px = 0
        # loop over the contours
        for c in cnts:
            # approximate the contour
            peri = cv2.arcLength(c, True)
            approx = cv2.approxPolyDP(c, 0.03 * peri, True)

            # ensure that the approximated contour is "roughly" rectangular
            if 4 == len(approx):
                # compute the bounding box of the approximated contour and
                # use the bounding box to compute the aspect ratio
                (x, y), (w, h), theta = cv2.minAreaRect(approx)
                aspect_ratio = w / float(h)

                # compute the solidity of the original contour
                area = cv2.contourArea(c)
                hull_area = cv2.contourArea(cv2.convexHull(c))
                solidity = area / float(hull_area)

                # compute whether or not the width and height, solidity, and
                # aspect ratio of the contour falls within appropriate bounds
                keep_dims = w > minWidth and h > minHeight
                keep_solidity = solidity > 0.9
                keep_aspect_ratio = 0.7 <= aspect_ratio <= 1.3
                larger = area > largest_area

                # ensure that the contour passes all our tests
                if keep_dims and keep_solidity and keep_aspect_ratio and larger:
                    warped = perspective.four_point_transform(edged, approx.reshape(4, 2))
                    triangle_present = find_triangle(warped)
                    if triangle_present:
                        shape_is_found = True
                        largest_area = area
                        target_distance = targetWidth / (radiansPerPixel * max(h, w))
                        largest_approx = approx
                        cv2.drawContours(gray, [largest_approx], -1, (0, 0, 125), 2)
        side_err = 0
        if shape_is_found:
            # compute the center of the contour region
            moment = cv2.moments(largest_approx)
            (object_center_x, _) = (int(moment["m10"] / moment["m00"]), int(moment["m01"] / moment["m00"]))
            # Calculate how many radians the object differs from center
            image_x_center = frame.shape[1] / 2
            side_err = (image_x_center - object_center_x) * self.radiansPerPixel

        return shape_is_found, target_distance, side_err

    def stop(self):
        self.video.stop()
        cv2.destroyAllWindows()


class Controller(object):
    def __init__(self, setpoint=150, max_forward_speed=65, max_wheel_speed=75):
        super(ClassName, self).__init__()

        self.setpoint = setpoint
        self.max_forward_speed = max_forward_speed
        self.max_wheel_speed = max_wheel_speed

    def calculate_speed(self, process_distance, angle_deviation):
        distance_error = process_distance - self.setpoint
        v = min(math.pow(2, distance_error / 5) - 1, self.max_forward_speed)
        degrees = math.degrees(angle_deviation / 2)
        delta_v = math.sqrt(math.fabs(degrees * 15))

        if degrees > 0:
            speed_left = int(min((v + delta_v), self.max_wheel_speed))
            speed_right = int(min((v - delta_v), self.max_wheel_speed))
        else:
            speed_left = int(min((v - delta_v), self.max_wheel_speed))
            speed_right = int(min((v + delta_v), self.max_wheel_speed))

        return speed_right, speed_left
