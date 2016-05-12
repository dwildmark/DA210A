
import cv2
import math
import time
import serial
import imutils.video.videostream as imv
from sys import platform as _platform

screenwidth = 640
screenheight = 480
camera = cv2.VideoCapture(0)
screenrads = 53.5 * math.pi / 180
radiansPerPixel = screenrads / screenwidth
maxDist = 400
targetWidth = 24.5
targetHeight = 22.5
minWidth = (targetWidth/maxDist) * radiansPerPixel
minHeight = (targetHeight/maxDist) * radiansPerPixel


#ser = serial.Serial(
#    port='/dev/ttyACM2', baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,
#    timeout=0, writeTimeout=0)

#print("connected to: " + ser.portstr)
if _platform == "linux2":
    video = imv.VideoStream(usePiCamera=True, resolution=(screenwidth,screenheight), framerate=6)
else:
    video = imv.VideoStream(resolution=(640,480))
video.start()

# keep looping
time.sleep(5)
while True:
    #time.sleep(0.1)
    # grab the current frame and initialize the status text
    frame = video.read()
    status = "No Targets"

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
        approx = cv2.approxPolyDP(c, 0.10 * peri, True)

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
            keepDims = w > 20 and h > 20
            keepSolidity = solidity > 0.9
            keepAspectRatio = 0.8 <= aspectRatio <= 1.2

            # ensure that the contour passes all our tests
            if keepDims and keepSolidity and keepAspectRatio:
                if area > largestArea and w > minWidth and h > minHeight:
                    largestArea = area
                    distance = 24.5 / (radiansPerPixel * max(h, w))
                    largestApprox = approx
                
    if largestArea > 0:
        # draw an outline around the target and update the status text
        cv2.drawContours(frame, [largestApprox], -1, (0, 0, 255), 4)
        status = "Distance: " + str(distance) + "cm, pixels: " + str(w) + "x" + str(h)
        # compute the center of the contour region and draw the
        # crosshairs
        M = cv2.moments(largestApprox)
        (cX, cY) = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
        (startX, endX) = (int(cX - (w * 0.15)), int(cX + (w * 0.15)))
        (startY, endY) = (int(cY - (h * 0.15)), int(cY + (h * 0.15)))
        cv2.line(frame, (startX, cY), (endX, cY), (0, 0, 255), 3)
        cv2.line(frame, (cX, startY), (cX, endY), (0, 0, 255), 3)

        # Calculate pulsewidths for Arduino
        middleX = frame.shape[1] / 2

        sideError = middleX - cX
        distanceError = (distance - 100) * 4
        PWL = (distanceError + 1500) + (sideError / 2) * 0.1
        PWR = (distanceError + 1500) - (sideError / 2) * 0.1

        PWL = int(min(max(1000, PWL), 2000))
        PWR = int(min(max(1000, PWR), 2000))

        stringen = str(PWL) + ":" + str(PWR)
        print(stringen)
        #ser.write(stringen + "\r\n")
        #print(ser.readline())



    cv2.putText(frame, status, (20, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
    cv2.imshow("Frame", frame)
    key = cv2.waitKey(1) & 0xFF
    if key == ord("q"):
        break

# cleanup the camera and close any open windows
ser.close()
camera.release()
cv2.destroyAllWindows()
