#!/usr/bin/python3.8

import numpy as np
from matplotlib import pyplot as plt

# Run ./build/fldigi-noise-sdr-x86-STREAM -d "Hello" -m MODE_3X_PSK250R -o
# buffer.txt -w data/rfpwm.txt -c 4000
# to generate the waveforms

SMALL_SIZE = 8*3
MEDIUM_SIZE = 10*3
BIGGER_SIZE = 12*3

plt.rc('font', size=SMALL_SIZE)          # controls default text sizes
# plt.rc('axes', titlesize=SMALL_SIZE)     # fontsize of the axes title
# plt.rc('axes', labelsize=MEDIUM_SIZE)    # fontsize of the x and y labels
# plt.rc('xtick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
# plt.rc('ytick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
# plt.rc('legend', fontsize=SMALL_SIZE)    # legend fontsize
# plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

Fs=80000
Fc = 4000
B = 700

x = np.loadtxt("data/buffer.txt")
z = np.loadtxt("data/rfpwm.txt")

y = []
for t in z:
    th = int(1e-9*t[0]*Fs)
    tl = int(1e-9*t[1]*Fs) - th
    for i in range(th):
        y.append(1)
    for i in range(tl):
        y.append(0)

plt.subplots_adjust(hspace = 1) 
plt.subplot(2,1,1)

plt.plot(y[250:700])
plt.plot(x[250:700])
# plt.show()

plt.subplot(2,1,2)
plt.psd(y, Fs=Fs, NFFT=4096*2)
plt.psd(x, Fs=Fs, NFFT=4096*2)
plt.axvline(x=Fc-B, color='g')
plt.axvline(x=Fc+B, color='g')
plt.show()


# plt.subplots_adjust(left = 0.4) 

# plt.subplot(6, 1, 1)
# plt.plot(a)
# plt.ylabel("a(t)      ", rotation="horizontal",ha='right',va='center')

# plt.subplot(6, 1, 2)
# plt.plot(phi)
# plt.ylabel("\u03B8(t)      ", rotation="horizontal",ha='right',va='center')

# plt.subplot(6, 1, 3)
# # plt.plot(y)
# plt.plot(rfpwm)
# plt.ylabel("x(t)      ", rotation="horizontal",ha='right',va='center')

# plt.subplot(6, 1, 4)
# # plt.plot(y)
# plt.plot(a)
# plt.ylabel("\u03B4(t)      ", rotation="horizontal",ha='right',va='center')

# plt.subplot(6, 1, 5)
# # plt.plot(y)
# plt.plot(fundamental, 'r')
# plt.ylabel("k=1 ", rotation="horizontal",ha='right',va='center')

# plt.subplot(6, 1, 6)
# # plt.plot(y)
# plt.plot(harmonic)
# plt.ylabel("k=2", rotation="horizontal",ha='right',va='center')

# plt.xlabel("Samples")
# plt.show()

# plt.plot(rfpwm); plt.show()
# plt.plot(fundamental); plt.show()
# plt.plot(harmonic); plt.show()
