import numpy as np, pandas as pd
import scipy.interpolate as interpolate

DT_SIM = 0.01
MPS_TO_MPH = 2.23694

class EnvironmentalSimulator:

    def __init__(self, lead_drive_cycle_path: str, set_speed: float, init_lead_dist: float) -> None:

        drive_cycle = pd.read_csv(lead_drive_cycle_path)

        self.times = drive_cycle.iloc[:,0].values.astype(np.float32)
        self.lead_speeds = drive_cycle.iloc[:,1].values.astype(np.float32)
        self.set_speed = set_speed

        self.ego_position = 0
        self.lead_position = init_lead_dist
        self.time = 0

        self.dt = DT_SIM

        self.scale_drive_cycle()
        self.lead_speed_interpolator = interpolate.interp1d(self.times, self.lead_speeds)
        
    def scale_drive_cycle(self) -> None:

        # conversion: 1mph -> 0.01 m/s
        SPEED_SCALE_FACTOR = MPS_TO_MPH / 100
        self.lead_speeds *= SPEED_SCALE_FACTOR
        
    def get_current_lead_speed(self) -> float:
        return self.lead_speed_interpolator(self.time)

    def get_lead_speed(self, curr_time: float) -> float:
        return self.lead_speed_interpolator(curr_time)
    
    def get_set_speed(self) -> float:
        return self.set_speed
    
    def update_distance_travelled(self, ego_speed: float, lead_speed: float) -> None:
        self.ego_position += self.dt * ego_speed
        self.lead_position += self.dt * lead_speed

    def get_rel_lead_distance(self) -> float:
        return self.lead_position - self.ego_position
    
    def step_simulator(self, ego_speed: float) -> None:

        # get lead speed
        lead_speed = self.get_lead_speed(self.time)

        # update positions of vehicles in sim
        self.update_distance_travelled(ego_speed, lead_speed)
        
        # update sim time
        self.time += self.dt

