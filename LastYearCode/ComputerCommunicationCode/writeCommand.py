#Import required libraries
import serial
import time
import sys
from pynput.keyboard import Key, Listener
import pygame

#These are the values that can be changed depending on the situation
port = "/dev/cu.usbmodem14101" #this is the port that is used to access the arduino and will change

INPUT_TYPE = "Keyboard"

ARREYLEN=5

ANGLEMIN=-180
ANGLEMAX=180

SPEEDMIN=-255
SPEEDMAX=255
#End

#Global constants
#Wl, Wr, S1, S2, con
 #This is the array to send over, #ensure default to no movement


class Communicator:

    def __init__(port):
        print("Communication Setup")
        self.arrayToSend = [63.5,63.5,0,0,0]
        try:
            self.ser=serial.Serial(port,9600) #this is the setup of the serial connection
            print("Serial port succesfully connected")
        except:
            print("No serial port to be found.")
            sys.exit(1)
        #This gets it all ready to go idk why i need to do this.
        setCurrentState(0)

    def setWheel(speed, side):
        speed = speed - SPEEDMIN
        speed = speed / (SPEEDMAX-SPEEDMIN)
        speed = speed *127
        if side =="Left":
            self.arrayToSend[0]=speed
        if side =="Right":
            self.arrayToSend[1]=speed
        self.transmitWire()

    def setServo(angle, number):
        angle = angle - ANGLEMIN
        angle = angle / (ANGLEMAX-ANGLEMIN)
        angle = angle *127

        if number ==1:
            self.arrayToSend[2]=angle
        if number ==2:
            self.arrayToSend[3]=angle
        self.transmitWire()

    def setCurrentState(state):
        if state==0 or state==1:
            self.arrayToSend[4] =  state
        else:
            self.arrayToSend[4] = 0
        self.transmitWire()

    def transmitWire():
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

    def closeCommunication():
        print("End of communication")
        self.ser.close() #it is important to close the communication channel

    # def on_press(key):
    #     if key == Key.up:
    #         self.setWheel(255, "Left")
    #         self.setWheel(255, "Right")
    #     if key == Key.down:
    #         self.setWheel(-255, "Left")
    #         self.setWheel(-255, "Right")
    #     if key == Key.left:
    #         self.setWheel(-255, "Left")
    #         self.setWheel(255, "Right")
    #     if key == Key.right:
    #         self.setWheel(255, "Left")
    #         self.setWheel(-255, "Right")
    #
    # def on_release(key):
    #     if key == Key.up or key == Key.down or key == Key.left or key == Key.right:
    #         print("RELEASE")
    #         self.setWheel(0, "Left")
    #         self.setWheel(0, "Right")
    #
    # def keyboard_setup():
    #     # Collect events until released
    #     with Listener(
    #             on_press=self.on_press,
    #             on_release=self.on_release) as listener:
    #             listener.join()
    #
    # def joystick_setup():
    #     pygame.init()
    #     done = False
    #     pygame.joystick.init()
    #
    #     joystick_count = pygame.joystick.get_count()
    #     if joystick_count==0:
    #         print("NO joystick")
    #
    #     while not done:
    #         for event in pygame.event.get(): # User did something.
    #             if event.type == pygame.QUIT: # If user clicked close.
    #                 done = True # Flag that we are done so we exit this loop.
    #
    #
    #         joystick = pygame.joystick.Joystick(0)
    #         joystick.init()
    #         try:
    #             jid = joystick.get_instance_id()
    #         except AttributeError:
    #             # get_instance_id() is an SDL2 method
    #             jid = joystick.get_id()
    #             print("Joystick {}".format(jid))
    #         name = joystick.get_name()
    #         print("Joystick name: {}".format(name))
    #         try:
    #             guid = joystick.get_guid()
    #         except AttributeError:
    #             # get_guid() is an SDL2 method
    #             pass
    #         else:
    #             print("GUID: {}".format(guid))
    #
    #         axes = joystick.get_numaxes()
    #         print("Number of axes: {}".format(axes))
    #
    #         for i in range(axes):
    #             axis = joystick.get_axis(i)
    #             print("Axis {} value: {:>6.3f}".format(i, axis))
    #
    #         side = joystick.get_axis(0)
    #         forward = -joystick.get_axis(1)
    #
    #
    #         buttons = joystick.get_numbuttons()
    #         print("Number of buttons: {}".format(buttons))
    #
    #         for i in range(buttons):
    #             button = joystick.get_button(i)
    #             print("Button {:>2} value: {}".format(i, button))
    #
    #         hats = joystick.get_numhats()
    #         print("Number of hats: {}".format(hats))
    #
    #         for i in range(hats):
    #             hat = joystick.get_hat(i)
    #             print("Hat {} value: {}".format(i, str(hat)))
    #     pygame.quit()

    # def start():
    #     setupCommunication()
    #     if INPUT_TYPE=="Keyboard":
    #         keyboard_setup()
    #     if INPUT_TYPE=="Joystick":
    #         joystick_setup()
    #     closeCommunication()




#Code for testing
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
