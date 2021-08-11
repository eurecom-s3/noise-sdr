#!/usr/bin/python3.8

import pandas as pd
import numpy as np
from matplotlib import pyplot as plt
from tabulate import tabulate

df = pd.read_csv("data/ft4-turing-2020-08-12-log.csv")
df.columns = ["distance", "side", "usrpgain", "sent", "time", "snr", "timeoffset",
        "freqoffset", "", "m1", "m2", "m3"]

correct = df["m1"] == "IU7GTP"
df['correct'] = correct

print(df)

# tab = df.groupby(["distance", "side"]).agg({
tab = df.groupby(["distance"]).agg({
        "" : ["count"],
        "correct" : ["mean"],
        "snr": ["mean", "std"]
        # "freqoffset": ["mean", "std"]
    })

print(tab)

# names = ["d (m) and side", "rx (40 tx)", "avg(snr)", "std(snr)", "\% correct"]
names = ["d (m)", "rx (160 tx)", "avg(snr)", "std(snr)", "\% correct"]
print(tabulate(tab, headers=names, tablefmt="latex_raw", floatfmt=".2f"))

# print(tabulate(list(zip(*table)), tablefmt="latex_raw", floatfmt=".2f"))

df = df[df['correct']==True]

up = df[df['side']=='Up'].groupby(["distance"]).agg({
        "" : ["count"],
        "correct" : ["mean"],
        "snr": ["mean", "std"]
        # "freqoffset": ["mean", "std"]
    })

right = df[df['side']=='Right'].groupby(["distance"]).agg({
        "" : ["count"],
        "correct" : ["mean"],
        "snr": ["mean", "std"]
        # "freqoffset": ["mean", "std"]
    })

down = df[df['side']=='Down'].groupby(["distance"]).agg({
        "" : ["count"],
        "correct" : ["mean"],
        "snr": ["mean", "std"]
        # "freqoffset": ["mean", "std"]
    })

left = df[df['side']=='Left'].groupby(["distance"]).agg({
        "" : ["count"],
        "correct" : ["mean"],
        "snr": ["mean", "std"]
        # "freqoffset": ["mean", "std"]
    })

SMALL_SIZE = 8*3
MEDIUM_SIZE = 10*3
BIGGER_SIZE = 12*3

plt.rc('font', size=SMALL_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=SMALL_SIZE)     # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)    # fontsize of the x and y labels
plt.rc('xtick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
plt.rc('ytick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=SMALL_SIZE)    # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

plt.subplots_adjust(hspace = 1) 
# plt.subplots_adjust(left = 0.4) 

plt.subplot(2, 2, 1)

plt.ylabel('SNR (dB)')
plt.xlabel('distance (m)')
plt.plot(up['snr']['mean'])
plt.plot(right['snr']['mean'])
plt.plot(down['snr']['mean'])
plt.plot(left['snr']['mean'])

plt.subplot(2, 2, 2)

plt.ylabel("% rx")
plt.xlabel('distance (m)')
plt.plot(100 * up['']['count'] / 40.0, label='Up') 
plt.plot(100 * right['']['count'] / 40.0, label='Right') 
plt.plot(100 * down['']['count'] / 40.0, label='Down') 
plt.plot(100 * left['']['count'] / 40.0, label='Left') 
plt.legend()
plt.show()

# import IPython; IPython.embed()
