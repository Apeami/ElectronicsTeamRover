import pygame

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
