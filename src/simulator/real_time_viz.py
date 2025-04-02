import pygame as pg
from typing import Tuple, List
import numpy as np
from time import time, sleep
from pathlib import Path
from random import randint
from math import pi, radians, cos, sin, tan

BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 100, 255)
SKY_BLUE = (135, 206, 235)
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
        self.DASHBOARD_WIDTH = 1.5
        self.DASHBOARD_HEIGHT = 0.65
        
        # parameter that dictates distortion of screen as a function of long distance from ego veh
        # higher parameter has more distortion, 1 has no distortion
        self.MAX_DISTORTION: float = 15.0

        # max length, height, depth of visualizer (m)
        self.VIZ_LENGTH_M: float = 0.5
        self.VIZ_HEIGHT_M: float = 0.3
        self.MAX_DEPTH_M: float = 4.0

        # horizon params
        self.HORIZON_HEIGHT_PCT = 0.7
        self.HORIZON_HEIGHT_PX = self.pct_to_h_px(self.HORIZON_HEIGHT_PCT)
        
        self.moving_avg_speed = 0.0
        self.MOVING_AVG_WINDOW = 10

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
        self.LEAD_VEH_HEIGHT_M = 0.125
        
        # sprite objects
        self.car_sprite = None
        self.steering_wheel_sprite = None
        
        # fonts
        self.small_font = None
        self.med_font = None
        self.big_font = None

        # sample time and vehicle parameters
        self.dt = DT_VIZ
        
        self.gradient_surface = pg.Surface((200, 200), pg.SRCALPHA)
        
        self.sun_x = randint(200, self.WINDOW_WIDTH-100)
        self.clouds = [(randint(100, self.WINDOW_WIDTH - 100), randint(115, 160), randint(60, 120)) for _ in range(5)]
        
        # road drawing parameters
        self.plane_center_x = self.WINDOW_WIDTH // 2
        self.resolution = 1
        self.wall_height = 32
        self.plane_center = int(self.WINDOW_HEIGHT * (1-self.HORIZON_HEIGHT_PCT))
        fov = 60
        self.to_plane_dist = int((self.WINDOW_WIDTH / 2) / tan(radians(fov / 2)))

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
        self.font = pg.font.SysFont('Bodoni', int(0.05*self.WINDOW_HEIGHT))
        
        # car and steering wheel sprites
        self.car_sprite = pg.image.load(str(ASSET_PATH / '80s_car.png')).convert_alpha()
        # self.car_sprite = pg.transform.scale(self.car_sprite, (160, 82))
        self.steering_wheel_sprite = pg.image.load(str(ASSET_PATH / 'steering_wheel.png')).convert_alpha()
        self.steering_wheel_sprite = pg.transform.scale(self.steering_wheel_sprite, (self.WINDOW_WIDTH*0.4, self.WINDOW_WIDTH*0.4))
        
        # Ground and road sprites
        texture = pg.image.load(str(ASSET_PATH / 'stripes.png')).convert()
        self.stripes_texture = pg.transform.scale(texture, (self.WINDOW_WIDTH, texture.get_height()))
        self.road_texture = pg.image.load(str(ASSET_PATH / 'road3.png')).convert()
        self.road_width = self.road_texture.get_width()
        self.road_height = self.road_texture.get_height()

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

        lead_width_px = max(self.width_to_px(self.LEAD_VEH_WIDTH_M, lead_dist), 1)
        lead_height_px = max(self.height_to_px(self.LEAD_VEH_HEIGHT_M, lead_dist), 1)

        bottom_y = int((lead_dist / self.MAX_DEPTH_M) * self.HORIZON_HEIGHT_PX)
        bottom_y = min(bottom_y, self.WINDOW_HEIGHT * 0.6925)
        top_y = bottom_y + lead_height_px
        top_y = self.WINDOW_HEIGHT - top_y

        left_x = int(self.WINDOW_WIDTH / 2) - int(lead_width_px / 2)
                
        car_sprite = pg.transform.scale(self.car_sprite, (lead_width_px, lead_height_px))
        self.screen.blit(car_sprite, (left_x, top_y))

    def draw_road(self, ego_position: float) -> None:
        wall_bottom = self.WINDOW_HEIGHT
        while wall_bottom > self.plane_center + 1:
            wall_bottom -= self.resolution
            row = wall_bottom - self.plane_center
            straight_p_dist = (self.wall_height / row * self.to_plane_dist)
            to_floor_dist = (straight_p_dist)
            ray_y = int(-ego_position * 1000 - to_floor_dist)
            floor_y = (ray_y % self.road_height)
            slice_width = int(self.road_width / to_floor_dist * self.to_plane_dist)
            slice_x = (self.plane_center_x) - (slice_width // 2)
            row_slice = self.road_texture.subsurface(0, floor_y, self.road_width, 1)
            row_slice = pg.transform.scale(row_slice, (slice_width, self.resolution))
            self.screen.blit(self.stripes_texture, (0, wall_bottom), (0, floor_y, self.WINDOW_WIDTH, self.resolution))
            self.screen.blit(row_slice, (slice_x, wall_bottom))
            
    def draw_hud(self, ego_speed: float):
        self.moving_avg_speed = self.moving_avg_speed * (self.MOVING_AVG_WINDOW - 1)/self.MOVING_AVG_WINDOW + ego_speed / self.MOVING_AVG_WINDOW
        
        ellipse_rect = pg.Rect(int((self.WINDOW_WIDTH - self.WINDOW_WIDTH * self.DASHBOARD_WIDTH) / 2), 
                            int(self.WINDOW_HEIGHT * self.DASHBOARD_HEIGHT), 
                            int(self.WINDOW_WIDTH * self.DASHBOARD_WIDTH), 
                            int(self.WINDOW_HEIGHT * self.DASHBOARD_HEIGHT))

        lighter_ellipse_rect = pg.Rect(int((self.WINDOW_WIDTH - self.WINDOW_WIDTH * self.DASHBOARD_WIDTH) / 2), 
                            int(self.WINDOW_HEIGHT * self.DASHBOARD_HEIGHT) + 20, 
                            int(self.WINDOW_WIDTH * self.DASHBOARD_WIDTH), 
                            int(self.WINDOW_HEIGHT * self.DASHBOARD_HEIGHT))
        
        pg.draw.ellipse(self.screen, DARKGRAY, ellipse_rect)
        pg.draw.ellipse(self.screen, GRAY, lighter_ellipse_rect)
    
        CENTER = (int(self.WINDOW_WIDTH * 0.6), int(self.WINDOW_HEIGHT * 0.8))
        RADIUS = 70
        RED_RADIUS = 65
        NUM_TICKS = 5
        MAX_SPEED = 1  # Maximum speed the speedometer will display

        pg.draw.circle(self.screen, BLACK, CENTER, RADIUS, 5)

        for i in range(RED_RADIUS, 0, -1):
            alpha = int(255 * (i / RED_RADIUS))  # Transparency decreases outward
            faded_color = (*RED, alpha)
            pg.draw.circle(self.gradient_surface, faded_color, (RED_RADIUS, RED_RADIUS), i)

        self.screen.blit(self.gradient_surface, (CENTER[0] - RED_RADIUS, CENTER[1] - RED_RADIUS))
    
        # Draw speed lines and numbers
        for i in range(0, NUM_TICKS+1):
            speed = i * MAX_SPEED / NUM_TICKS
            angle = radians(-180 + (i / NUM_TICKS) * 180)
            x1 = CENTER[0] + RADIUS * 0.8 * cos(angle)
            y1 = CENTER[1] + RADIUS * 0.8 * sin(angle)
            x2 = CENTER[0] + RADIUS * cos(angle)
            y2 = CENTER[1] + RADIUS * sin(angle)
            pg.draw.line(self.screen, DARKGRAY, (x1, y1), (x2, y2), 3)
            
            # Draw numbers
            text_x = CENTER[0] + (RADIUS + 15) * cos(angle)
            text_y = CENTER[1] + (RADIUS + 15) * sin(angle)
            text_y -= 12
            if (i < NUM_TICKS / 2):
                text_x -= 25
            elif (i == NUM_TICKS / 2):
                text_x -= 15
            else:
                text_x -= 10
            text_surface = self.small_font.render(str(round(speed, 2)), True, BLACK)
            self.screen.blit(text_surface, (text_x, text_y))
        
        # Draw needle with a triangular shape
        needle_angle = -180 + (self.moving_avg_speed / MAX_SPEED) * 180  # Map speed to angle range
        needle_rad = radians(needle_angle)
        
        needle_x1 = CENTER[0] + RADIUS * 0.7 * cos(needle_rad)
        needle_y1 = CENTER[1] + RADIUS * 0.7 * sin(needle_rad)
        
        base_angle1 = needle_rad + radians(90)
        base_angle2 = needle_rad - radians(90)
        
        needle_x2 = CENTER[0] + 5 * cos(base_angle1)
        needle_y2 = CENTER[1] + 5 * sin(base_angle1)
        
        needle_x3 = CENTER[0] + 5 * cos(base_angle2)
        needle_y3 = CENTER[1] + 5 * sin(base_angle2)
        
        pg.draw.polygon(self.screen, BLACK, [(needle_x1, needle_y1), (needle_x2, needle_y2), (needle_x3, needle_y3)])
        
        # Display speed value
        speed_text = self.font.render(f"Speed: {round(self.moving_avg_speed, 2)} m/s", True, BLACK)
        self.screen.blit(speed_text, (CENTER[0] - 50, CENTER[1] + 80))
        
        self.screen.blit(self.steering_wheel_sprite, (self.WINDOW_WIDTH*0.05, self.WINDOW_HEIGHT*0.6))


    def display_stats(self, ego_speed: float, lead_speed: float, lead_dist: float) -> None:
        lead_speed_text = self.font.render(f'Lead Vehicle Speed: {lead_speed:.2f} m/s', False, WHITE)
        self.screen.blit(lead_speed_text, (0,20))
        lead_dist_text = self.font.render(f'Lead Vehicle Distance: {lead_dist:.2f} m', False, WHITE)
        self.screen.blit(lead_dist_text, (0,60))
        
    def draw_sky(self):
        # draw sky
        pg.draw.rect(self.surface, SKY_BLUE, (0, 0, self.WINDOW_WIDTH, self.WINDOW_HEIGHT - self.HORIZON_HEIGHT_PX))
        
        # Draw sun
        pg.draw.circle(self.surface, YELLOW, (self.sun_x, 180), 50)
        
        # Draw clouds
        for cx, cy, size in self.clouds:
            pg.draw.ellipse(self.surface, WHITE, (cx, cy, size, size // 2))
            pg.draw.ellipse(self.surface, WHITE, (cx - 20, cy + 10, size, size // 2))
            pg.draw.ellipse(self.surface, WHITE, (cx + 20, cy + 10, size, size // 2))

    def draw(self, ego_position: float, ego_speed: float, lead_speed: float, lead_dist: float) -> None:

        # draw background (grass and sky)
        self.surface.fill(GREEN)

        # draw road (grey polygon and white lines)
        self.draw_road(ego_position)

        self.draw_sky()

        # draw lead vehicle
        self.draw_lead_vehicle(lead_dist, RED)
        
        self.draw_hud(ego_speed)

        self.display_stats(ego_speed, lead_speed, lead_dist)

        pg.event.get()
        pg.display.update()

    def animate(self):
        
        while True:
            start = time()

            self.draw()

            end = time()

            elapsed = end - start
            wait_time = max(0, self.dt - elapsed)

            sleep(wait_time)
  
# Default scenario that does not require hil          
if __name__ == '__main__':        
    rt = RealTimeViz(dt=0.01)
    ego_speed = 0
    ego_distance = 0.0
    lead_distance = 2.0
    while True:
        ego_distance += ego_speed * 0.01
        if ego_speed < 1:
            ego_speed += 0.0005
        lead_distance += 0.0005
        rt.draw(
            ego_position=ego_distance, 
            ego_speed=ego_speed, 
            lead_speed=0.02,
            lead_dist=lead_distance,
        )