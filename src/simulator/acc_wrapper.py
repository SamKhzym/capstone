import ctypes
from ctypes import c_float, c_ubyte

class AccWrapper:

    def __init__(self, shared_object_path: str):
        self.accLibrary = ctypes.CDLL(shared_object_path)

    def init_acc(self) -> None:
        self.accLibrary.initAcc()

    def step_acc(self, host_vel: float, lead_vel: float, set_speed: float, lead_dist: float) -> int:
        return ctypes.c_ubyte(
            self.accLibrary.stepAcc(
                c_float(host_vel), 
                c_float(lead_vel), 
                c_float(set_speed), 
                c_float(lead_dist)
            )
        ).value