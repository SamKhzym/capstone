import control
import numpy as np

DT_PLANT = 0.01

class PlantModel:

    def __init__(self, gain: float, zeta: float, omega_n: float) -> None:

        self.gain = gain
        self.zeta = zeta
        self.omega_n = omega_n

        numerator = [gain * self.omega_n ** 2]
        denominator = [1, 2 * self.zeta * omega_n, omega_n ** 2]

        self.system = control.tf2ss(numerator, denominator)
        self.dt = DT_PLANT

        self.state = np.array([0.0,0.0])

    def step_plant(self, pwm_command: int) -> float:

        x_dot = self.system.A @ self.state + self.system.B @ np.array([pwm_command])

        self.state += (x_dot * self.dt)

        output = self.system.C @ self.state + self.system.D @ np.array([pwm_command])

        return output[0]