import pygame as pg
from typing import Tuple, List
import numpy as np
from time import time, sleep
from pathlib import Path

BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 100, 255)
YELLOW = (255, 255, 0)
DARKGRAY = (59,59,59)
VERYDARKGRAY = (34,34,34)
GRAY = (100, 100, 100)
WHITE = (255, 255, 255)

DT_VIZ = 0.02

ASSET_PATH = Path(__file__).parent / 'assets'      

class RealTimeViz:

    def __init__(self, dt: float = DT_VIZ):
        
        # window height and length (pixels)
        self.WINDOW_WIDTH: int = 1500
        self.WINDOW_HEIGHT: int = 1000
        
        # parameter that dictates distortion of screen as a function of long distance from ego veh
        # higher parameter has more distortion, 1 has no distortion
        self.MAX_DISTORTION: float = 6.0

        # max length, height, depth of visualizer (m)
        self.VIZ_LENGTH_M: float = 0.5
        self.VIZ_HEIGHT_M: float = 0.3
        self.MAX_DEPTH_M: float = 2.0

        # horizon params
        self.HORIZON_HEIGHT_PCT = 0.7
        self.HORIZON_HEIGHT_PX = self.pct_to_h_px(self.HORIZON_HEIGHT_PCT)

        # lanes params
        self.LANE_WIDTH_M = 0.3
        self.NUM_LANES = 3
        
        # lane line params
        self.LINE_WIDTH_M = 0.02
        self.LINE_LENGTH_M = 0.2
        self.LINE_SPACING_M = self.LINE_LENGTH_M

        # lead vehicle params
        self.LEAD_VEH_WIDTH_M = 0.25
        self.LEAD_VEH_LENGTH_M = 0.3
        self.LEAD_VEH_HEIGHT_M = 0.2
        
        # sprite objects
        self.car_sprite = None
        self.steering_wheel_sprite = None
        
        # fonts
        self.small_font = None
        self.med_font = None
        self.big_font = None

        # sample time and vehicle parameters
        self.dt = DT_VIZ

        self.init_window()

    def init_window(self) -> None:
        pg.init()
        pg.font.init()
        pg.display.set_caption("Real-Time Visualizer")
        self.clock = pg.time.Clock()
        self.surface = pg.display.set_mode((self.WINDOW_WIDTH, self.WINDOW_HEIGHT))
        self.screen = pg.display.set_mode((self.WINDOW_WIDTH, self.WINDOW_HEIGHT))
        
        # initialize fonts
        font_path = str(ASSET_PATH / 'display_font.ttf')
        self.small_font = pg.font.Font(font_path, int(0.025*self.WINDOW_HEIGHT))
        self.med_font = pg.font.Font(font_path, int(0.05*self.WINDOW_HEIGHT))
        self.big_font = pg.font.Font(font_path, int(0.075*self.WINDOW_HEIGHT))
        self.font = pg.font.SysFont('Comic Sans MS', int(0.05*self.WINDOW_HEIGHT))
        
        # car and steering wheel sprites
        self.car_sprite = pg.image.load(str(ASSET_PATH / 'car.webp')).convert_alpha()
        self.steering_wheel_sprite = pg.image.load(str(ASSET_PATH / 'steering_wheel.png')).convert_alpha()
        self.steering_wheel_sprite = pg.transform.scale(self.steering_wheel_sprite, (self.WINDOW_WIDTH*0.4, self.WINDOW_WIDTH*0.4))

    def pct_to_h_px(self, pct: float) -> int:
        '''Converts a percentage of the screen height into a number of pixels'''
        return int(pct * self.WINDOW_HEIGHT)

    def pct_to_w_px(self, pct: float) -> int:
        '''Converts a percentage of the screen width into a number of pixels'''
        return int(pct * self.WINDOW_WIDTH)

    def distance_to_distortion_pct(self, distance_m: float) -> float:
        '''Converts a longitudinal distance from ego vehicle to a percentage of distortion'''
        
        # 1 distortion at 0 distance, max distortion at max distance
        slope = (1 / self.MAX_DISTORTION - 1) / self.MAX_DEPTH_M
        return 1 + (slope * distance_m)

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
    
    def draw_offset_center_polygon(
            self, 
            width_m: float, 
            start_depth_m: float, 
            depth_m: float, 
            lat_offset_m: float, 
            color: Tuple[float, float, float], 
            vert_offset_m: float = 0) -> None:

        # determine y-position for close and far lines based on start depth and line depth
        close_y = int((start_depth_m / self.MAX_DEPTH_M) * self.HORIZON_HEIGHT_PX) + self.height_to_px(vert_offset_m)
        far_y = close_y + int((depth_m / self.MAX_DEPTH_M) * self.HORIZON_HEIGHT_PX)

        # determine width for close and far lines based on start depth and line depth
        close_width = self.width_to_px(width_m, start_depth_m)
        far_width = self.width_to_px(width_m, start_depth_m + depth_m)

        # determine offset from center of screen for close and far lines
        close_offset = self.width_to_px(lat_offset_m, start_depth_m)
        far_offset = self.width_to_px(lat_offset_m, start_depth_m + depth_m)

        # use widths and offsets to determine x-values
        close_left_x = int(self.WINDOW_WIDTH / 2) - int(close_width / 2) + close_offset
        close_right_x = int(self.WINDOW_WIDTH / 2) + int(close_width / 2) + close_offset
        far_left_x = int(self.WINDOW_WIDTH / 2) - int(far_width / 2) + far_offset
        far_right_x = int(self.WINDOW_WIDTH / 2) + int(far_width / 2) + far_offset

        # create point list and flip it to conform to pygame coord system (+ve y is down)
        points = self.flip_coords([
            (close_left_x, close_y), (close_right_x, close_y), (far_right_x, far_y), (far_left_x, far_y)
        ])

        pg.draw.polygon(self.surface, color, points)

    def draw_lead_vehicle(self, lead_dist: float, color: Tuple[float, float, float]) -> None:

        lead_width_px = self.width_to_px(self.LEAD_VEH_WIDTH_M, lead_dist)
        lead_height_px = self.height_to_px(self.LEAD_VEH_HEIGHT_M, lead_dist)

        bottom_y = int((lead_dist / self.MAX_DEPTH_M) * self.HORIZON_HEIGHT_PX)
        top_y = bottom_y + lead_height_px
        top_y = self.WINDOW_HEIGHT - top_y

        left_x = int(self.WINDOW_WIDTH / 2) - int(lead_width_px / 2)

        # pg.draw.rect(self.surface, color, (left_x, top_y, lead_width_px, lead_height_px))
        # self.draw_offset_center_polygon(self.LEAD_VEH_WIDTH_M, lead_dist, self.LEAD_VEH_LENGTH_M, 0, YELLOW)
        
        car_sprite = pg.transform.scale(self.car_sprite, (lead_width_px, lead_height_px))
        self.screen.blit(car_sprite, (left_x, top_y))

    def draw_road(self, ego_position: float) -> None:

        # draw road
        self.draw_offset_center_polygon(self.LANE_WIDTH_M * self.NUM_LANES, 0, self.MAX_DEPTH_M, 0, GRAY)

        # draw lane lines
        lane_line_offset = -ego_position % (self.LINE_LENGTH_M + self.LINE_SPACING_M)
        for line_start in np.arange(lane_line_offset, self.MAX_DEPTH_M , self.LINE_LENGTH_M + self.LINE_SPACING_M):
            self.draw_offset_center_polygon(self.LINE_WIDTH_M, line_start, self.LINE_LENGTH_M, self.LANE_WIDTH_M / 2, WHITE)
            self.draw_offset_center_polygon(self.LINE_WIDTH_M, line_start, self.LINE_LENGTH_M, -self.LANE_WIDTH_M / 2, WHITE)
            
    def draw_hud(self, ego_speed: float, lead_speed: float, lead_dist: float):
        
        # draw left gray pillar of car interior
        pg.draw.polygon(self.surface, VERYDARKGRAY, 
            [(0,0), 
             (0, self.WINDOW_HEIGHT), 
             (self.WINDOW_WIDTH*0.15, self.WINDOW_HEIGHT),
             (self.WINDOW_WIDTH*0.00, 0)])
        
        # draw right gray pillar of car interior
        pg.draw.polygon(self.surface, VERYDARKGRAY, 
            [(self.WINDOW_WIDTH,0), 
             (self.WINDOW_WIDTH, self.WINDOW_HEIGHT), 
             (self.WINDOW_WIDTH*(1-0.15), self.WINDOW_HEIGHT),
             (self.WINDOW_WIDTH*(1-0.00), 0)])
        
        # draw top and bottom of car interior
        pg.draw.rect(self.surface, VERYDARKGRAY, ((0, 0, self.WINDOW_WIDTH, self.WINDOW_HEIGHT*0.1)))
        pg.draw.rect(self.surface, VERYDARKGRAY, ((0, self.WINDOW_HEIGHT*(1 - 0.25), self.WINDOW_WIDTH, self.WINDOW_HEIGHT*0.25)))
        
        # draw lighter shade for car dashboard and steering wheel
        pg.draw.rect(self.surface, DARKGRAY, ((0, self.WINDOW_HEIGHT*(1 - 0.2), self.WINDOW_WIDTH, self.WINDOW_HEIGHT*0.2)))
        self.screen.blit(self.steering_wheel_sprite, (self.WINDOW_WIDTH*0.05, self.WINDOW_HEIGHT*0.6))
        
        ego_speed_text = self.med_font.render(f'{ego_speed:.2f} m/s', False, (0, 0, 0))
        lead_speed_text = self.med_font.render(f'{lead_speed:.2f} m/s', False, (0, 0, 0))
        lead_dist_text = self.med_font.render(f'{lead_dist:.2f} m', False, (0, 0, 0))
        
        # draw data displays - not working :(
        # pg.draw.rect(self.surface, VERYDARKGRAY, 
        #     ((int(self.WINDOW_WIDTH*0.5), int(self.WINDOW_HEIGHT*(1 - 0.175)), int(self.WINDOW_HEIGHT*0.2), int(self.WINDOW_HEIGHT*0.15))), 
        #     border_radius=int(self.WINDOW_HEIGHT*0.01))
        
        # pg.draw.rect(self.surface, BLACK, 
        #     ((int(self.WINDOW_WIDTH*0.5), int(self.WINDOW_HEIGHT*(1 - 0.175)), int(self.WINDOW_HEIGHT*0.2), int(self.WINDOW_HEIGHT*0.15))), 
        #     border_radius=int(self.WINDOW_HEIGHT*0.01))
        
        # self.screen.blit(ego_speed_text, (self.WINDOW_WIDTH*0.5, self.WINDOW_HEIGHT*0.8))
        # self.screen.blit(lead_speed_text, (self.WINDOW_WIDTH*0.6, self.WINDOW_HEIGHT*0.8))
        # self.screen.blit(lead_dist_text, (self.WINDOW_WIDTH*0.7, self.WINDOW_HEIGHT*0.8))

    def display_stats(self, ego_speed: float, lead_speed: float, lead_dist: float) -> None:
        ego_speed_text = self.font.render(f'Ego Vehicle Speed: {ego_speed:.2f} m/s', False, WHITE)
        lead_speed_text = self.font.render(f'Lead Vehicle Speed: {lead_speed:.2f} m/s', False, WHITE)
        lead_dist_text = self.font.render(f'Lead Vehicle Distance: {lead_dist:.2f} m', False, WHITE)
        self.screen.blit(ego_speed_text, (0,0))
        self.screen.blit(lead_speed_text, (0,30))
        self.screen.blit(lead_dist_text, (0,60))

    def draw(self, ego_position: float, ego_speed: float, lead_speed: float, lead_dist: float) -> None:

        # draw background (grass and sky)
        self.surface.fill(GREEN)

        # draw road (grey polygon and white lines)
        self.draw_road(ego_position)

        # draw sky
        pg.draw.rect(self.surface, BLUE, (0, 0, self.WINDOW_WIDTH, self.WINDOW_HEIGHT - self.HORIZON_HEIGHT_PX))

        # draw lead vehicle
        self.draw_lead_vehicle(lead_dist, RED)
        
        self.draw_hud(ego_speed, lead_speed, lead_dist)

        self.display_stats(ego_speed, lead_speed, lead_dist)

        pg.event.get()
        pg.display.update()

    def animate(self):
        
        while True:
            start = time()

            self.update()
            self.draw()

            end = time()

            elapsed = end - start
            wait_time = max(0, self.dt - elapsed)

            sleep(wait_time)
        
# rt = RealTimeViz(dt=0.01)
# rt.init_window()
# rt.animate()