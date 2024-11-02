import numpy as np
import pandas as pd
import math

MPS_TO_MILESPH = 2.23694

def calculate_timeseries_speed_error(df: pd.DataFrame, has_lead=False):
    if not has_lead:
        return (df['ego_set_speed'] - df['ego_speed'])
    return (np.minimum(df['ego_set_speed'], df['ACTOR_lead_speed']) - df['ego_speed'])

def rms_speed_error(df: pd.DataFrame, has_lead=False):
    errors = calculate_timeseries_speed_error(df, has_lead).values
    return np.sqrt(np.mean(np.square(errors)))

def calculate_closest_follow_dist_m(lead_speed_mps):
    lead_speed_milesph = lead_speed_mps * MPS_TO_MILESPH
    return 2.8 * (lead_speed_milesph ** 0.45) + 8

def calculate_farthest_follow_dist_m(lead_speed_mps):
    lead_speed_milesph = lead_speed_mps * MPS_TO_MILESPH
    if lead_speed_milesph <= 20:
        return 100
    return (-0.2 * (20 - lead_speed_milesph) ** 2) + 100

def calculate_desired_follow_dist_m(lead_speed_mps):
    return 0.5 * (
        calculate_closest_follow_dist_m(lead_speed_mps) +
        calculate_farthest_follow_dist_m(lead_speed_mps)
    )

def calculate_timeseries_distance_error(df: pd.DataFrame):
    dist_calc_vectorized = np.vectorize(calculate_desired_follow_dist_m)
    desired_distances = dist_calc_vectorized(df['ACTOR_lead_speed'].values)
    errors = desired_distances - df['rel_lead_x'].values
    return errors

def rms_distance_error(df: pd.DataFrame):
    errors = calculate_timeseries_distance_error(df)
    return np.sqrt(np.mean(np.square(errors)))