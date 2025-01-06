import pygame as pg
from typing import Tuple, List

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
        
        # window height and length (pixels)
        self.WINDOW_WIDTH: int = 800
        self.WINDOW_HEIGHT: int = 600

        # max distance of visualizer (m)
        self.MAX_DEPTH_M: float = 50.0
        
        # parameter that dictates distortion of screen as a function of long distance from ego veh
        # higher parameter has more distortion, 1 has no distortion
        self.MAX_DISTORTION: float = 1.1

        # percentage to pixel ratios
        self.VIZ_LENGTH_M: float = 0.5
        self.VIZ_HEIGHT_M: float = 1.0

        # visualizer params
        self.HORIZON_HEIGHT_PCT = 0.6
        self.HORIZON_HEIGHT_PX = self.pct_to_h_px(self.HORIZON_HEIGHT_PCT)

        self.LANE_WIDTH_M = 0.3
        self.LINE_WIDTH_M = 0.02
        self.NUM_LANES = 3

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

    def pct_to_h_px(self, pct: float) -> int:
        '''Converts a percentage of the screen height into a number of pixels'''
        return int(pct * self.WINDOW_HEIGHT)

    def pct_to_w_px(self, pct: float) -> int:
        '''Converts a percentage of the screen width into a number of pixels'''
        return int(pct * self.WINDOW_WIDTH)

    def distance_to_distortion_pct(self, distance_m: float) -> float:
        '''Converts a longitudinal distance from ego vehicle to a percentage of distortion'''
        
        # 1 distortion at 0 distance, max distortion at max distance
        slope = (1 / self.MAX_DISTORTION) / self.MAX_DEPTH_M
        return 1 - (slope * distance_m)

    def height_to_px(self, length_m: float, distance_m: float = 0) -> int:
        '''Converts a longitudinal length to a number of pixels based on distance from lead vehicle'''
        distortion_pct = self.distance_to_distortion_pct(distance_m)
        return self.pct_to_h_px(distortion_pct * (length_m / self.VIZ_HEIGHT_M))

    def width_to_px(self, length_m: float, distance_m: float) -> int:
        '''Converts a lateral length to a number of pixels based on distance from lead vehicle'''
        distortion_pct = self.distance_to_distortion_pct(distance_m)
        return self.pct_to_w_px(distortion_pct * (length_m / self.VIZ_LENGTH_M))
    
    def flip_coords(self, coords: List[Tuple[float, float]]) -> List[Tuple[float, float]]:
        '''Flips a list of coords along the x-axis'''
        new_coords = []
        for i in range(len(coords)):
            new_coords.append((coords[i][0], self.WINDOW_HEIGHT - coords[i][1]))

        return new_coords
    
    def draw_offset_center_polygon(self, width_m: float, start_depth_m, depth_m: float, offset_m: float, color: Tuple[float, float, float]) -> None:

        # determine y-position for close and far lines based on start depth and line depth
        close_y = int((start_depth_m / self.MAX_DEPTH_M) * self.HORIZON_HEIGHT_PX)
        far_y = close_y + int((depth_m / self.MAX_DEPTH_M) * self.HORIZON_HEIGHT_PX)

        # determine width for close and far lines based on start depth and line depth
        close_width = self.width_to_px(width_m, start_depth_m)
        far_width = self.width_to_px(width_m, start_depth_m + depth_m)

        # determine offset from center of screen for close and far lines
        close_offset = self.width_to_px(offset_m, start_depth_m)
        far_offset = self.width_to_px(offset_m, start_depth_m + depth_m)

        # use widths and offsets to determine x-values
        close_left_x = int(self.WINDOW_WIDTH / 2) - int(close_width / 2) + close_offset
        close_right_x = int(self.WINDOW_WIDTH / 2) + int(close_width / 2) + close_offset
        far_left_x = int(self.WINDOW_WIDTH / 2) - int(far_width / 2) + far_offset
        far_right_x = int(self.WINDOW_WIDTH / 2) + int(far_width / 2) + far_offset

        # create point list and flip it to conform to pygamr coord system (+ve y is down)
        points = self.flip_coords([
            (close_left_x, close_y), (close_right_x, close_y), (far_right_x, far_y), (far_left_x, far_y)
        ])

        pg.draw.polygon(self.surface, color, points)

    def draw_road(self):

        # draw road
        self.draw_offset_center_polygon(self.LANE_WIDTH_M * self.NUM_LANES, 0, self.MAX_DEPTH_M, 0, GRAY)

        # draw lane lines
        self.draw_offset_center_polygon(self.LINE_WIDTH_M, 0, self.MAX_DEPTH_M, self.LANE_WIDTH_M / 2, YELLOW)
        self.draw_offset_center_polygon(self.LINE_WIDTH_M, 0, self.MAX_DEPTH_M, -self.LANE_WIDTH_M / 2, YELLOW)

    def draw(self):

        # draw background (grass and sky)
        self.surface.fill(GREEN)
        pg.draw.rect(self.surface, BLUE, (0, 0, self.WINDOW_WIDTH, self.WINDOW_HEIGHT - self.HORIZON_HEIGHT_PX))

        self.draw_road()

        pg.event.get()
        pg.display.update()
        
rt = RealTimeViz()
rt.init_window()

while True:
    rt.draw()