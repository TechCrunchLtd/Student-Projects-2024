# Importing required libraries
import matplotlib.pyplot as plt
from scipy.datasets import electrocardiogram
# from scipy.misc import electrocardiogram
import numpy as np
import time
import sys

RA_signal_factor = float(sys.argv[1])
LA_signal_factor = float(sys.argv[2])
LL_signal_factor = float(sys.argv[3])
LA_timelag_factor = 0.05
LL_timelag_factor = 0.075
LA_lead_factor = 30
LL_lead_factor = 35

# Generating the ECG signal using scipy's electrocardiogram function
ecg = electrocardiogram()

# Frequency of the ECG signal (sampling rate)
frequency = 360

# Calculating time data based on the size of the ECG signal and the frequency
time_data = np.arange(ecg.size) / frequency
time_window = 1
start_index = 0

# Create the plot
fig, axes = plt.subplots(2, 1, figsize=(10, 6), sharex=True)
axes[0].set_ylabel("ECG (milliVolts)")
axes[1].set_ylabel("RAW (milliVolts)")
plt.xlabel("Time (seconds)")
show_label = False


plt.ion()  # Turn on interactive mode
while start_index < len(ecg):
    end_index = start_index + int(frequency * time_window)
    if start_index > 60:
        axes[0].plot(time_data[start_index:end_index] - 0.1, ecg[start_index-60:end_index-60], color='blue', label='ECG')
    # else:
    #     axes[0].plot(time_data[start_index:end_index] - 0.1, ecg[start_index:end_index], color='blue', label='ECG')
    if start_index > 35:
        axes[1].plot(time_data[start_index:end_index], ecg[start_index:end_index]*RA_signal_factor, color='black', label='RA')  # Plot with 10ms delay
        axes[1].plot(time_data[start_index:end_index] - LA_timelag_factor, ecg[start_index-LA_lead_factor:end_index-LA_lead_factor]*LA_signal_factor, color='green', label='LA')  # Plot with 10ms delay
        axes[1].plot(time_data[start_index:end_index] - LL_timelag_factor, ecg[start_index-LL_lead_factor:end_index-LL_lead_factor]*LL_signal_factor, color='yellow', label='LL')  # Plot with 10ms delay
        plt.xlim(time_data[start_index], time_data[start_index + frequency - 1])  # Update x-axis limits
        plt.draw()
        if show_label == False:
            show_label = True
            plt.legend()

    if start_index%10 == 0:
        plt.pause(0.001)
    # time.sleep(0.00001)
    start_index += 1

plt.ioff()  # Turn off interactive mode after the loop
plt.show()  # Display the final plot


# DEPENDENCIES
# matplotlib
# numpy
# scipy
# pooch

# import sys

# Access command-line arguments
# value1 = sys.argv[1]
# value2 = sys.argv[2]

# Use the passed values
# print(f"value1: {value1}, value2: {value2}")
