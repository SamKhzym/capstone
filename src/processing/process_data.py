import matplotlib.pyplot as plt

class SerialData:

    def __init__(self, txt_name):

        lines = []

        self.times = []
        self.speeds = []
        self.dutyCycles = []
        self.pwmSpeeds = []

        with open(txt_name, 'r') as file:
            lines = file.readlines()

        for i in range(len(lines)):

            if '====' in lines[i]: continue

            print(lines[i])

            fields = lines[i].split(":")
            name = fields[0]
            value = float(fields[1])

            if name == "Elapsed Time": self.times.append(value)
            if name == "PWM_Speed": self.pwmSpeeds.append(value)
            if name == "Duty_Cycle": self.dutyCycles.append(value)
            if name == "Vehicle_Speed": self.speeds.append(value)

        print(len(self.times))
        print(self.times)

    def plotMetrics(self):

        fig, ax = plt.subplots(3, 1)

        ax[0].plot(s.times, s.dutyCycles, label="Duty Cycle (%)")
        ax[0].legend()
        ax[1].plot(s.times, s.pwmSpeeds, label="PWM Output Commands (0-255)")
        ax[1].legend()
        ax[2].plot(s.times, s.speeds, label="Linear Speed (m/s)")
        ax[2].legend()

        plt.show()

s = SerialData("data/sine_command.txt")
s.plotMetrics()