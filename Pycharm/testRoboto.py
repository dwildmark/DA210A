
import cv2
import math
import time
import serial
import serial.tools.list_ports
import imutils.video.videostream as imv
from sys import platform as _platform
import socket

running = True
screenwidth = 320
screenheight = 240
camera = cv2.VideoCapture(0)
screenrads = 53.5 * math.pi / 180
radiansPerPixel = screenrads / float(screenwidth)
maxDist = 400 #cm
targetWidth = 20 #cm
targetHeight = 20 #cm
minWidth = (targetWidth/float(maxDist)) / radiansPerPixel
minHeight = (targetHeight/float(maxDist)) / radiansPerPixel

def getIP():
    try:
        return [l for l in ([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][:1], [[(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]]) if l][0][0]
    except:
        return "Not connected to Wifi"

def click_and_close(event, x, y, flags, param):
    global running
    running = False 

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


if _platform == "linux2":
    video = imv.VideoStream(usePiCamera=True, resolution=(screenwidth,screenheight), framerate=30)
else:
    video = imv.VideoStream(resolution=(640,480))
video.start()

#Fullscreen
cv2.namedWindow("Frame", cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty("Frame", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
cv2.setMouseCallback("Frame", click_and_close)

ports = list(serial.tools.list_ports.comports())
addr = ''
for p in ports:
    print p[0]
    if '/dev/ttyACM' in p[0]:
        addr = str(p[0])
try:
    ser = serial.Serial(
        port=addr, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,
        timeout=0, writeTimeout=0)
except:
    ser = None
    
# keep looping
time.sleep(1)
while running:
    #time.sleep(0.1)
    # grab the current frame and initialize the status text
    frame = video.read()
    status = "No Targets"
    status2 = "0:0"

    # convert the frame to grayscale, blur it, and detect edges
    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    (B, G, R) = cv2.split(frame)
    blurred = cv2.GaussianBlur(R, (7, 7), 0)
    edged = cv2.Canny(blurred, 30, 200)


    # find contours in the edge map
    (_, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    
    largestArea = -1
    distance = -1
    largestApprox = None

    # loop over the contours
    for c in cnts:
        # approximate the contour
        peri = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.05 * peri, True)

        # ensure that the approximated contour is "roughly" rectangular
        if 4 <= len(approx) <= 5:
            # compute the bounding box of the approximated contour and
            # use the bounding box to compute the aspect ratio
            (x, y, w, h) = cv2.boundingRect(approx)
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
            triangle_present = find_triangle(edged.copy()[y:y+h, x:x+w])

            # ensure that the contour passes all our tests
            if keepDims and keepSolidity and keepAspectRatio and larger and triangle_present:
                largestArea = area
                distance = 24.5 / (radiansPerPixel * max(h, w))
                largestApprox = approx
                
    if largestArea > 0:
        # draw an outline around the target and update the status text
        cv2.drawContours(frame, [largestApprox], -1, (0, 0, 125), 2)
        status = "D: " + str(int(distance)) + "cm "
        # compute the center of the contour region and draw the
        # crosshairs
        M = cv2.moments(largestApprox)
        (cX, cY) = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
        (startX, endX) = (int(cX - (w * 0.15)), int(cX + (w * 0.15)))
        (startY, endY) = (int(cY - (h * 0.15)), int(cY + (h * 0.15)))
        cv2.line(frame, (startX, cY), (endX, cY), (0, 0, 125), 2)
        cv2.line(frame, (cX, startY), (cX, endY), (0, 0, 125), 2)

        # Calculate pulsewidths for Arduino
        middleX = frame.shape[1] / 2

        sideError = (middleX - cX) * float(radiansPerPixel)
        distanceError = (distance - 100) * 4
        distanceError = min(max(-500, distanceError), 500)
        v = max(math.exp((distance-100)/5), 70)
        degrees = (sideError)*180/math.pi
 
        deltaV = math.sqrt(math.fabs(degrees*20))
 
        if degrees > 0:
           VL = int(max((v + deltaV), 80))
           VR = int(max((v - deltaV), 80))
        else:
           VL = int(max((v - deltaV), 80))
           VR = int(max((v + deltaV), 80))
 
        
        PWL = (distanceError + 1500) + (sideError / 2) * 1000
        PWR = (distanceError + 1500) - (sideError / 2) * 1000

        PWL = int(min(max(1000, PWL), 2000))
        PWR = int(min(max(1000, PWR), 2000))

        VL = int((PWL - 1500) / 6.25)
        VR = int((PWR - 1500) / 6.25)
        
        if ser is not None:
            stringen = str(VL) + ":" + str(VR) 
            status2 = stringen
            ser.write(stringen + "\r\n")
        #print(ser.readline())

    else:
        if ser is not None:
            ser.write("0:0" + "\r\n")

    if ser is not None:
        status2 += " " + ser.readline()

    cv2.putText(frame, getIP(), (20, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 150), 2)
    cv2.putText(frame, status, (20, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 150), 2)
    cv2.putText(frame, status2, (20, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 150), 2)
    cv2.imshow("Frame", frame)

    key = cv2.waitKey(1) & 0xFF
    if key == ord("q"):
        break

    if not running:
        break

# cleanup the camera and close any open windows
if ser is not None:
    ser.close()
camera.release()
cv2.destroyAllWindows()
