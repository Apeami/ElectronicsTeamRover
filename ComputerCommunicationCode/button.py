import pygame

class Button:
    """Create a button, then blit the surface in the while loop"""

    def __init__(self, text,  pos, size, font, bg):
        self.x, self.y = pos
        self.font = pygame.font.SysFont("Arial", font)
        self.size = size
        self.textCon = text
        self.bg=bg
        self.hover = self.subtract(self.bg,50)
        self.press = self.subtract(self.bg,100)
        self.change_text( text = self.textCon, clr=self.bg)

    def change_text(self, text, clr=(0,0,0)):
        """Change the text whe you click"""
        self.text = self.font.render(text, 1, pygame.Color("White"))

        self.surface = pygame.Surface(self.size)
        self.surface.fill(clr)
        self.surface.blit(self.text, (self.size[0]/2-self.text.get_size()[0]/2, self.size[1]/2-self.text.get_size()[1]/2))
        self.rect = pygame.Rect(self.x, self.y, self.size[0], self.size[1])

    def show(self,screen):
        screen.blit(self.surface, (self.x, self.y))

    def subtract(self, color, rate):
        tempColor=[0,0,0]
        for i in range(0,3):
            tempColor[i] = rate+color[i]
            if tempColor[i]>255:
                tempColor[i] -= 255
        return (tempColor[0],tempColor[1],tempColor[2])

    def click(self, event):
        x, y = pygame.mouse.get_pos()
        if self.rect.collidepoint(x, y):
            for e in event:
                if e.type == pygame.MOUSEBUTTONDOWN:
                    #click
                    self.change_text( text = self.textCon, clr=self.press)
                    if pygame.mouse.get_pressed()[0]:
                        return True
                else:
                    #hover
                    self.change_text( text = self.textCon, clr=self.hover)
        else:
            #None
            self.change_text( text = self.textCon, clr=self.bg)
        return False
