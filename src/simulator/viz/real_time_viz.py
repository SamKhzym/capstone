import pygame as pg

BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
GRAY = (100, 100, 100)
WHITE = (255, 255, 255)

class Vehicle:

    def __init__(self, pos_m = 0, speed_mps = 0):
        self.speed_mps = speed_mps
        self.pos_m = pos_m

    def update(self, dt):
        self.pos_m += self.speed_mps * dt


class RealTimeViz:

    def __init__(self, dt = 0.02):
        
        self.WINDOW_WIDTH = 800
        self.WINDOW_HEIGHT = 600

        self.dt = dt
        self.ego_vehicle = Vehicle(pos_m=0)
        self.lead_vehicle = Vehicle(pos_m=50)

    def update(self):
        self.ego_vehicle.update(self.dt)

    def init_window(self):
        pg.init()
        pg.display.set_caption("Real-Time Visualizer")
        self.clock = pg.time.Clock()
        self.surface = pg.display.set_mode((self.WINDOW_WIDTH, self.WINDOW_HEIGHT))

    def draw_road(self):

        ROAD_WIDTH = 300
        LINE_WIDTH = 10
        LINE_HEIGHT = 50
        LINE_SPACING = LINE_HEIGHT // 2

        pg.draw.rect(self.surface, BLACK, (self.WINDOW_WIDTH / 2 - (ROAD_WIDTH + 6) / 2, 0, ROAD_WIDTH + 6, self.WINDOW_WIDTH))
        pg.draw.rect(self.surface, GRAY, (self.WINDOW_WIDTH / 2 - ROAD_WIDTH / 2, 0, ROAD_WIDTH, self.WINDOW_WIDTH))

        for i in range(self.WINDOW_HEIGHT // (LINE_WIDTH + LINE_SPACING) + 1):
            pg.draw.rect(self.surface, YELLOW, (self.WINDOW_WIDTH / 2 - LINE_WIDTH / 2, i*(LINE_HEIGHT+LINE_SPACING), LINE_WIDTH, LINE_HEIGHT))

    def draw(self):

        self.surface.fill(GREEN)
        self.draw_road()

        pg.event.get()
        pg.display.update()
        
rt = RealTimeViz()
rt.init_window()

while True:
    rt.draw()