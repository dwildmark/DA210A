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

while looping:

	if shape_found:
		state = "running"

	elif state == "running" and not shape_found:
		state = "missing"
		start_time_missing = cv2.getTickCount()

	elif state == "missing" and not shape_found:
		if float((cv2.getTickCount() - start_time_missing)) / cv2.getTickFrequency() > maximum_missing_time:
			state = "stopped"

	elif state == "stopped" and not shape_found:
		pass

video.stop()
cv2.destroyAllWindows()

class ArduinoCom(object):
	"""docstring for ClassName"""
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
			port=addr, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,
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

class ImageAnalyzer(object):
	"""docstring for ClassName"""
	def __init__(self, screenwidth,screenheight):
		super(ClassName, self).__init__()

		self.distance_setpoint = 150 #cm
		self.screenrads = math.radians(53.5)
		self.fps = FPS()
		self.radiansPerPixel = float(self.screenrads) / float(self.screenwidth)
		self.maxDist = 500 #cm
		self.targetWidth = 20 #cm
		self.targetHeight = 20 #cm
		self.minWidth = (targetWidth/float(maxDist)) / radiansPerPixel
		self.minHeight = (targetHeight/float(maxDist)) / radiansPerPixel
		self.module = "No Module attached"

		from sys import platform as _platform
		if _platform == "linux2":
			self.video = imv.VideoStream(usePiCamera=True, resolution=(screenwidth,screenheight), framerate=60)
			# Camera warmup
			time.sleep(1)
		else:
			self.video = imv.VideoStream(resolution=(640,480))
		self.video.start()
		
		cv2.namedWindow("Frame", cv2.WND_PROP_FULLSCREEN)
		cv2.setWindowProperty("Frame", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
		cv2.setMouseCallback("Frame", click_and_close)
