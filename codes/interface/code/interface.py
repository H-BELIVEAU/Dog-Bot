import numpy as np
import pygame
import os


SIZE = 600, 600
FPS = 60


AX, AY = 42.5, 20.4
L1 = 45
L2 = 70
ALPHA = np.deg2rad(45)


SERVO_SPEED = np.deg2rad(100)/60.0
SCALE = 2.0
DECX, DECY = SIZE[0]/2, SIZE[1]/3


def normalize_angle(angle):
    angle = (angle+np.pi*2)%(np.pi*2)
    if angle >= np.pi:
        angle -= np.pi*2
    return angle

def world_to_screen(p):
    return p[0]*SCALE+DECX, -p[1]*SCALE+DECY

def screen_to_world(p):
    return (p[0]-DECX)/SCALE, (-p[1]+DECY)/SCALE


class Interface:
    def __init__(self):
        pygame.init()

        self.screen = pygame.display.set_mode(SIZE)
        pygame.display.set_caption("Interface - Dog Bot")
        self.clock = pygame.time.Clock()

        self.pts = {
            'O': np.array((0.0, 0.0)),
            'A': np.array((AX, AY)),
            'B': np.array((0.0, 0.0)),
            'C': np.array((0.0, 0.0)),
            'D': np.array((0.0, 0.0)),
            'E': np.array((0.0, 0.0)),
            'F': np.array((0.0, 0.0)),
            'M': np.array((0.0, 0.0)),
        }
        self.servoA = np.deg2rad(270)
        self.servoB = np.deg2rad(135)

        self.running = False
        self.keylistener = {}

        self.update_positions()

    def fetch_events(self):
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                self.running = False
                return
            elif e.type == pygame.KEYDOWN:
                if e.key == pygame.K_ESCAPE:
                    self.running = False
                    return
                self.keylistener[e.key] = True
            elif e.type == pygame.KEYUP:
                self.keylistener[e.key] = False

    def run(self):
        self.running = True
        while self.running:
            self.clock.tick(FPS)
            self.fetch_events()

            sA, sB = self.servoA, self.servoB
            if self.keylistener.get(pygame.K_a): sA += SERVO_SPEED
            if self.keylistener.get(pygame.K_z): sA -= SERVO_SPEED
            if self.keylistener.get(pygame.K_o): sB += SERVO_SPEED
            if self.keylistener.get(pygame.K_p): sB -= SERVO_SPEED

            if sA != self.servoA or sB != self.servoB:
                self.servoA = normalize_angle(sA)
                self.servoB = normalize_angle(sB)
                self.update_positions()

            self.draw()

    def update_positions(self):
        cosa, sina = np.cos(self.servoA), np.sin(self.servoA)
        cosb, sinb = np.cos(self.servoB), np.sin(self.servoB)
        cosc, sinc = np.cos(self.servoB+ALPHA), np.sin(self.servoB+ALPHA)
        self.pts['C'] = np.array((cosb*L1, sinb*L1))
        self.pts['B'] = self.pts['C']+self.pts['A']
        self.pts['F'] = np.array((cosa*L2, sina*L2))
        self.pts['D'] = np.array((cosc*L1, sinc*L1))
        self.pts['E'] = self.pts['D']+self.pts['F']
        self.pts['M'] = self.pts['F'] - 2*self.pts['D']*L2/L1

    def draw(self):
        def draw_line(a, b, color= (200, 200, 220)):
            pygame.draw.line(self.screen, color, world_to_screen(a), world_to_screen(b), width=3)
        def draw_point(a, color= (200, 100, 100)):
            pygame.draw.circle(self.screen, color, world_to_screen(a), 5)

        self.screen.fill((40, 40, 60))
        draw_line(screen_to_world((0.0, DECY)), screen_to_world((SIZE[0], DECY)), (100, 100, 120))
        draw_line(screen_to_world((DECX, 0.0)), screen_to_world((DECX, SIZE[1])), (100, 100, 120))

        draw_line(self.pts['O'], self.pts['F'])
        draw_line(self.pts['O'], self.pts['D'])
        draw_line(self.pts['D'], self.pts['C'])
        draw_line(self.pts['A'], self.pts['B'])
        draw_line(self.pts['C'], self.pts['B'])
        draw_line(self.pts['C'], self.pts['O'])
        draw_line(self.pts['D'], self.pts['E'])
        draw_line(self.pts['E'], self.pts['F'])
        draw_line(self.pts['F'], self.pts['M'])
        draw_point(self.pts['O'])
        draw_point(self.pts['A'])
        draw_point(self.pts['B'])
        draw_point(self.pts['C'])
        draw_point(self.pts['D'])
        draw_point(self.pts['E'])
        draw_point(self.pts['F'])
        draw_point(self.pts['M'])

        pygame.display.flip()


if __name__ == "__main__":
    app = Interface()
    app.run()
