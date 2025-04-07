import matplotlib.pyplot as plt
from env_simulator import EnvironmentalSimulator
from pathlib import Path

PROJECT_BASE = Path(__file__).parents[2]
SIMULATOR_PATH = PROJECT_BASE / 'src' / 'simulator'

SET_SPEED_MPS = 0.85
INIT_LEAD_DIST_M = 1.0

if __name__ == '__main__':
    hwfet_sim = EnvironmentalSimulator(
        str(SIMULATOR_PATH / 'drive_cycle' / 'hwfet.csv'), # drive cycle path
        SET_SPEED_MPS, # set speed (m/s)
        INIT_LEAD_DIST_M # initial lead distance (m)
    )

    udds_sim = EnvironmentalSimulator(
        str(SIMULATOR_PATH / 'drive_cycle' / 'udds.csv'), # drive cycle path
        SET_SPEED_MPS, # set speed (m/s)
        INIT_LEAD_DIST_M # initial lead distance (m)
    )
    
    us06_sim = EnvironmentalSimulator(
        str(SIMULATOR_PATH / 'drive_cycle' / 'us06.csv'), # drive cycle path
        SET_SPEED_MPS, # set speed (m/s)
        INIT_LEAD_DIST_M # initial lead distance (m)
    )

    fig, ax = plt.subplots(3,1)
    fig.suptitle('Lead Vehicle Drive Profiles', fontsize=20, weight='bold')
    fig.supxlabel('Time (seconds)', weight='bold', fontsize=15)
    fig.supylabel('Lead Vehicle Speed (m/s)', weight='bold', fontsize=15)
    ax[0].plot(hwfet_sim.times, hwfet_sim.lead_speeds, linewidth=3, color='red', label='$\\bf{HWFET}$')
    ax[1].plot(udds_sim.times, udds_sim.lead_speeds, linewidth=3, color='green', label='$\\bf{UDDS}$')
    ax[2].plot(us06_sim.times, us06_sim.lead_speeds, linewidth=3, color='blue', label='$\\bf{US06}$')
    
    for a in ax:
        a.legend(loc='upper left', title_fontsize=20, fancybox=True, fontsize=15)

    fig.set_size_inches(9, 8)
    plt.show()