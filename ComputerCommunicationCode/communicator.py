#Import required libraries

import signal
import serial
import time
import sys
from pynput.keyboard import Key, Listener
import threading
from datetime import datetime

#These are the values that can be changed depending on the situation

#Global constants
ARREYLEN=5

ANGLEMIN=-180
ANGLEMAX=180

SPEEDMIN=-255
SPEEDMAX=255
#End


class Communicator:

    def __init__(self,port,outputAll = False):
        print("Communication Setup")

        signal.signal(signal.SIGINT, self.handler)
        dateTime=datetime.now().strftime("%m-%d-%Y-%H:%M:%S")
        self.fileName= "RoverData-"+dateTime+".csv"
        self.saveFile = open(self.fileName,"w")

        self.outputAll=outputAll
        self.recievedValues = [0,0,0,0,0,0,0,0] #Connection, Speed, angular speed, orientation, posX, posY,posZ , voltage

        self.arrayToSend = [63.5,63.5,0,0,0] #Wl, Wr, S1, S2, con
        self.prevArray = [0,0,0,0,0]
        try:
            print(port)
            self.ser=serial.Serial(port,9600) #this is the setup of the serial connection
            print("Serial port succesfully connected")
            self.connect = True
            #This gets it all ready to go idk why i need to do this.
            self.setCurrentState(0)

        except:
            print("No serial port to be found.")
            self.connect = False
            return
            #sys.exit(1)

        #Start threading
        self.t1 = threading.Thread(target=self.threadRecieve)
        self.runRecieve=True
        self.t1.start()

    def handler(self,signum, frame):
        self.saveFile.close()
        sys.exit(1)



    def threadRecieve(self):
        tickToSave = 0
        while self.runRecieve==True:
            for i in range(0,8):
                line = self.ser.read()
                line= int.from_bytes(line, "big")
                self.recievedValues[i]=line
                self.saveFile.write(str(line)+",")
                if self.outputAll==True:
                    print(self.recievedValues)
            self.saveFile.write("\n")

        self.saveFile.close()
        self.recievedValues = [0,0,0,0,0,0,0,0]
        self.ser.close() #it is important to close the communication channel

    def setWheel(self,speed, side):
        speed = speed - SPEEDMIN
        speed = speed / (SPEEDMAX-SPEEDMIN)
        speed = speed *127
        if side =="Left":
            self.arrayToSend[0]=speed
        if side =="Right":
            self.arrayToSend[1]=speed
        self.transmitWire()

    def setServo(self,angle, number):
        angle = angle - ANGLEMIN
        angle = angle / (ANGLEMAX-ANGLEMIN)
        angle = angle *127

        if number ==1:
            self.arrayToSend[2]=angle
        if number ==2:
            self.arrayToSend[3]=angle
        self.transmitWire()

    def setCurrentState(self,state):
        if state>=0 and state<=7:
            self.arrayToSend[4] =  state
        else:
            self.arrayToSend[4] = 0
        self.transmitWire()

    def transmitWire(self):
        if self.arrayToSend != self.prevArray:
            ## TEMP:
            # if self.arrayToSend[0]==63.5 and self.arrayToSend[1]==63.5:
            #     self.arrayToSend[4]=0
            # else:
            #     self.arrayToSend[4]=1


            array=self.arrayToSend #This is the array of data that we are sending to the arduino
            #array=[0,0,0,0,1]
            print(array)
            send=b''
            for x in array:
                print(str.encode(chr(int(x))))
                add = str.encode(chr(int(x)))
                send=send+add #we will change it into a string of bytes because python is not fun
            print(send)
            self.ser.write(send)
        for i in range(0,5):
            self.prevArray[i] = self.arrayToSend[i]

    def closeCommunication(self):
        print("End of communication")
        self.runRecieve=False

    def on_press(self,key):
        if key == Key.up:
            self.setWheel(255, "Left")
            self.setWheel(255, "Right")
        if key == Key.down:
            self.setWheel(-255, "Left")
            self.setWheel(-255, "Right")
        if key == Key.left:
            self.setWheel(-255, "Left")
            self.setWheel(255, "Right")
        if key == Key.right:
            self.setWheel(255, "Left")
            self.setWheel(-255, "Right")

    def on_release(self,key):
        if key == Key.up or key == Key.down or key == Key.left or key == Key.right:
            print("RELEASE")
            self.setWheel(0, "Left")
            self.setWheel(0, "Right")

    def keyboard_setup(self):
        # Collect events until released
        with Listener(
                on_press=self.on_press,
                on_release=self.on_release) as listener:
                listener.join()



    #This function is only run when the program is being runi without GUI and does setup
    def start(self):
        #setupCommunication()
        if self.connect==True:
            self.setCurrentState(1) #The current state is turned on
            self.keyboard_setup()
            self.closeCommunication() #Ensures that communication is closed at the end

#If the program is run independantly there is no GUI an only keyboard is supported
if __name__ == "__main__":
    if len(sys.argv[1])>=2:
        Communicator(sys.argv[1],True).start()


#Code for testing Not used at the moment and might never be used.

# def testFunction():
#     pass
#     #test function that can be written to to test the code
#     # print("TEST")
#     # while 1:
#     #     print("RUN")
#     #     print(isUp)
#     #     if isUp:
#     #         print("IS UP")
#     #         setWheel(255, 0)
#     #         setWheel(255, 1)
#     #     if isDown:
#     #         setWheel(-255, 0)
#     #         setWheel(-255, 1)
#     #     if isLeft:
#     #         setWheel(255, 0)
#     #         setWheel(-255, 1)
#     #     if isRight:
#     #         setWheel(-255, 0)
#     #         setWheel(255, 1)
#
#         # inp = input("Command: ")
#         # if inp == "P":
#         #     data = input("Data: ")
#         #     setOnState(int(data))
#         # if inp == "M":
#         #     data = input("Data: ")
#         #     side = input("Side: ")
#         #     setWheel(int(data), side)
#         # if inp == "D":
#         #     print("HERE")
#         #     data = input("Data: ")
#         #     setWheel(int(data), "Left")
#         #     setWheel(int(data), "Right")
#         # if inp =="T":
#         #     data = input("Data: ")
#         #     setWheel(int(data), "Left")
#         #     setWheel(-int(data), "Right")
#         # if inp == "Q":
#         #     break
#         #     return
