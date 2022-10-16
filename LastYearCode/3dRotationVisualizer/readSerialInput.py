import serial

from math import pi, sin, cos
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
from direct.interval.IntervalGlobal import Sequence
from panda3d.core import Point3

port = "/dev/cu.usbmodem14101"

ser = serial.Serial(port,115200)

class MyApp(ShowBase):
    def __init__(self):
        ShowBase.__init__(self)
        # Disable the camera trackball controls.
        self.disableMouse()
        # Load the environment model.
        self.scene = self.loader.loadModel("models/environment")
        # Reparent the model to render.
        self.scene.reparentTo(self.render)
        # Apply scale and position transforms on the model.
        self.scene.setScale(0.25, 0.25, 0.25)
        self.scene.setPos(-8, 42, 0)

        # Add the spinCameraTask procedure to the task manager.
        self.taskMgr.add(self.spinCameraTask, "SpinCameraTask")
        self.taskMgr.add(self.moveRover, "MoveRover")

        # Load and transform the panda actor.
        self.rover = Actor("rover.bam")
        self.rover.setScale(0.005, 0.005, 0.005)
        self.rover.setPos(0,0,0)
        self.rover.setHpr(0,0,0)
        self.rover.reparentTo(self.render)
        self.posX=0


    def moveRover(self,task):
        line = ser.read()
        line=int.from_bytes(line, "big")
        line = round((line/0.7)-180,2)
        #pitch=line * 57.296 # rad to deg
        pitch=line
        print("Pitch ")
        print(line)
        print("\n")
        line = ser.read()
        line=int.from_bytes(line, "big")
        line = round((line/0.7)-180,2)
        #roll=line * 57.296 # rad to deg
        roll=line
        print("Roll ")
        print(line)
        print("\n")
        app.rover.setHpr(0,roll,pitch)
        return Task.cont


    # Define a procedure to move the camera.
    def spinCameraTask(self, task):
        angleDegrees = task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(20 * sin(angleRadians), -20 * cos(angleRadians), 3)
        self.camera.setHpr(angleDegrees, 0, 0)
        return Task.cont

for i in range(0,0):
    line = ser.readline()

print("Ready")

app = MyApp()
app.run()
