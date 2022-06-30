import pygame
import pygame.camera
from pygame.locals import *

pygame.init()
pygame.camera.init()
pygame.display.set_caption('Rover Control Panel')

class Button:
    """Create a button, then blit the surface in the while loop"""

    def __init__(self, text,  pos, size, font, bg):
        self.x, self.y = pos
        self.font = pygame.font.SysFont("Arial", font)
        self.size = size
        self.change_text(text, bg)

    def change_text(self, text, bg="black"):
        """Change the text whe you click"""
        self.text = self.font.render(text, 1, pygame.Color("White"))

        self.surface = pygame.Surface(self.size)
        self.surface.fill(bg)
        self.surface.blit(self.text, (self.size[0]/2-self.text.get_size()[0]/2, self.size[1]/2-self.text.get_size()[1]/2))
        self.rect = pygame.Rect(self.x, self.y, self.size[0], self.size[1])

    def show(self,screen):
        screen.blit(self.surface, (self.x, self.y))

    def click(self, event):
        x, y = pygame.mouse.get_pos()
        for e in event:
            if e.type == pygame.MOUSEBUTTONDOWN:
                if pygame.mouse.get_pressed()[0]:
                    if self.rect.collidepoint(x, y):
                        return True
        return False


def mainloop():
    """ The infinite loop where things happen """
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
            button1.click(event)
        button1.show()
        clock.tick(30)
        pygame.display.update()

class Capture(object):
    def __init__(self):

        #Connection
        self.mode = "Main Mode"
        self.relay = False
        self.rover = False
        self.joystick = False
        self.keyboard = False
        self.state=False #False = off, True = on

        #values
        self.speed=0
        self.angSpeed=0
        self.orientation=0
        self.posx=0
        self.posy=0
        self.posz=0
        self.batvol=0

        #buttons
        self.set_buttons()

        #display
        self.scale = 1
        self.size = (640*self.scale,480*self.scale)
        self.camSize = (400*self.scale,230*self.scale)
        # create a display surface. standard pygame stuff
        self.display = pygame.display.set_mode(self.size, 0 )

        # this is the same as what we saw before
        self.clist = pygame.camera.list_cameras()
        if not self.clist:
            raise ValueError("Sorry, no cameras detected.")
        self.cam = pygame.camera.Camera(self.clist[0], self.size)
        self.cam.start()

        # create a surface to capture to.  for performance purposes
        # bit depth is the same as that of the display surface.
        self.snapshot = pygame.surface.Surface(self.camSize, 0, self.display)

    def get_camera(self):
        # if you don't want to tie the framerate to the camera, you can check
        # if the camera has an image ready.  note that while this works
        # on most cameras, some will never return true.
        if self.cam.query_image():
            self.snapshot = self.cam.get_image(self.snapshot)

        # blit it to the display surface.  simple!
        self.display.blit(self.snapshot, (10,30))

    def get_background(self):
        red = (255,0, 0)
        blue = (0,255, 0)
        white = (255,255,255)
        green = (0,0,255)
        cyan = (0,255,255)
        self.display.fill(red)
        #pygame.draw.rect(self.display,blue,(10,10,400,250)) #Camera Zone
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

        self.display.blit(font.render('Mode: '+self.mode, False, (0, 0, 0)) , (425,200)) #Current Mode

        if self.state==True:
            self.display.blit(font.render('Current State : On', False, (0, 255, 0)) , (425,215)) #Current Mode
        if self.state==False:
            self.display.blit(font.render('Current State : Off', False, (255, 0, 0)) , (425,215)) #Current Mode

    def set_buttons(self):
        self.btnStop = Button("STOP",(180, 355),(50,50), 15, (255,0,0))
        self.btnLeft = Button("Left",(240, 355),(50,50), 15, (0,180,180))
        self.btnRight = Button("Right",(120, 355),(50,50), 15, (0,180,180))
        self.btnTop = Button("Fwd",(180, 295),(50,50), 15, (0,180,180))
        self.btnDown = Button("Back",(180, 415),(50,50), 15, (0,180,180))

        self.btnArmUp = Button("Arm Up",(40, 295),(50,50), 12, (200,180,180))
        self.btnArmMid = Button("Arm Mid",(40, 355),(50,50), 12, (200,180,180))
        self.btnArmDown = Button("Arm Down",(40, 415),(50,50), 10, (200,180,180))

        self.btnOff = Button("Off",(330, 295),(50,50), 15, (255,0,0))
        self.btnOn = Button("On",(330, 355),(50,50), 15, (0,255,0))

        self.btnModeChoose = Button("Mode Current",(450, 370),(150,35), 15, (0,0,0))
        self.btnModeSelect = Button("Change Mode",(450, 420),(150,35), 15, (0,0,255))

    def run_buttons(self):
        self.btnStop.show(self.display)
        self.btnLeft.show(self.display)
        self.btnRight.show(self.display)
        self.btnTop.show(self.display)
        self.btnDown.show(self.display)

        self.btnArmUp.show(self.display)
        self.btnArmMid.show(self.display)
        self.btnArmDown.show(self.display)

        self.btnOff.show(self.display)
        self.btnOn.show(self.display)

        self.btnModeChoose.show(self.display)
        self.btnModeSelect.show(self.display)

        if self.btnStop.click(pygame.event.get()) == True:
            print("Pressed")

    def main(self):
        going = True
        while going:
            self.get_background()
            self.get_text()
            self.set_values()
            self.get_camera()
            self.run_buttons()

            events = pygame.event.get()
            for e in events:
                if e.type == QUIT or (e.type == KEYDOWN and e.key == K_ESCAPE):
                    # close the camera safely
                    self.cam.stop()
                    going = False
            pygame.display.flip()

captureObj =  Capture()
captureObj.main()
