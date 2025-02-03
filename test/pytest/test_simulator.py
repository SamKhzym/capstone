import pytest, sys
from pathlib import Path

PROJECT_BASE = Path(__file__).parents[2]
SIMULATOR_PATH = PROJECT_BASE / 'src' / 'simulator'

sys.path.append(str(SIMULATOR_PATH))

from env_simulator import EnvironmentalSimulator, DT_SIM

SET_SPEED_MPS = 0.85
INIT_LEAD_DIST_M = 1.0

@pytest.fixture
def hwfet_simulator():
    return EnvironmentalSimulator(
        str(SIMULATOR_PATH / 'drive_cycle' / 'hwfet.csv'), # drive cycle path
        SET_SPEED_MPS, # set speed (m/s)
        INIT_LEAD_DIST_M # initial lead distance (m)
    )

@pytest.fixture
def const_30_simulator():
    return EnvironmentalSimulator(
        str(SIMULATOR_PATH / 'drive_cycle' / 'constant_30.csv'), # drive cycle path
        SET_SPEED_MPS, # set speed (m/s)
        INIT_LEAD_DIST_M # initial lead distance (m)
    )

@pytest.fixture
def rising_30_simulator():
    return EnvironmentalSimulator(
        str(SIMULATOR_PATH / 'drive_cycle' / 'rising_30.csv'), # drive cycle path
        SET_SPEED_MPS, # set speed (m/s)
        INIT_LEAD_DIST_M # initial lead distance (m)
    )

def test_set_speed(rising_30_simulator):
    '''tests set speed is set correctly'''
    assert rising_30_simulator.get_set_speed() == SET_SPEED_MPS

def test_drive_cycle_scaling(rising_30_simulator):
    '''tests proper scaling of drive cycle from highway speeds to rc car speeds'''

    # test 1: assert the lead speed is 0 at 0 seconds
    assert rising_30_simulator.get_lead_speed(0) == 0

    # test 2: assert the lead speed (30 m/s) is converted properly to 0.671081 m/s
    # conversion factor: MPS_TO_MPH / 100
    assert rising_30_simulator.get_lead_speed(20) == pytest.approx(0.671081, 0.001)

def test_drive_cycle_interpolaton_breakpoints(rising_30_simulator):
    '''tests proper interpolation of drive cycle at breakpoints'''

    MAX_SPEED_MPS = 0.671081

    # test 1: assert speed is correct at all breakpoints (0, 10, 20, 30)
    assert rising_30_simulator.get_lead_speed(0) == 0
    assert rising_30_simulator.get_lead_speed(10) == 0
    assert rising_30_simulator.get_lead_speed(20) == pytest.approx(MAX_SPEED_MPS, 0.001)
    assert rising_30_simulator.get_lead_speed(30) == pytest.approx(MAX_SPEED_MPS, 0.001)

def test_drive_cycle_interpolaton_intervals(rising_30_simulator):
    '''tests proper interpolation of drive cycle between time breakpoints'''

    MAX_SPEED_MPS = 0.671081

    # test 1: assert speed is correct at middle of intervals
    assert rising_30_simulator.get_lead_speed(5) == 0
    assert rising_30_simulator.get_lead_speed(15) == pytest.approx(MAX_SPEED_MPS/2, 0.001)
    assert rising_30_simulator.get_lead_speed(25) == pytest.approx(MAX_SPEED_MPS, 0.001)

    # test 2: test linear interpolation at several timesteps within middle of rising edge interval
    assert rising_30_simulator.get_lead_speed(12) == pytest.approx(MAX_SPEED_MPS/5, 0.001)
    assert rising_30_simulator.get_lead_speed(14) == pytest.approx(2*MAX_SPEED_MPS/5, 0.001)
    assert rising_30_simulator.get_lead_speed(16) == pytest.approx(3*MAX_SPEED_MPS/5, 0.001)
    assert rising_30_simulator.get_lead_speed(18) == pytest.approx(4*MAX_SPEED_MPS/5, 0.001)

def test_lead_updates(const_30_simulator):
    '''tests if lead vehicle distance is updated appropriately over time'''

    MAX_SPEED_MPS = 0.671081

    # assert lead vehicle initially at init distance from ego vehicle
    assert const_30_simulator.lead_position == INIT_LEAD_DIST_M

    # run for one second (ego vehicle speed = 0)
    num_timesteps = int(1 / DT_SIM)
    for _ in range(num_timesteps):
        const_30_simulator.step_simulator(0)

    # assert lead vehicle has travelled its top speed for one second
    assert const_30_simulator.lead_position == pytest.approx(INIT_LEAD_DIST_M + MAX_SPEED_MPS, 0.0001)

    # run for one more second (ego vehicle speed = 0)
    num_timesteps = int(1 / DT_SIM)
    for _ in range(num_timesteps):
        const_30_simulator.step_simulator(0)

    # assert lead vehicle has travelled its top speed for two seconds
    assert const_30_simulator.lead_position == pytest.approx(INIT_LEAD_DIST_M + 2*MAX_SPEED_MPS, 0.0001)

def test_ego_updates(const_30_simulator):
    '''tests if ego vehicle distance is updated appropriately over time'''

    EGO_SPEED_MPS = 0.5

    # assert ego vehicle initially at 0
    assert const_30_simulator.ego_position == 0

    # run for one second (ego vehicle speed = EGO_SPEED_MPS)
    num_timesteps = int(1 / DT_SIM)
    for _ in range(num_timesteps):
        const_30_simulator.step_simulator(EGO_SPEED_MPS)

    # assert lead vehicle has travelled its top speed for one second
    assert const_30_simulator.ego_position == pytest.approx(EGO_SPEED_MPS, 0.0001)

    # run for one more second (ego vehicle speed = EGO_SPEED_MPS)
    num_timesteps = int(1 / DT_SIM)
    for _ in range(num_timesteps):
        const_30_simulator.step_simulator(EGO_SPEED_MPS)

    # assert lead vehicle has travelled its top speed for two seconds
    assert const_30_simulator.ego_position == pytest.approx(2*EGO_SPEED_MPS, 0.0001)

def test_rel_position_updates(const_30_simulator):
    '''tests if relative distance is updated appropriately over time'''

    EGO_SPEED_MPS = 0.5
    MAX_LEAD_SPEED_MPS = 0.671081

    # assert lead vehicle initially init dist away
    assert const_30_simulator.get_rel_lead_distance() == INIT_LEAD_DIST_M
    
    # run for one second (ego vehicle speed = EGO_SPEED_MPS)
    num_timesteps = int(1 / DT_SIM)
    for _ in range(num_timesteps):
        const_30_simulator.step_simulator(EGO_SPEED_MPS)

    # assert lead vehicle distance is (initial distance + lead vehicle speed - ego vehicle speed)
    assert const_30_simulator.get_rel_lead_distance() == pytest.approx(INIT_LEAD_DIST_M + MAX_LEAD_SPEED_MPS - EGO_SPEED_MPS, 0.0001)
    
    # run for one second (ego vehicle speed = EGO_SPEED_MPS)
    num_timesteps = int(1 / DT_SIM)
    for _ in range(num_timesteps):
        const_30_simulator.step_simulator(EGO_SPEED_MPS)

    # assert lead vehicle distance is (initial distance + 2 * (lead vehicle speed - ego vehicle speed))
    assert const_30_simulator.get_rel_lead_distance() == pytest.approx(INIT_LEAD_DIST_M + 2 * (MAX_LEAD_SPEED_MPS - EGO_SPEED_MPS), 0.0001)