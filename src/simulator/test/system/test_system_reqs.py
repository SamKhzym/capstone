import pytest, sys
from pathlib import Path
import scipy.interpolate
from typing import List, Tuple
import numpy as np
import pytest
import matplotlib.pyplot as plt
import os

PROJECT_BASE = Path(__file__).parents[4]
SIMULATOR_PATH = PROJECT_BASE / 'src' / 'simulator'

DRIVE_CYCLE_DIR = SIMULATOR_PATH / 'drive_cycle'
ACC_DLL_PATH = PROJECT_BASE / 'src' / 'controller' / 'bin' / 'controller_shared.dll'

PLOT_PATH = Path(__file__).parent / 'plots'

sys.path.append(str(SIMULATOR_PATH))

from env_simulator import EnvironmentalSimulator, DT_SIM
from plant_model import PlantModel
from acc_wrapper import AccWrapper
from time import time

INIT_LEAD_DIST_M = 1.0

# plant model parameters
GAIN = 1/255
ZETA = 1.0
OMEGA_N = 10.0

class SilTester:

    def __init__(
        self,
        drive_cycle_path: str,
        set_speed: float = None,
        set_speed_schedule: List[List[float]] = None,
    ):
        
        self.drive_cycle_path = drive_cycle_path
        self.set_speed = set_speed
        self.set_speed_schedule = set_speed_schedule
        self.is_const_set_speed = self.set_speed_schedule is None

        if not self.is_const_set_speed:
            self.set_speed_schedule_func = scipy.interpolate.interp1d(
                self.set_speed_schedule[0],
                self.set_speed_schedule[1]
            )
            self.set_speed = self.set_speed_schedule_func(0)

        self.simulator = EnvironmentalSimulator(
            self.drive_cycle_path, 
            self.set_speed, 
            INIT_LEAD_DIST_M
        )

        self.final_time = self.simulator.times[-1]

        self.metric_names = [
            'time_s', 
            'set_speed_mps',
            'ego_speed_mps',
            'lead_speed_mps',
            'lead_dist_m',
            'act_req'
        ]
        self.metrics = {}

        self.reset()

    def reset(self):

        self.plant = PlantModel(GAIN, ZETA, OMEGA_N)
        self.acc = AccWrapper(str(ACC_DLL_PATH))
        self.ego_speed = 0
        self.act_req = 0

        for metric_name in self.metric_names:
            self.metrics[metric_name] = []

        self.acc.init_acc()

    def run(self):

        num_timesteps = int(self.final_time / DT_SIM)
        for ts in range(num_timesteps):

            # update set speed from schedule if necessary
            if not self.is_const_set_speed:
                self.set_speed = self.set_speed_schedule_func(ts * DT_SIM)

            # update metrics
            self.metrics['time_s'].append(ts * DT_SIM)
            self.metrics['set_speed_mps'].append(self.set_speed)
            self.metrics['act_req'].append(self.act_req)
            self.metrics['ego_speed_mps'].append(self.ego_speed)
            self.metrics['lead_speed_mps'].append(self.simulator.get_current_lead_speed())
            self.metrics['lead_dist_m'].append(self.simulator.get_rel_lead_distance())

            # step acc
            self.act_req = self.acc.step_acc(
                self.ego_speed,
                self.simulator.get_current_lead_speed(),
                self.simulator.get_set_speed(),
                self.simulator.get_rel_lead_distance()
            )

            # update plant model and get ego speed
            self.ego_speed = self.plant.step_plant(self.act_req)

            # update positions of vehicles in simulator
            self.simulator.step_simulator(self.ego_speed)

        self.calculate_metrics()

    def calculate_metrics(self):

        # numpy-ize the metric lists
        for metric_name, metric_list in self.metrics.items():
            self.metrics[metric_name] = np.array(metric_list)

        self.metrics['ego_accel_mps2'] = np.diff(self.metrics['ego_speed_mps']) / np.diff(self.metrics['time_s'])
        self.metrics['ego_accel_mps2'] = np.concatenate([[0.0], self.metrics['ego_accel_mps2']], axis=None)
        
        self.metrics['ego_jerk_mps3'] = np.diff(self.metrics['ego_accel_mps2']) / np.diff(self.metrics['time_s'])
        self.metrics['ego_jerk_mps3'] = np.concatenate([[0.0], self.metrics['ego_jerk_mps3']], axis=None)

        self.metrics['speed_error_mps'] = self.metrics['ego_speed_mps'] - np.min(np.array([self.metrics['set_speed_mps'], self.metrics['lead_speed_mps']]))

    def plot_metrics(self, plot_name):

        t = self.metrics['time_s']

        fig, ax = plt.subplots(5, 1, sharex=True)

        fig.suptitle(plot_name)
        fig.set_size_inches(18.5, 10.5)

        ax[0].plot(t, self.metrics['ego_speed_mps'], label="Ego Speed (m/s)")
        ax[1].plot(t, self.metrics['ego_accel_mps2'], label="Ego Accel (m/s^2)")
        ax[2].plot(t, self.metrics['ego_jerk_mps3'], label="Ego Jerk (m/s^3)")
        ax[3].plot(t, self.metrics['lead_dist_m'], label="Lead Dist (m)")
        ax[4].plot(t, self.metrics['act_req'], label="Act Req")

        for a in ax:
            a.legend()
            a.grid()

        plt.savefig(str(PLOT_PATH / f'{plot_name}.png'))

LV1_NO_LEAD_DC_PATH = DRIVE_CYCLE_DIR / 'no_lead.csv'
LV2_STD_HIGHWAY = DRIVE_CYCLE_DIR / 'hwfet.csv'
LV3_STD_CITY = DRIVE_CYCLE_DIR / 'udds.csv'
LV4_STEPWISE_VEL = DRIVE_CYCLE_DIR / 'stepwise.csv'

SS1_CONST_AVG = [[0, 1000000.0], [0.55, 0.55]]
SS2_CONST_MIN = [[0, 1000000.0], [0.3, 0.3]]
SS3_CONST_MAX = [[0, 1000000.0], [0.8, 0.8]]
SS4_STEPWISE = [[i*30.0 for i in range(100)], [0.8, 0.55, 0.3, 0.5]*25]

def get_test_matrix():
    test_matrix = []

    for i, lv in enumerate([LV1_NO_LEAD_DC_PATH, LV2_STD_HIGHWAY, LV3_STD_CITY, LV4_STEPWISE_VEL]):
        for j, ss in enumerate([SS1_CONST_AVG, SS2_CONST_MIN, SS3_CONST_MAX, SS4_STEPWISE]):
            test_matrix.append(pytest.param(str(lv), ss, id=f'system_test_lv{i+1}_ss{j+1}'))

    return test_matrix

TEST_MATRIX = get_test_matrix()

@pytest.mark.parametrize(
    ("lv", "ss"),
    TEST_MATRIX,
)
def test_suite(lv, ss):
    tester = SilTester(drive_cycle_path=lv, set_speed_schedule=ss)
    tester.run()
    tester.plot_metrics(os.environ.get('PYTEST_CURRENT_TEST').split('[')[1].split(']')[0])