import matplotlib.pyplot as plt
from pathlib import Path

PROJECT_BASE = Path(__file__).parents[2]

class SerialData:

    def __init__(self, txt_name):

        lines = []

        self.times = []
        self.wheelSpeeds1 = []
        self.wheelSpeeds1 = []

        self.dynoSpeeds1 = []
        self.dynoSpeeds2 = []

        self.w1DutyCycles = []
        self.w2DutyCycles = []

        self.t1DutyCycles = []
        self.t2DutyCycles = []
        self.pwmSpeeds = []

        with open(txt_name, 'r') as file:
            lines = file.readlines()

        for i in range(len(lines)):

            if '====' in lines[i]: continue

            # print(lines[i])

            fields = lines[i].split(":")
            name = fields[0]
            value = float(fields[1])

            if name == "Elapsed Time": self.times.append(value)
            if name == "PWM_Speed": self.pwmSpeeds.append(value)

            if name == "Duty_Cycle_W1": self.w1DutyCycles.append(value)
            if name == "Duty_Cycle_W2": self.w2DutyCycles.append(value)
            
            if name == "Duty_Cycle_T1": self.t1DutyCycles.append(value)
            if name == "Duty_Cycle_T2": self.t2DutyCycles.append(value)
            
            if name == "Wheel_Speed_W1": self.wheelSpeeds1.append(value)
            if name == "Wheel_Speed_W2": self.wheelSpeeds2.append(value)

            if name == "Dyno_Speed_T1": self.dynoSpeeds1.append(value)
            if name == "Dyno_Speed_T2": self.dynoSpeeds2.append(value)
        # print(len(self.times))
        # print(self.times)

    def plotMetrics(self):

        fig, ax = plt.subplots(6, 1, sharex=True)

        ax[0].plot(s.times, s.dutyCycles1, label="Duty Cycle 1 (%)")
        ax[0].axhline(y = 90, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[0].axhline(y = 45, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[0].legend()

        ax[1].plot(s.times, s.dutyCycles2, label="Duty Cycle 2 (%)")
        ax[1].axhline(y = 50, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[1].axhline(y = 10, color = 'r', linestyle = '-', label='Threshold (%)') 
        ax[1].legend()

        ax[2].plot(s.times, s.pwmSpeeds, label="PWM Output Commands (0-255)")
        ax[2].legend()

        ax[3].plot(s.times, s.wheelSpeeds1, label="Linear Wheel Speed W1 (m/s)")
        ax[3].plot(s.times, s.wheelSpeeds2, label="Linear Wheel Speed W2 (m/s)")
        ax[3].plot(s.times, [(s.wheelSpeeds1[i] + s.wheelSpeeds2[i]) / 2 for i in range(len(s.wheelSpeeds1))], label="Average Wheel Speed (m/s)")
        ax[3].legend()

        ax[4].plot(s.times, s.dynoSpeeds1, label="Linear Dyno Speed T1 (m/s)")
        ax[4].plot(s.times, s.dynoSpeeds2, label="Linear Dyno Speed T2 (m/s)")
        ax[4].plot(s.times, [(s.dynoSpeeds1[i] + s.dynoSpeeds2[i]) / 2 for i in range(len(s.dynoSpeeds1))], label="Average Dyno Speed (m/s)")
        ax[4].legend()

        ax[5].plot(s.times, s.wheelSpeeds1, label="Linear Wheel Speed W1 (m/s)")
        ax[5].plot(s.times, s.wheelSpeeds2, label="Linear Wheel Speed W2 (m/s)")
        ax[5].plot(s.times, s.dynoSpeeds1, label="Linear Dyno Speed T1 (m/s)")
        ax[5].plot(s.times, s.dynoSpeeds2, label="Linear Dyno Speed T2 (m/s)")
        ax[5].plot(s.times, [(s.dynoSpeeds1[i] + s.dynoSpeeds2[i]+ s.wheelSpeeds1[i] + s.wheelSpeeds2[i]) / 4 for i in range(len(s.dynoSpeeds1))], label="Average Dyno Speed (m/s)")
        ax[5].legend()
        plt.show()

if __name__ == '__main__':
    # file = PROJECT_BASE / 'src' / 'processing' / 'data' / 'swept_sine_0_9.txt'
    file = PROJECT_BASE / 'src' / 'processing' / 'data' / 'putty.txt'

    s = SerialData(str(file))
    s.plotMetrics()