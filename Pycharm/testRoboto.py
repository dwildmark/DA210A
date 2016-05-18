
import cv2
import math
import time
import serial
import serial.tools.list_ports
import imutils.video.videostream as imv
import imutils.perspective as perspective
from sys import platform as _platform
import socket

def getIP():
    try:
        return [l for l in ([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][:1], [[(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) if l][0][0]
    except:
        return "Not connected to Wifi"

def click_and_close(event, x, y, flags, param):
    global looping
    looping = False 

# Finds a triangle shape in a numpy array
def find_triangle(image):
    (_, cnts, _) = cv2.findContours(image.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for c in cnts:
    # approximate the contour
        peri = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.05 * peri, True)
        if 3 == len(approx):
            # compute the solidity of the original contour
            area = cv2.contourArea(c)
            hullArea = cv2.contourArea(cv2.convexHull(c))
            solidity = area / float(hullArea)
            if solidity > 0.8:
                return True
    return False

# distance in cm, sideError in radians
# Returns tuple with speed setpoints
def calculate_speed(distance, sideError):
    v = min(math.pow(2, (distance-distance_setpoint)/5) - 1, 65)
    degrees = math.degrees(sideError / 2)
    deltaV = math.sqrt(math.fabs(degrees*15))

    if degrees > 0:
       VL = int(min((v + deltaV), 75))
       VR = int(min((v - deltaV), 75))
    else:
       VL = int(min((v - deltaV), 75))
       VR = int(min((v + deltaV), 75))

    return (VR, VL)

# Attempts to connect to Arduino and returns a serial object
# Returns None if connection fails 
def connect_to_arduino():
    ports = list(serial.tools.list_ports.comports())
    addr = ''
    for p in ports:
        if '/dev/ttyACM' in p[0]:
            addr = str(p[0])
            break
    try:
        ser = serial.Serial(
            port=addr, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,
            timeout=0, writeTimeout=0)
    except:
        ser = None

    return ser

looping = True
distance_setpoint = 165 #cm
maximum_missing_time = 0.7
state = "stopped"
start_time_missing = 0
screenwidth = 400 #pixels
screenheight = 300 #pixels
screenrads = 53.5 * math.pi / float(180)
radiansPerPixel = float(screenrads) / float(screenwidth)
maxDist = 400 #cm
targetWidth = 20 #cm
targetHeight = 20 #cm
minWidth = (targetWidth/float(maxDist)) / radiansPerPixel
minHeight = (targetHeight/float(maxDist)) / radiansPerPixel

if _platform == "linux2":
    video = imv.VideoStream(usePiCamera=True, resolution=(screenwidth,screenheight), framerate=30)
else:
    video = imv.VideoStream(resolution=(640,480))
video.start()
# Camera warmup
time.sleep(1)

#Fullscreen
cv2.namedWindow("Frame", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("Frame", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
cv2.setMouseCallback("Frame", click_and_close)

ser = connect_to_arduino()
ip = getIP()

# keep looping
while looping:
    # grab the current frame and initialize the status text
    frame = video.read()
    status = "No Targets"
    status2 = "-:-"

    # convert the frame to grayscale, blur it, and detect edges
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (7, 7), 0)
    edged = cv2.Canny(blurred, 30, 200)

    # find contours in the edge map
    (_, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    largestArea = -1
    distance = -1
    largestApprox = None
    shape_found = False
    # loop over the contours
    for c in cnts:
        # approximate the contour
        peri = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.05 * peri, True)

        # ensure that the approximated contour is "roughly" rectangular
        if 4 == len(approx):
            # compute the bounding box of the approximated contour and
            # use the bounding box to compute the aspect ratio
            (x,y),(w,h),theta = cv2.minAreaRect(approx)
            aspectRatio = w / float(h)

            # compute the solidity of the original contour
            area = cv2.contourArea(c)
            hullArea = cv2.contourArea(cv2.convexHull(c))
            solidity = area / float(hullArea)

            # compute whether or not the width and height, solidity, and
            # aspect ratio of the contour falls within appropriate bounds
            keepDims = w > minWidth and h > minHeight
            keepSolidity = solidity > 0.9
            keepAspectRatio = 0.7 <= aspectRatio <= 1.3
            larger = area > largestArea           

            # ensure that the contour passes all our tests
            if keepDims and keepSolidity and keepAspectRatio and larger:
                warped = perspective.four_point_transform(edged, approx.reshape(4, 2))
                triangle_present = find_triangle(warped)
                if triangle_present:
                    shape_found = True
                    largestArea = area
                    distance = 24.5 / (radiansPerPixel * max(h, w))
                    largestApprox = approx
                
    if shape_found:
    	state = "running"
        # draw an outline around the target and update the status text
        cv2.drawContours(frame, [largestApprox], -1, (0, 0, 125), 2)
        status = "D: " + str(int(distance)) + "cm "
        # compute the center of the contour region and draw the crosshairs
        M = cv2.moments(largestApprox)
        (cX, cY) = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
        (startX, endX) = (int(cX - (w * 0.15)), int(cX + (w * 0.15)))
        (startY, endY) = (int(cY - (h * 0.15)), int(cY + (h * 0.15)))
        cv2.line(frame, (startX, cY), (endX, cY), (0, 0, 125), 2)
        cv2.line(frame, (cX, startY), (cX, endY), (0, 0, 125), 2)

        # Calculate speed for Arduino
        middleX = frame.shape[1] / 2
        sideError = (middleX - cX) * float(radiansPerPixel)
        (VR, VL) = calculate_speed(distance, sideError)
        
        if ser is not None:
            state = "running"
            stringen = str(VR) + ":" + str(VL)
            status2 = stringen
            ser.write(stringen + "\n")
    
    elif state == "running" and not shape_found:
        state = "missing"
        start_time_missing = cv2.getTickCount()        

    elif state == "missing" and not shape_found:
        if float((cv2.getTickCount() - start_time_missing)) / cv2.getTickFrequency() > maximum_missing_time:
            state = "stopped"
    	    if ser is not None:
                ser.write("0:0" + "\n")

    elif state == "stopped" and not shape_found:
    	pass

    if ser is not None:
        status2 += " " + ser.readline() + " " + state

    cv2.putText(frame, ip, (20, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (50, 0, 200), 1)
    cv2.putText(frame, status, (20, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (50, 0, 200), 1)
    cv2.putText(frame, status2, (20, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (50, 0, 200), 1)
    cv2.imshow("Frame", frame)
    cv2.waitKey(1)

# cleanup the camera and close any open windows
if ser is not None:
    ser.close()
video.stop()
cv2.destroyAllWindows()
