from optimize import besee_ga_optimize
from typing import Dict
from pathlib import Path
import pandas as pd
import sys

PROJECT_BASE = Path(__file__).parent.parent.parent

dependencies = [
    PROJECT_BASE / 'python' / 'besee_core',
    PROJECT_BASE / 'python' / 'metrics'
]

for dep in dependencies: sys.path.append(str(dep))

from besee_utils import run_besee
from metrics import rms_jerk, average_soc_drop
from cacc_metrics import rms_speed_error, rms_distance_error

def cacc_run_cost(df: pd.DataFrame, has_lead=False):
    
    speed_error_term = rms_speed_error(df, has_lead=has_lead)
    distance_error_term = 0 if not has_lead else rms_distance_error(df)
    jerk_term = rms_jerk(df)
    soc_term = average_soc_drop(df) * 1000
    
    print(f'speed error: {speed_error_term}')
    print(f'distance error: {distance_error_term}')
    print(f'jerk term: {jerk_term}')
    print(f'soc term: {soc_term}')
    
    # need to weigh these guys somehow...
    return speed_error_term + distance_error_term + jerk_term + soc_term

def cacc_total_cost_function(params: Dict[str, float]) -> float:
    
    scenarios = [
        ('open_straight_road', False),
        ('straight_road_lead_vehicle_ftp', True),
        ('straight_road_lead_vehicle_hwfet', True),
        ('straight_road_lead_vehicle_us06', True)
    ]
    
    total_cost = 0
    
    for scenario, has_lead in scenarios:
        metadata = run_besee(scenario, 100, suppress_output=True)
        total_cost += cacc_run_cost(metadata['df'], has_lead=has_lead)
    
    return total_cost

if __name__ == '__main__':
    total_cost = cacc_total_cost_function(None)
    print(total_cost)