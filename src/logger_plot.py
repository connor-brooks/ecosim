#!/usr/bin/env python3
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import sys
import importlib
import logger_data
sys.tracebacklimit = 0

matplotlib.rcParams['toolbar'] = 'None'
plt.ion()

while True:
    # Try to import the datafile ecosim generated
    importlib.reload(logger_data)
    try:
        from logger_data import *
    except ImportError:
        print("No data, please ensure LOGGER_ENABLED is set to (1)!")
        exit()

    # Grab int values for population
    x_time = np.asarray(x_time)
    y_pop = np.asarray(y_pop)
    y_food = np.asarray(y_food)
    y_herb = np.asarray(y_herb)
    y_meat = np.asarray(y_meat)
    
    # Grab values for traits, scale from int to float
    y_metab_a = np.asarray(y_metab_a) / 100
    y_vision_a = np.asarray(y_vision_a) / 100
    y_rebirth_a = np.asarray(y_rebirth_a) / 100
    y_diet_a = np.asarray(y_diet_a) / 100
    y_flock_a = np.asarray(y_flock_a) / 100
    y_wobble_a = np.asarray(y_wobble_a) / 100

    # Plot population
    ax1 = plt.subplot(211)
    plt.plot(x_time, y_pop, label='Living agents')
    plt.plot(x_time, y_food, label='Food')
    plt.plot(x_time, y_herb, label='Herbivours')
    plt.plot(x_time, y_meat, label='Carnivours')
    plt.title("Ecosim Population")
    plt.ylabel("Population")
    plt.legend(loc='upper right')
    plt.setp(ax1.get_xticklabels(), visible=False)

    # Plot traits
    ax2 = plt.subplot(212)
    plt.plot(x_time, y_metab_a, label='Metabolism')
    plt.plot(x_time, y_vision_a, label='Vision')
    plt.plot(x_time, y_rebirth_a, label='Rebirth')
    plt.plot(x_time, y_diet_a, label='Diet')
    plt.plot(x_time, y_flock_a, label='Flocking')
    plt.plot(x_time, y_wobble_a, label='Wobble')
    plt.title("Average Genetics")
    plt.xlabel("Time (s)")
    plt.ylabel("Average Trait Value")
    plt.legend(loc='upper right')

    # Draw every second
    plt.draw()
    plt.pause(1)
    plt.clf()
    
