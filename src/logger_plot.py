#!/usr/bin/env python3.5
import matplotlib.pyplot as plt
import sys
sys.tracebacklimit = 0
try:
    from logger_data import *
except ImportError:
    print("No data, please ensure LOGGER_ENABLED is set to (1)!")
    exit()
plt.plot(x_time, y_pop, label='All agents')
plt.plot(x_time, y_food, label='Food')
plt.plot(x_time, y_herb, label='Herbivours')
plt.plot(x_time, y_meat, label='Carnivours')
plt.title("Ecosim Population")
plt.xlabel("Time (s)")
plt.ylabel("Population")
plt.legend(loc='upper right')
plt.show()
