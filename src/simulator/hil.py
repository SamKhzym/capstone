from env_simulator import EnvironmentalSimulator, DT_SIM
from client_socket import Socket
from real_time_viz import RealTimeViz, DT_VIZ
from pathlib import Path
from time import time
import sys

# pathing stuff
PROJECT_BASE = Path(__file__).parents[1]
DRIVE_CYCLE_DIR = PROJECT_BASE / 'simulator' / 'drive_cycle'

# simulation parameters
DRIVE_CYCLE_NAME = 'hwfet.csv'
SET_SPEED_MPS = 0.85
INIT_LEAD_DIST_M = 1.0
VIZ_ENABLE = True

# Ethernet connection params
HOST = "fd12:3456:789a::1"
PORT = 4242

def main():

    dt = DT_SIM
    start_time = time()
    elapsed_time = 0
    ego_speed = 0.0

    drive_cycle_path = str(DRIVE_CYCLE_DIR / DRIVE_CYCLE_NAME)
    simulator = EnvironmentalSimulator(drive_cycle_path, SET_SPEED_MPS, INIT_LEAD_DIST_M)
    
    socket = Socket(HOST, PORT)

    if VIZ_ENABLE:
        viz = RealTimeViz()
    doViz = True
    
    try:
        while True:

            # loop around until enough time has passed (sample time from last execution)
            while (elapsed_time > 0 and (time() - (start_time + elapsed_time)) < dt): pass
            elapsed_time = time() - start_time
            
            print("Waiting on vehicle speed...")
            ego_speed = socket.receive_vehicle_speed()
            print(f"Received vehicle speed: {ego_speed} m/s")
            
            # update positions of vehicles in simulator
            simulator.step_simulator(ego_speed)
            lead_speed = simulator.get_current_lead_speed()
            lead_distance = simulator.get_rel_lead_distance()
            
            if VIZ_ENABLE:
                # invoke visualizer every other timestep (dt_viz = 2*dt_sim)
                doViz = not doViz
                if doViz:
                    viz.draw(
                        simulator.ego_position, 
                        ego_speed, 
                        lead_speed,
                        lead_distance
                    )

            print("Sending environmental info to board...")
            socket.send_environment_info(lead_speed, SET_SPEED_MPS, lead_distance)
            
    except Exception as e:
        print(e)
    finally:
        socket.close_connection()
        sys.exit()
        

if __name__ == '__main__':
    main()