import matplotlib.pyplot as plt
from pathlib import Path

PROJECT_BASE = Path(__file__).parents[2]

class SerialData:

    def __init__(self, txt_name):

        lines = []

        self.times = []
        self.wheelSpeeds1 = []
        self.wheelSpeeds2 = []
        self.dynoWheelSpeeds1 = []
        self.dynoWheelSpeeds2 = []
        self.dutyCycles1 = []
        self.dutyCycles2 = []
        self.pwmSpeeds = []
        self.vehSpeeds = []
        self.dynoPwm1 = []
        self.dynoPwm2 = []

        with open(txt_name, 'r', encoding="utf8") as file:
            lines = file.readlines()

        for i in range(len(lines)):

            if '====' in lines[i]: continue
            if 'STUPID' in lines[i]: continue

            print(lines[i])

            fields = lines[i].split(":")
            name = fields[0]
            value = float(fields[1])

            if name == "Elapsed Time": self.times.append(value)
            if name == "PWM_Speed": self.pwmSpeeds.append(value)
            if name == "Duty_Cycle_W1": self.dutyCycles1.append(value)
            if name == "Duty_Cycle_W2": self.dutyCycles2.append(value)
            if name == "Duty_Cycle_T1": self.dynoPwm1.append(value)
            if name == "Duty_Cycle_T2": self.dynoPwm2.append(value)
            if name == "Wheel_Speed_W1": self.wheelSpeeds1.append(value)
            if name == "Wheel_Speed_W2": self.wheelSpeeds2.append(value)
            if name == "Dyno_Speed_T1": self.dynoWheelSpeeds1.append(value)
            if name == "Dyno_Speed_T2": self.dynoWheelSpeeds2.append(value)
            if name == "Vehicle Speed": self.vehSpeeds.append(value)

        # print(len(self.times))
        # print(self.times)

    def plotMetrics(self):

        fig, ax = plt.subplots(6, 1, sharex=True)

        ax[0].plot(s.times, s.dutyCycles1, label="Duty Cycle 1 (%)")
        ax[0].axhline(y = 65, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[0].axhline(y = 60, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[0].legend()

        ax[1].plot(s.times, s.dutyCycles2, label="Duty Cycle 2 (%)")
        ax[1].axhline(y = 50, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[1].axhline(y = 40, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[1].legend()

        ax[2].plot(s.times, s.dynoPwm1, label="Duty Cycle 1 (%)")
        ax[2].axhline(y = 55, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[2].axhline(y = 50, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[2].legend()

        # ax[3].plot(s.times, s.dynoPwm2, label="Duty Cycle 2 (%)")
        # ax[3].axhline(y = 49, color = 'r', linestyle = '-', label='Threshold (%)') 
        # ax[3].axhline(y = 46, color = 'r', linestyle = '-', label='Threshold (%)') 
        # ax[3].legend()

        ax[4].plot(s.times, s.pwmSpeeds, label="PWM Output Commands (0-255)")
        ax[4].legend()

        ax[5].plot(s.times, s.wheelSpeeds1, label="Linear Wheel Speed W1 (m/s)", color='red')
        ax[5].plot(s.times, s.wheelSpeeds2, label="Linear Wheel Speed W2 (m/s)", color='blue')
        ax[5].plot(s.times, s.dynoWheelSpeeds1, label="Linear Wheel Speed T1 (m/s)", color='green')
        ax[5].plot(s.times, s.dynoWheelSpeeds2, label="Linear Wheel Speed T2 (m/s)", color='purple')
        ax[5].plot(s.times, s.vehSpeeds, label="Linear Veh Speed (m/s)", color='black', linewidth=3)
        
        # ax[5].plot(s.times, [(s.wheelSpeeds1[i] + s.wheelSpeeds2[i] + s.dynoWheelSpeeds1[i] + s.dynoWheelSpeeds2[i]) / 4 for i in range(len(s.wheelSpeeds1))], label="Average Wheel Speed (m/s)", color='black', linewidth=3)
        ax[5].legend()

        plt.show()

if __name__ == '__main__':
    # file = PROJECT_BASE / 'src' / 'processing' / 'data' / 'swept_sine_0_9.txt'
    file = PROJECT_BASE / 'src' / 'processing' / 'data' / 'putty.txt'

    s = SerialData(str(file))
    s.plotMetrics()