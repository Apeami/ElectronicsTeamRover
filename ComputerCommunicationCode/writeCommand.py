import serial
import time
import sys
#import yourMom #joe mama is so fat that we need her to work the communication method

#your mom haha
#These are the values that can be changed depending on the situation
port = "/dev/cu.usbmodem14101" #this is the port that is used to access the arduino and will change

ARREYLEN=5

ANGLEMIN=-180
ANGLEMAX=180

SPEEDMIN=-255
SPEEDMAX=255
#End

#Global constants
#Wl, Wr, S1, S2, con
arrayToSend = [0,0,0,0,0]

try:
    ser=serial.Serial(port,9600) #this is the setup of the serial connection
except:
    print("No serial port to be found. Maybe try using your mom's port. 笑笑")
    sys.exit(1)

def setupCommunication():
    #This gets it all ready to go idk why i need to do this.
    setOnState(0)


def setWheel(speed, side):
    speed = speed - SPEEDMIN
    speed = speed / (SPEEDMAX-SPEEDMIN)
    speed = speed *127

    if side =="Left":
        arrayToSend[0]=speed
    if side =="Right":
        arrayToSend[1]=speed
    transmitWire()


def setServo(angle, number): #Your mom has some thicc angles, be sure not to overwork her
    angle = angle - ANGLEMIN
    angle = angle / (ANGLEMAX-ANGLEMIN)
    angle = angle *127

    if number ==1:
        arrayToSend[2]=angle
    if number ==2:
        arrayToSend[3]=angle
    transmitWire()

def setOnState(state):
    if state==0 or state==1:
        arrayToSend[4] =  state
    else:
        arrayToSend[4] = 0
    transmitWire()

def transmitWire():
    array=arrayToSend #This is the array of data that we are sending to the arduino via your mom
    #array=[0,0,0,0,1]
    print(array)
    send=b''
    for x in array:
        print(str.encode(chr(int(x))))
        add = str.encode(chr(int(x)))
        send=send+add #we will change it into a string of bytes because python is not fun
    print(send)
    ser.write(send) #send the string through yourmom's serial port.

def closeCommunication():
    ser.close() #it is important to close the communication channel (yourMom) at the end. Your mom might risk a stroke if worked too much because she is so fat

def testFunction():
    #test function that can be written to to test the code
    while 1:
        inp = input("Command: ")
        if inp == "P":
            data = input("Data: ")
            setOnState(int(data))
        if inp == "M":
            data = input("Data: ")
            side = input("Side: ")
            setWheel(int(data), side)
        if input == "Q":
            break
            return
        if input == "D":
            data = input("Data: ")
            setWheel(int(data), "Left")
            setWheel(int(data), "Right")
        if input =="T":
            data = input("Data: ")
            setWheel(int(data), "Left")
            setWheel(-int(data), "Right")

if __name__=="__main__":
    setupCommunication()
    testFunction()
    closeCommunication()
    print("Control program Exit")
