import pygame
import pygame.camera
from pygame.locals import *
import sys

import button
import communicator

pygame.init()
pygame.camera.init()
pygame.joystick.init()
pygame.display.set_caption('Rover Control Panel')


class Capture(object):
    def __init__(self,port):

        #Rover commands
        self.wheelLeft = 0
        self.wheelRight = 0
        self.servoAxis = 0
        self.servoGrap = 0
        self.modeId = 0

        #Connection
        self.mode = "Button"
        self.relay = False
        self.rover = False
        self.joystick = False
        self.keyboard = False
        self.camera = False
        self.state=False #False = off, True = on
        self.error=True
        self.port=port

        #values
        self.speed=0
        self.angSpeed=0
        self.orientation=0
        self.posx=0
        self.posy=0
        self.posz=0
        self.batvol=0

        #display
        self.scale = 1
        self.size = (640*self.scale,480*self.scale)
        self.camSize = (400*self.scale,230*self.scale)
        # create a display surface. standard pygame stuff
        self.display = pygame.display.set_mode(self.size, 0 )

        #buttons
        self.modesList = ('Button','Keyboard','JoystickCom','JoystickArd',"JoystickExt","AutomaticCom","AutomaticArd")
        self.currentCycle = 0
        self.set_buttons()

        #refresh items
        self.refresh_items()

        # create a surface to capture to.  for performance purposes
        # bit depth is the same as that of the display surface.
        self.snapshot = pygame.surface.Surface(self.camSize, 0, self.display)

    def refresh_items(self):
        #camera
        self.clist = pygame.camera.list_cameras()
        if self.clist:
            self.camera=True
            self.cam = pygame.camera.Camera(self.clist[0], self.size)
            self.cam.start()
        else:
            self.camera=False
        #joysticks
        if pygame.joystick.get_count()>0:
            self.joystick=True
            self.joystickItem = pygame.joystick.Joystick(0)
            self.joystickItem.init()
        else:
            self.joystick=False
        #keyboard
        try:
            pygame.key.get_pressed()
            self.keyboard=True
        except:
            self.keyboard=False
        #relay
        self.communication = communicator.Communicator(self.port, outputAll=False)
        if self.communication.connect ==True:
            self.relay = True
        if self.communication.connect ==False:
            self.relay = False
        #Rover
        #no implementation yet

    def get_camera(self):
        if self.camera==True:
            # if you don't want to tie the framerate to the camera, you can check
            # if the camera has an image ready.  note that while this works
            # on most cameras, some will never return true.
            if self.cam.query_image():
                self.snapshot = self.cam.get_image(self.snapshot)

            # blit it to the display surface.  simple!
            self.display.blit(self.snapshot, (10,30))

    def get_background(self):
        red = (255,0, 0)
        blue = (45,66, 79)
        white = (255,255,255)
        green = (0,0,255)
        cyan = (0,255,255)
        self.display.fill(red)
        pygame.draw.rect(self.display,blue,(10,30,400,230)) #Camera Zone
        pygame.draw.rect(self.display,white,(420,10,210,320)) #Information text zone
        pygame.draw.rect(self.display,cyan,(420,340,210,130)) #Mode select zone
        pygame.draw.rect(self.display,green,(10,270,400,200)) #Control zone

    def get_text(self):
        font = pygame.font.SysFont('Arial', 20)
        self.display.blit(font.render('Camera', False, (0, 0, 0)) , (160,5))
        self.display.blit(font.render('Data Readings', False, (0, 0, 0)) , (465,10))
        self.display.blit(font.render('Connection Status', False, (0, 0, 0)) , (447,115))
        self.display.blit(font.render('Controller', False, (0, 0, 0)) , (160,270))
        self.display.blit(font.render('Set Mode', False, (0, 0, 0)) , (480,340))
        if self.camera==False:
            self.display.blit(font.render('NO SIGNAL', False, (255, 255, 255)) , (160,140))

        if self.error ==True:
            self.display.blit(font.render('ERROR IN CONFIG!!', False, (255, 0, 0)) , (430,290))

    def set_values(self):
        font = pygame.font.SysFont('Arial', 15)
        self.display.blit(font.render('Speed: '+str(self.speed)+' cm/s', False, (0, 0, 0)) , (425,40)) #speed
        self.display.blit(font.render('Angular Speed: '+str(self.angSpeed)+' deg/s', False, (0, 0, 0)) , (425,55)) #speed
        self.display.blit(font.render('Orientation: '+str(self.orientation)+' deg', False, (0, 0, 0)) , (425,70)) #orien
        self.display.blit(font.render('Position: '+' X- '+str(self.posx)+' Y- '+str(self.posy)+' Z- '+str(self.posz), False, (0, 0, 0)) , (425,85)) #pos
        self.display.blit(font.render('Voltage: '+str(self.batvol)+' V', False, (0, 0, 0)) , (425,100)) #Voltage

        if self.relay == True:
            self.display.blit(font.render('Relay: Connection Success', False, (0, 255, 0)) , (425,140)) #Relay Connection
        else:
            self.display.blit(font.render('Relay: No Connection', False, (255, 0, 0)) , (425,140)) #Relay Connection
        if self.rover==True:
            self.display.blit(font.render('Rover: Connection Success', False, (0, 255, 0)) , (425,155)) #Rover Connection
        else:
            self.display.blit(font.render('Rover: No Connection', False, (255, 0, 0)) , (425,155)) #Rover Connection
        if self.joystick==True:
            self.display.blit(font.render('Joystick: Connection Success', False, (0, 255, 0)) , (425,170)) #Joystick Connect
        else:
            self.display.blit(font.render('Joystick: No Connection', False, (255, 0, 0)) , (425,170)) #Joystick Connect
        if self.keyboard==True:
            self.display.blit(font.render('Keyboard: Connection Success', False, (0, 255, 0)) , (425,185)) #Keyboard Connect
        else:
            self.display.blit(font.render('Keyboard: No Connection', False, (255, 0, 0)) , (425,185)) #Keyboard Connect
        if self.camera==True:
            self.display.blit(font.render('Camera: Connection Success', False, (0, 255, 0)) , (425,200)) #Camera Connect
        else:
            self.display.blit(font.render('Camera: No Connection', False, (255, 0, 0)) , (425,200)) #Camera Connect

        self.display.blit(font.render('Mode: '+self.mode, False, (0, 0, 0)) , (425,215)) #Current Mode
        if self.modeId == -1:
            self.error=True
        else:
            self.error=False

        if self.state==True:
            self.display.blit(font.render('Current State : On', False, (0, 255, 0)) , (425,230)) #Current Mode
        if self.state==False:
            self.display.blit(font.render('Current State : Off', False, (255, 0, 0)) , (425,230)) #Current Mode

    def set_buttons(self):
        self.btnStop = button.Button("STOP",(200, 355),(50,50), 15, (255,0,0))
        self.btnLeft = button.Button("Left",(260, 355),(50,50), 15, (0,180,180))
        self.btnRight = button.Button("Right",(140, 355),(50,50), 15, (0,180,180))
        self.btnTop = button.Button("Fwd",(200, 295),(50,50), 15, (0,180,180))
        self.btnDown = button.Button("Back",(200, 415),(50,50), 15, (0,180,180))

        self.btnArmUp = button.Button("Arm Up",(20, 295),(50,50), 12, (200,180,180))
        self.btnArmMid = button.Button("Arm Mid",(20, 355),(50,50), 12, (200,180,180))
        self.btnArmDown = button.Button("Arm Down",(20, 415),(50,50), 10, (200,180,180))
        self.btnArmGrab = button.Button("Arm Grab",(80, 295),(50,50), 10, (200,180,180))

        self.btnOff = button.Button("Off",(330, 295),(50,50), 15, (255,0,0))
        self.btnOn = button.Button("On",(330, 355),(50,50), 15, (0,255,0))
        self.btnHelp = button.Button("Help",(330, 415),(50,50), 15, (69,255,123))
        self.btnRefresh = button.Button("Refresh",(260, 295),(50,50), 15, (69,255,123))
        self.btnDisconnect = button.Button("Disconnect",(260, 415),(50,50), 10, (255,69,69))

        self.btnModeChoose = button.Button(self.modesList[self.currentCycle] ,(450, 370),(150,35), 15, (0,0,0))
        self.btnModeSelect = button.Button("Change Mode",(450, 420),(150,35), 15, (0,0,255))

    def run_buttons(self,event):

        self.btnStop.show(self.display)
        self.btnLeft.show(self.display)
        self.btnRight.show(self.display)
        self.btnTop.show(self.display)
        self.btnDown.show(self.display)

        self.btnArmUp.show(self.display)
        self.btnArmMid.show(self.display)
        self.btnArmDown.show(self.display)
        self.btnArmGrab.show(self.display)

        self.btnOff.show(self.display)
        self.btnOn.show(self.display)
        self.btnHelp.show(self.display)
        self.btnRefresh.show(self.display)
        self.btnDisconnect.show(self.display)

        self.btnModeSelect.show(self.display)

        if self.btnModeChoose.click(event) == True:
            self.currentCycle+=1
            if self.currentCycle>len(self.modesList)-1:
                self.currentCycle=0

        if self.btnModeSelect.click(event) == True:
            self.modeId=self.currentCycle
            self.mode = self.modesList[self.modeId]
            if self.relay == True:
                self.communication.setWheel(0, "Left")
                self.communication.setWheel(0, "Right")

        if self.btnOff.click(event) == True:
            self.state=False
        if self.btnOn.click(event) == True:
            self.state=True

        if self.btnRefresh.click(event) == True:
            self.refresh_items()

        if self.btnDisconnect.click(event) == True:
            self.communication.closeCommunication()
            self.relay=False
            self.rover=False

        if self.btnStop.click(event) == True:
            print("Pressed")

        self.btnModeChoose.change_text(self.modesList[self.currentCycle],(0,0,0))
        self.btnModeChoose.show(self.display)

    def manage_keyboard(self):
        keys=pygame.key.get_pressed()
        if self.modeId==1:
            if keys[K_LEFT] or keys[K_a]:
                self.communication.setWheel(255, "Left")
                self.communication.setWheel(-255, "Right")
            elif keys[K_RIGHT] or keys[K_d]:
                self.communication.setWheel(-255, "Left")
                self.communication.setWheel(255, "Right")
            elif keys[K_DOWN] or keys[K_s]:
                self.communication.setWheel(-255, "Left")
                self.communication.setWheel(-255, "Right")
            elif keys[K_UP] or keys[K_w]:
                self.communication.setWheel(255, "Left")
                self.communication.setWheel(255, "Right")
            #if keys[K_SPACE]:
            else:
                self.communication.setWheel(0, "Left")
                self.communication.setWheel(0, "Right")

    def manage_joystick(self):
        if self.modeId==2 and self.joystick==True:
            side = self.joystickItem.get_axis(3)
            forward = -self.joystickItem.get_axis(2)
            print(forward)
            print(side)
            if abs(side)<0.2:
                wheelL = (forward)*255
                wheelR = (forward)*255
            else:
                wheelL = (side)*255
                wheelR = -(side)*255
            self.communication.setWheel(wheelL, "Left")
            self.communication.setWheel(wheelR, "Right")

    def manage_button(self,event):
        if self.modeId==0:
            if self.btnStop.click(event) == True:
                self.communication.setWheel(0, "Left")
                self.communication.setWheel(0, "Right")
            if self.btnLeft.click(event) == True:
                self.communication.setWheel(255, "Left")
                self.communication.setWheel(-255, "Right")
            if self.btnRight.click(event) == True:
                self.communication.setWheel(-255, "Left")
                self.communication.setWheel(255, "Right")
            if self.btnTop.click(event) == True:
                self.communication.setWheel(255, "Left")
                self.communication.setWheel(255, "Right")
            if self.btnDown.click(event) == True:
                self.communication.setWheel(-255, "Left")
                self.communication.setWheel(-255, "Right")

    def manage_mode(self):
        if self.state == True:
            self.communication.setCurrentState(self.modeId+1)
        if self.state == False:
            self.communication.setCurrentState(0)

    def set_recieved_values(self):
        if self.communication.recievedValues[0] >= 1:
            self.rover = True
        if self.communication.recievedValues[0] == 0:
            self.rover = False

        self.speed = self.communication.recievedValues[1]
        self.angSpeed = self.communication.recievedValues[2]
        self.orientation = self.communication.recievedValues[3]
        self.posx = self.communication.recievedValues[4]
        self.posy = self.communication.recievedValues[5]
        self.posz = self.communication.recievedValues[6]
        self.batvol = self.communication.recievedValues[7]

    def main(self):
        clock = pygame.time.Clock()
        going = True
        while going:


            events = pygame.event.get()

            self.get_background()
            self.get_text()

            self.set_recieved_values()
            self.set_values()
            self.get_camera()
            self.run_buttons(events)


            if self.relay == True:
                self.manage_mode()
                self.manage_button(events)
                self.manage_joystick()
                self.manage_keyboard()


            for e in events:
                if e.type == QUIT or (e.type == KEYDOWN and e.key == K_ESCAPE):
                    # close the camera safely
                    self.communication.closeCommunication()
                    self.cam.stop()
                    going = False
            pygame.display.flip()

        dt = clock.tick(5)

#Start program
if __name__ == "__main__":
    if len(sys.argv[1])>=2:
        captureObj = Capture(sys.argv[1])
        captureObj.main()
