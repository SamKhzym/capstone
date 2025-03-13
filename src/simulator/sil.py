from env_simulator import EnvironmentalSimulator, DT_SIM
from real_time_viz import RealTimeViz, DT_VIZ
from plant_model import PlantModel
from acc_wrapper import AccWrapper
from pathlib import Path
from time import time

# pathing stuff
PROJECT_BASE = Path(__file__).parents[1]
DRIVE_CYCLE_DIR = PROJECT_BASE / 'simulator' / 'drive_cycle'
ACC_DLL_PATH = PROJECT_BASE / 'controller' / 'bin' / 'controller_shared.dll'

# simulation parameters
DRIVE_CYCLE_NAME = 'hwfet.csv'
SET_SPEED_MPS = 0.85
INIT_LEAD_DIST_M = 1.0

# plant model parameters
GAIN = 1/255
ZETA = 1.0
OMEGA_N = 10.0

def main():

    dt = DT_SIM
    start_time = time()
    elapsed_time = 0
    ego_speed = 0

    drive_cycle_path = str(DRIVE_CYCLE_DIR / DRIVE_CYCLE_NAME)
    simulator = EnvironmentalSimulator(drive_cycle_path, SET_SPEED_MPS, INIT_LEAD_DIST_M)
    plant = PlantModel(GAIN, ZETA, OMEGA_N)
    viz = RealTimeViz()
    acc = AccWrapper(str(ACC_DLL_PATH))

    # initialize acc controller
    acc.init_acc()

    doViz = True
    while True:

        # loop around until enough time has passed (sample time from last execution)
        while (elapsed_time > 0 and (time() - (start_time + elapsed_time)) < dt): pass
        elapsed_time = time() - start_time

        act_req = acc.step_acc(
            ego_speed,
            simulator.get_current_lead_speed(),
            simulator.get_set_speed(),
            simulator.get_rel_lead_distance()
        )

        # update plant model and get ego speed
        ego_speed = plant.step_plant(act_req)

        # update positions of vehicles in simulator
        simulator.step_simulator(ego_speed)

        # invoke visualizer every other timestep (dt_viz = 2*dt_sim)
        doViz = not doViz
        if doViz:
            viz.draw(
                simulator.ego_position, 
                ego_speed, 
                simulator.get_current_lead_speed(),
                simulator.get_rel_lead_distance()
            )

if __name__ == '__main__':
    main()