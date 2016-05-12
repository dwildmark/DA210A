import cv2
import imutils.video.videostream as imv
import time
from sys import platform as _platform
import numpy as np
import mahotas


if _platform == "linux2":
    video = imv.VideoStream(usePiCamera=True, resolution=(640,480), framerate=2)
else:
    video = imv.VideoStream()

radians = 24.5 / 167
radiansPerPixel = radians / 160

video.start()
time.sleep(0.1)
while True:
    image = video.read()
    if image is not None:
        #time.sleep(0.1)
        # grab the current frame and initialize the status text
        status = "No Targets"

        # convert the frame to grayscale, blur it, and detect edges
        #gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        #(B, G, R) = cv2.split(image)
        #black = np.zeros((640,480), dtype = "uint8")
        blackLower = np.array([0,0,0], dtype="uint8")
        blackUpper = np.array([50, 50, 50], dtype="uint8")
        blue = cv2.inRange(image, blackLower, blackUpper)
        blurred = cv2.GaussianBlur(blue, (5, 5), 0)
        edged = cv2.Canny(blurred, 50, 150)

        # find contours in the edge map
        (_, cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        # loop over the contours
        for c in cnts:
            # approximate the contour
            peri = cv2.arcLength(c, True)
            approx = cv2.approxPolyDP(c, 0.01 * peri, True)

            # ensure that the approximated contour is "roughly" rectangular
            if 4 <= len(approx) <= 6:
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
                #take away this line maybe

                # ensure that the contour passes all our tests
                if keepDims and keepSolidity and keepAspectRatio:
                    # draw an outline around the target and update the status
                    # text
                    cv2.drawContours(image, [approx], -1, (0, 0, 255), 4)
                    distance = 24.5 / (radiansPerPixel * max(h, w))
                    status = "Distance: " + str(distance) + "cm, pixels: " + str(w) + "x" + str(h)

                    # compute the center of the contour region and draw the
                    # crosshairs
                    M = cv2.moments(approx)
                    (cX, cY) = (int(M["m10"] / M["m00"]), int(M["m01"] / M["m00"]))
                    (startX, endX) = (int(cX - (w * 0.15)), int(cX + (w * 0.15)))
                    (startY, endY) = (int(cY - (h * 0.15)), int(cY + (h * 0.15)))
                    cv2.line(image, (startX, cY), (endX, cY), (0, 0, 255), 3)
                    cv2.line(image, (cX, startY), (cX, endY), (0, 0, 255), 3)

        cv2.putText(image, status, (20, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
        cv2.imshow("Frame", image)
        cv2.imshow("edged", edged)
        key = cv2.waitKey(1) & 0xFF

        if key == ord("q"):
            break
video.stop()
cv2.destroyAllWindows()