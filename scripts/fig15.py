#!/usr/bin/python3.8

import numpy as np
from matplotlib import pyplot as plt
from scipy import signal
from scipy.signal import butter, lfilter

#
# Filter creation functions taken from https://stackoverflow.com/a/12233959
#
def butter_bandstop(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='bandstop')
    return b, a

def butter_bandstop_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandstop(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def butter_lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='high', analog=False)
    return b, a

def butter_highpass_filter(data, cutoff, fs, order=5):
    b, a = butter_highpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

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

ovsr = 8
f_if = 10e3
f_s = 80e3
f_ovs = f_s*ovsr
n_periods = 2

time_ms = np.array([i/1000 for i in range(n_periods*int(f_ovs/f_if))])
x_if = np.array([np.sin(2*np.pi*f_if*i/f_ovs) for i in
    range(n_periods*int(f_ovs/f_if))])

x_rfpwm = np.sign(x_if)

x_if = (x_if+1)/2
x_rfpwm = (x_rfpwm+1)/2
bb = np.array([0.5 for i in range(n_periods*int(f_ovs/f_if))])

x_pwm = []
for i in range(0, n_periods*int(f_ovs/f_if), ovsr):
    for j in range(ovsr):
        if j < int(x_if[i]*ovsr):
            x_pwm.append(1)
        else:
            x_pwm.append(0)

x_bb_pwm = butter_lowpass_filter(x_pwm, 3*f_if, f_ovs)

plt.subplots_adjust(hspace = 1) 
# plt.subplots_adjust(wspace = 1) 
# plt.subplot(2,2,1)
# plt.plot(time_ms, x_pwm, 'b')
# plt.plot(time_ms, x_if,'r--')

# plt.subplot(2,2,2)
# plt.plot(time_ms, x_if,'r')

# plt.subplot(2,2,3)
# plt.plot(time_ms, x_rfpwm, 'b')
# plt.plot(time_ms, (2/np.pi)*(x_if-0.5), 'r--')

# plt.subplot(2,2,4)
# plt.plot(time_ms, (2/np.pi)*(x_if-0.5),'r')

plt.subplot(2,1,1)
plt.plot(time_ms, x_pwm, label="PWM")
plt.plot(time_ms, x_if,'--', label="cos")
# plt.ylabel("x(t)")
# plt.xlabel("t (ms)")
# plt.legend()

plt.subplot(2,1,2)
plt.plot(time_ms, x_rfpwm, label="RF-PWM")
plt.plot(time_ms, (2/np.pi)*(x_if-0.5), '--', label="cos")
# plt.xlabel("t (ms)")
# plt.ylabel("x(t)")
# plt.legend()

plt.show()
