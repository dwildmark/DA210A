import imutils.video.videostream as im
import serial


#variable holding the next state
next_state = 0

#port for communication with arduino
port = serial.Serial("/dev/tty.usbmodem1411", baudrate=115200, timeout=3.0)

video = im.VideoStream(framerate=40)

video.start()

#starting state, waits for arduino to signal
def state_listen():
    rcv = port.readline()
    if("start" in rcv):
        global next_state
        next_state = 1
        print "Going to state 1 from state 0"

#running state, runs the controller process
def state_running():
    rcv = port.readline()
    if("stop" in rcv):
        global next_state
        next_state = 0
        print "Going to state 0 from state 1"


options = {0 : state_listen,
           1 : state_running
}

while True:
    options[next_state]()