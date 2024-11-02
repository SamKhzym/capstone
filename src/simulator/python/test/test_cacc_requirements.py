from pathlib import Path
import sys
import pytest
import numpy as np, pandas as pd

PROJECT_BASE = Path(__file__).parent.parent.parent

dependencies = [
    PROJECT_BASE / 'python' / 'besee_core',
    PROJECT_BASE / 'python' / 'metrics'
]

for dep in dependencies: sys.path.append(str(dep))

from conftest import get_besee_logs_for_scenario_list
from cacc_metrics import calculate_closest_follow_dist_m

CACC_SCENARIOS = [
    'open_straight_road',
    'straight_road_lead_vehicle_ftp',
    'straight_road_lead_vehicle_hwfet',
    'straight_road_lead_vehicle_us06'
]
CACC_SCENARIO_DATA = get_besee_logs_for_scenario_list(CACC_SCENARIOS, suppress_output=True, delete_csv=True)

@pytest.mark.parametrize('scenario', CACC_SCENARIO_DATA.values())
def test_minimum_following_distance_requirement(scenario):
    '''
    requirement under test: at all points in time, the lead vehicle shall not be closer to the 
    ego in the positive x direction than the competition-defined closest following distance from the fdcw
    '''
    
    # load data from metadata dictionary (ignore first 10 entries of csv (0.1sec) to skip setup rows)
    df: pd.DataFrame = scenario['df']
    df = df.iloc[10:]
    scenario_name = scenario['scenario_name']
    
    # if lead vehicle data not logged, skip test
    if 'ACTOR_lead_speed' not in list(df.columns.values):
        pytest.skip(reason=f'Skipping test. No lead vehicle information in {scenario_name}.')
    
    # get lead vehicle speed information, calculate closest follow distance at each timestep
    lead_speeds = df['ACTOR_lead_speed'].values
    closest_following_distances = np.vectorize(calculate_closest_follow_dist_m)(lead_speeds)
    
    # get lead distance at each timestep, check which timesteps violate requirement
    lead_distances = df['rel_lead_x'].values
    failed_reqmt_timesteps = np.where((lead_distances) < closest_following_distances)[0]
    
    assert len(failed_reqmt_timesteps) == 0, f"""
    Lead vehicle gets closer than closest following distance 
    during scenario {scenario_name} at t = {df['time'].values[failed_reqmt_timesteps[0]]}s!
    """