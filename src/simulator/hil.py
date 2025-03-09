from env_simulator import EnvironmentalSimulator, DT_SIM
from real_time_viz import RealTimeViz, DT_VIZ
from client_socket import Socket
from pathlib import Path
from time import time

# pathing stuff
PROJECT_BASE = Path(__file__).parents[1]
DRIVE_CYCLE_DIR = PROJECT_BASE / 'simulator' / 'drive_cycle'

# simulation parameters
DRIVE_CYCLE_NAME = 'hwfet.csv'
SET_SPEED_MPS = 0.85
INIT_LEAD_DIST_M = 1.0

# Ethernet connection params
HOST = "fd12:3456:789a::1"
PORT = 4242

def main():

    dt = DT_SIM
    start_time = time()
    elapsed_time = 0

    drive_cycle_path = str(DRIVE_CYCLE_DIR / DRIVE_CYCLE_NAME)
    simulator = EnvironmentalSimulator(drive_cycle_path, SET_SPEED_MPS, INIT_LEAD_DIST_M)
    
    socket = Socket(HOST, PORT)

    viz_enable = False
    if viz_enable:
        viz = RealTimeViz()
    doViz = True
    
    try:
        while True:

            # loop around until enough time has passed (sample time from last execution)
            while (elapsed_time > 0 and (time() - (start_time + elapsed_time)) < dt): pass
            elapsed_time = time() - start_time

            socket.send_speed_request()
            # print("Waiting on vehicle speed...")
            # ego_speed = socket.receive_vehicle_speed()
            # print(f"Received vehicle speed: {ego_speed} m/s")
            # TODO: GET EGO SPEED FROM MICROCONTROLLER
            ego_speed = 0.2
            
            # update positions of vehicles in simulator
            simulator.step_simulator(ego_speed)
            lead_speed = simulator.get_current_lead_speed()
            lead_distance = simulator.get_rel_lead_distance()
            
            if viz_enable:
                # invoke visualizer every other timestep (dt_viz = 2*dt_sim)
                doViz = not doViz
                if doViz:
                    viz.draw(
                        simulator.ego_position, 
                        ego_speed, 
                        lead_speed,
                        lead_distance
                    )

            # TODO: transmit stuff to microcontroller
            socket.receive_confirmation_message()
            # print("Sending environmental info to board...")
            # socket.send_environment_info(lead_speed, SET_SPEED_MPS, lead_distance)
            
    # except Exception as e:
    #     print(e)
    finally:
        socket.close_connection()
        

if __name__ == '__main__':
    main()