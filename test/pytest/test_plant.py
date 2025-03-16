import pytest, sys
from pathlib import Path

PROJECT_BASE = Path(__file__).parents[2]
SIMULATOR_PATH = PROJECT_BASE / 'src' / 'simulator'

sys.path.append(str(SIMULATOR_PATH))

from plant_model import PlantModel, DT_PLANT

GAIN = 1
PWM_GAIN = 1.0/255.0
ZETA = 1
OMEGA = 1

@pytest.fixture
def basic_plant():
    return PlantModel(
        GAIN, ZETA, OMEGA
    )

@pytest.fixture
def basic_plant_with_pwm_gain():
    return PlantModel(
        PWM_GAIN, ZETA, OMEGA
    )

def test_basic_step_response(basic_plant):
    '''test basic properties of transfer function with gain=1, zeta=1, omega=1'''

    # run for one second (plant input = 0)
    num_timesteps = int(1.0 / DT_PLANT)
    veh_speed = 0
    for _ in range(num_timesteps):
        veh_speed = basic_plant.step_plant(0)

    # assert output speed is 0
    assert veh_speed == 0

    # run for 1.58 seconds (plant input = 1)
    num_timesteps = int(1.58 / DT_PLANT)
    for _ in range(num_timesteps):
        veh_speed = basic_plant.step_plant(1)

    # assert output speed is roughly 0.5 (from step response properties)
    # note: high tolerance here since forward euler is susceptible to noise over time
    assert veh_speed == pytest.approx(0.5, 0.1)

    # run for 10 more seconds (plant input = 1)
    num_timesteps = int(10.0 / DT_PLANT)
    for _ in range(num_timesteps):
        veh_speed = basic_plant.step_plant(1)

    # assert output speed is roughly 1 (from step response final value)
    assert veh_speed == pytest.approx(1.0, 0.01)

def test_pwm_gain_step_response(basic_plant_with_pwm_gain):
    '''test basic properties of transfer function with gain=1/255, zeta=1, omega=1'''

    # run for one second (plant input = 0)
    num_timesteps = int(1.0 / DT_PLANT)
    veh_speed = 0
    for _ in range(num_timesteps):
        veh_speed = basic_plant_with_pwm_gain.step_plant(0)

    # assert output speed is 0
    assert veh_speed == 0

    # run for 1.58 seconds (plant input = 255)
    num_timesteps = int(1.58 / DT_PLANT)
    for _ in range(num_timesteps):
        veh_speed = basic_plant_with_pwm_gain.step_plant(255)

    # assert output speed is roughly 0.5 (from step response properties)
    # note: high tolerance here since forward euler is susceptible to noise over time
    assert veh_speed == pytest.approx(0.5, 0.1)

    # run for 10 more seconds (plant input = 1)
    num_timesteps = int(10.0 / DT_PLANT)
    for _ in range(num_timesteps):
        veh_speed = basic_plant_with_pwm_gain.step_plant(255)

    # assert output speed is roughly 1 (from step response final value)
    assert veh_speed == pytest.approx(1.0, 0.01)