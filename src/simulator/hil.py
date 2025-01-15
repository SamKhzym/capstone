from env_simulator import EnvironmentalSimulator, DT_SIM
from real_time_viz import RealTimeViz, DT_VIZ
from pathlib import Path
from time import time

# pathing stuff
PROJECT_BASE = Path(__file__).parents[1]
DRIVE_CYCLE_DIR = PROJECT_BASE / 'simulator' / 'drive_cycle'

# simulation parameters
DRIVE_CYCLE_NAME = 'hwfet.csv'
SET_SPEED_MPS = 0.85
INIT_LEAD_DIST_M = 1.0

def main():

    dt = DT_SIM
    start_time = time()
    elapsed_time = 0

    drive_cycle_path = str(DRIVE_CYCLE_DIR / DRIVE_CYCLE_NAME)
    simulator = EnvironmentalSimulator(drive_cycle_path, SET_SPEED_MPS, INIT_LEAD_DIST_M)
    viz = RealTimeViz()

    doViz = True
    while True:

        # loop around until enough time has passed (sample time from last execution)
        while (elapsed_time > 0 and (time() - (start_time + elapsed_time)) < dt): pass
        elapsed_time = time() - start_time

        # TODO: GET EGO SPEED FROM MICROCONTROLLER
        ego_speed = 0.2

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

        # TODO: transmit stuff to microcontroller

if __name__ == '__main__':
    main()