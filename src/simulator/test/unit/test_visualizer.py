import pytest, sys
from pathlib import Path

PROJECT_BASE = Path(__file__).parents[4]
SIMULATOR_PATH = PROJECT_BASE / 'src' / 'simulator'

sys.path.append(str(SIMULATOR_PATH))

from real_time_viz import RealTimeViz, DT_VIZ

# TODO : write some good visualizer tests...