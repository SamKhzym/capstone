import pytest, sys
from pathlib import Path
import scipy.interpolate
from typing import List, Tuple
import numpy as np
import pytest
import matplotlib.pyplot as plt
import os

# min and max set speeds
MIN_SPEED_MPS = 0.4
MAX_SPEED_MPS = 1.0
AVG_SPEED_MPS = (MIN_SPEED_MPS + MAX_SPEED_MPS) / 2

# metric_names = [
#     'time_s', 
#     'set_speed_mps',
#     'ego_speed_mps',
#     'lead_speed_mps',
#     'lead_dist_m',
#     'act_req',
#     'ego_accel_mps2',
#     'ego_jerk_mps3',
#     'speed_error_mps'
# ]

MAX_ACCEL_MPS2 = 2
MAX_JERK_MPS3 = 10

MIN_ACT_REQ = 100
MAX_ACT_REQ = 255

DT = 0.01

SS_MAX_MPS = 0.5 / DT

V_ERR_MAX = 0.6
V_ERR_MAX_SS = 0.3

MAX_DELTA_ACT_REQ = 5

D_MAX = 1.5

def acc_2_1(metrics):
    true_set_speeds = metrics['ego_speed_mps'] - np.min(np.array([metrics['set_speed_mps'], metrics['lead_speed_mps']]), axis=0)
    set_speed_deltas = np.diff(true_set_speeds, prepend=[0])
    filtered_speed_error = metrics['speed_error_mps'][(set_speed_deltas <= SS_MAX_MPS) & (metrics['ego_speed_mps'] >= MIN_SPEED_MPS) & (metrics['lead_dist_m'] > D_MAX)]
    return np.all(np.abs(filtered_speed_error) <= V_ERR_MAX_SS)

def acc_2_2(metrics):
    true_set_speeds = metrics['ego_speed_mps'] - np.min(np.array([metrics['set_speed_mps'], metrics['lead_speed_mps']]), axis=0)
    set_speed_deltas = np.diff(true_set_speeds, prepend=[0])
    filtered_speed_error = metrics['speed_error_mps'][(set_speed_deltas > SS_MAX_MPS) & (metrics['ego_speed_mps'] >= MIN_SPEED_MPS) & (metrics['lead_dist_m'] > D_MAX)]
    return np.all(np.abs(filtered_speed_error) <= V_ERR_MAX)

def acc_2_3(metrics):
    true_set_speeds = metrics['ego_speed_mps'] - np.min(np.array([metrics['set_speed_mps'], metrics['lead_speed_mps']]), axis=0)
    set_speed_deltas = np.diff(true_set_speeds, prepend=[0])
    filtered_speed_error = metrics['speed_error_mps'][(set_speed_deltas <= SS_MAX_MPS) & (metrics['ego_speed_mps'] >= MIN_SPEED_MPS) & (metrics['lead_dist_m'] <= D_MAX)]
    return np.all(np.abs(filtered_speed_error) <= V_ERR_MAX_SS)

def acc_2_4(metrics):
    true_set_speeds = metrics['ego_speed_mps'] - np.min(np.array([metrics['set_speed_mps'], metrics['lead_speed_mps']]), axis=0)
    set_speed_deltas = np.diff(true_set_speeds, prepend=[0])
    filtered_speed_error = metrics['speed_error_mps'][(set_speed_deltas > SS_MAX_MPS) & (metrics['ego_speed_mps'] >= MIN_SPEED_MPS) & (metrics['lead_dist_m'] <= D_MAX)]
    return np.all(np.abs(filtered_speed_error) <= V_ERR_MAX)

def acc_3_1(metrics):
    filtered_accel = metrics['ego_accel_mps2'][metrics['ego_speed_mps'] >= MIN_SPEED_MPS]
    return np.all(np.abs(filtered_accel) <= MAX_ACCEL_MPS2)

def acc_3_2(metrics):
    filtered_jerk = metrics['ego_jerk_mps3'][metrics['ego_speed_mps'] >= MIN_SPEED_MPS]
    return np.all(np.abs(filtered_jerk) <= MAX_JERK_MPS3)

def acc_5_1(metrics):
    filtered_act_req = metrics['act_req'][metrics['ego_speed_mps'] >= MIN_SPEED_MPS]
    if len(filtered_act_req) == 0: return True
    return np.all(MIN_ACT_REQ <= filtered_act_req <= MIN_ACT_REQ)

def sm_3_1(metrics):
    delta_act_req = np.diff(metrics['act_req'], prepend=[0])
    filtered_delta_act_req = delta_act_req[metrics['ego_speed_mps'] >= MIN_SPEED_MPS]
    return np.all(np.abs(filtered_delta_act_req) > 5)

def no_collision(metrics):
    return np.all(metrics['lead_dist_m'] > 0)

def check_reqs(metrics, test_name):
    assert no_collision(metrics)
    assert acc_2_1(metrics)
    assert acc_2_2(metrics)
    assert acc_2_3(metrics)
    assert acc_2_4(metrics)
    assert acc_3_1(metrics)
    assert acc_3_2(metrics)
    assert acc_5_1(metrics)
    assert sm_3_1(metrics)