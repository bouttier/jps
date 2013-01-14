#!/usr/bin/env python2
from PIL import Image
import jps
import sys
import pygame


class Render:
    def __init__(self, dim, imgName):
        self.dim = dim
        mapImage = Image.open(imgName)
        self.size = mapImage.size
        self.res = (self.size[0] * self.dim[0], self.size[1] * self.dim[1])
        self.obstacle = None


        width, height = mapImage.size
        self.map = jps.Map((width, height))

        voidColor = mapImage.getpixel((1,1))
        count = 0
        for p in mapImage.getdata():
        	x,y = count % width, count // width
        	if p == voidColor:
        		self.map.remove((x, y))
        	else:
        		self.map.add((x, y))
        	count += 1

    def clean(self):
        self.start = None
        self.end = None
        self.solution = None
        pygame.draw.rect(self.screen, 0xffffff, pygame.Rect(0, 0,
            self.res[0], self.res[1]))
        for y in range(0, self.res[1], self.dim[1]):
            pygame.draw.line(self.screen, 0x888888, (0, y), (self.res[0], y))
        for x in range(0, self.res[0], self.dim[0]):
            pygame.draw.line(self.screen, 0x888888, (x, 0), (x, self.res[1]))
        pygame.display.flip()

    def refresh(self):
        self.clean()
        for x in range(0, self.size[0]):
            for y in range(0, self.size[1]):
                if not self.map.walkable((x, y)):
                    self.draw_rect((x, y), 0x000000)
        pygame.display.flip()

    def run(self):
        self.screen = pygame.display.set_mode(self.res)
        self.refresh()

        while True:
            ev = pygame.event.wait()
            if ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE: break
            if ev.type == pygame.QUIT: break

            posPixel = pygame.mouse.get_pos() # position de la souris
            pos = ((posPixel[0]) / self.dim[0], (posPixel[1]) / self.dim[1])

            #print("Position - x: " + str(pos[0]) + ", y: " + str(pos[1]))

            if ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 1:
                self.obstacle = self.map.walkable(pos)
            if ev.type == pygame.MOUSEBUTTONUP and ev.button == 1:
                self.obstacle = None

            if self.obstacle != None and self.obstacle == self.map.walkable(pos) \
                    and not pos == self.start and not pos == self.end:
                if self.obstacle:
                    color = 0x0
                    self.map.add(pos)
                else:
                    color = 0xffffff
                    self.map.remove(pos)
                self.draw_rect(pos, color)
                pygame.display.flip()

            if ev.type == pygame.MOUSEBUTTONUP and ev.button == 3 \
                    and self.obstacle == None and self.map.walkable(pos):
                if self.start == None:
                    self.start = pos
                    self.draw_rect(pos, 0x00ff00)
                    pygame.display.flip()
                elif self.end == None:
                    self.end = pos
                    self.draw_rect(pos, 0xff0000)
                    print("Compute path from %s to %s" %(self.start, self.end))
                    self.solution = jps.compute(self.map, self.start, self.end)
                    self.print_solution()
                    self.draw_solution()
                else:
                    self.refresh()
                    self.start = pos
                    self.draw_rect(pos, 0x00ff00)
                    pygame.display.flip()
            elif ev.type == pygame.MOUSEBUTTONUP and ev.button == 2:
                self.clean()
                self.map = jps.Map(self.size)

    def draw_rect(self, pos, color):
        pygame.draw.rect(self.screen, color,
            pygame.Rect(pos[0]*self.dim[0]+1, pos[1]*self.dim[1]+1,
                self.dim[0]-1, self.dim[1]-1))

    def print_solution(self):
        print(self.solution)

    def draw_solution(self):
        prev = None
        for i in self.solution:
            if not prev == None:
                ax = prev[0]*self.dim[0]+(self.dim[0]/2)
                ay = prev[1]*self.dim[1]+(self.dim[1]/2)
                bx = i[0]*self.dim[0]+(self.dim[0]/2)
                by = i[1]*self.dim[1]+(self.dim[1]/2)
                pygame.draw.line(self.screen, 0xff0000, (ax, ay), (bx, by))
            prev = i
        pygame.display.flip()
if __name__ == "__main__":
	if len(sys.argv) < 2:
		print "\nUtilisation:\n\t"+str(sys.argv[0])+" map\n"
		sys.exit(1)
	
	render = Render((8,8), sys.argv[1])
	render.run()
