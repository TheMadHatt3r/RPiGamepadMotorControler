#######################################################
# RaspberryPI RC control software.

#######################################################
from inputs import get_gamepad
import time
import serial
import queue
import threading
from multiprocessing import Queue

STICK_ZERO_THRESH = 3
STICK_ONE_THRESH = 61



q = Queue()




"""Filter Inputs for only button/stick data.
    Outputs -64 to 63. (7 bit)"""
def inputFilter(event):
    #print(event.ev_type, event.code, event.state)
    if event.ev_type is not "Sync":
        eventcode = event.code
        data = event.state
        if eventcode in ('ABS_Y','ABS_RY'):
            data=round(data/512)
            #Force 0 on bad sticks.
            if data > -1*STICK_ZERO_THRESH and data < STICK_ZERO_THRESH: data=0
            if data > STICK_ONE_THRESH: data = 63
            if data < -1*STICK_ONE_THRESH: data = -64
            return str(eventcode) + "=" + str(data)


def worker():
    while True:
        for event in get_gamepad():
            eventResult = inputFilter(event)
            if eventResult is not None:
                q.put(eventResult)
                

def main():

    t = threading.Thread(target=worker)
    t.daemon = True
    t.start()
    cnt=0

    while 1:
        eventResult = "?"
        if q.qsize() > 0:
            eventResult = q.get()
        if eventResult.startswith("ABS_Y"):
            motorL=eventResult.split("=")[1]
            print("  ml=" + str(motorL))
        elif eventResult.startswith("ABS_RY"):
            motorR=eventResult.split("=")[1]
            print("  mr=" + str(motorR))
        cnt = cnt + 1
        print(str(cnt))
        time.sleep(0.05)

           
        
if __name__ == "__main__":
    main()
    
    



    

    
    
    
    
    
    
    
    
    
    
    
    
    