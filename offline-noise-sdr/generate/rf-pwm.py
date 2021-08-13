#!/usr/bin/python3.6

# Giovanni Camurati

import numpy as np
import click
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d

@click.group()
def cli():
    """
    """

@cli.command()
@click.option("--input-file", default="/tmp/time",
              show_default=True,
              type=click.Path(dir_okay=False),
              help="File with baseband IQ data (gr-complex format).")
@click.option("--output-file", default="/tmp/rfpwm", 
              show_default=True,
              type=click.Path(dir_okay=False),
              help="File with RF-PWM timings.")
@click.option("--fs-in", default=48e3, show_default=True,
              help="Sampling frequency of the IQ file.")
@click.option("--fs-out", default=4.8e6, show_default=True,
              help="Sampling frequency of the IQ file.")
@click.option("--f-if", default=5e3, show_default=True,
              help="Intermediate frequency.")
@click.option("--plot/--no-plot", default=False, show_default=True,
              help="Visualize data (use only with small files/frequencies).")
@click.option("--normalize/--no-normalize", default=False, show_default=True,
              help="Normalize amplitude.")
def generate(input_file, output_file, fs_in, fs_out, f_if, plot, normalize):
    """
    Generate RF-PWM square wave timings starting from IQ baseband
    """
    print("Opening IQ file")
    with open(input_file) as f: 
        data = np.fromfile(f, dtype=np.complex64)

    print("Starting RF-PWM conversion")
    print("Amplitude and phase")
    a = np.absolute(data)
    phi = np.angle(data)

    if(normalize):
        print("Normalization")
        a = (a - np.min(a)) / np.ptp(a)

    print("Pre-distortion") 
    a2 = np.arcsin(a) / np.pi

    print("Resampling fs-in to fs-out")
    Nsa = len(a2)*int(fs_out/fs_in)
    x = np.linspace(0,len(a2),len(a2))
    xresampled = np.linspace(0,len(a2),len(a2)*int(fs_out/fs_in))

    fa2 = interp1d(x, a2, kind='cubic')
    fphi2 = interp1d(x, phi, kind='cubic')
    a2_resampled = fa2(xresampled)
    phi_resampled = fphi2(xresampled)

    # a2_resampled = a2.repeat(int(fs_out/fs_in))
    # phi_resampled = phi.repeat(int(fs_out/fs_in)) 

    print("Generating IF carrier")
    # lambda function to rf-pwm modulate a square wave
    modulate = lambda phi, i : np.sign(np.cos(2*np.pi*f_if*i/fs_out +
       + phi))
    time = np.linspace(0, Nsa, Nsa)
    y = modulate(phi_resampled, time)

    print("Generating pulse timings")
    th = []
    t = []
    i = 0
    if(y[i] < 0):
       while y[i] < 0:
           i = i + 1
    while(i < len(y)):
       j = 0
       while i+j < len(y) and y[i+j] > 0:
           j = j + 1
       while i+j < len(y) and y[i+j] < 0:
           j = j + 1
       t.append(j)
       th.append(a2_resampled[i] * j)
       i = i + j
    
    th = np.array(th)
    t = np.array(t)
    
    print("Saving output file with timings")
    with open(output_file, "w") as f:
        for x,q in zip(th,t):
           Th_ns = 1e9 * x / fs_out
           T_ns = 1e9 * q / fs_out
           # print("%d %d"%(Th_ns, T_ns)) 
           f.write("%d %d\n"%(Th_ns, T_ns)) 
    
    if(plot):
        time = [i/fs_in for i in range(len(data))]
        time2 = [i/fs_out for i in range(len(y))]
        
        plt.subplots_adjust(hspace = 1) 
        
        plt.subplot(5, 1, 1)
        plt.title("Spectrogram")
        plt.xlabel("Time (s)")
        plt.ylabel("Frequency (Hz)")
        plt.specgram(data, Fs=fs_in)
        
        plt.subplot(5, 1, 2)
        plt.title("IQ data")
        plt.xlabel("Time (s)")
        plt.ylabel("I(t), Q(t)")
        plt.plot(time, data.real, label="I(t)")
        plt.plot(time, data.imag, label="Q(t)")
        plt.legend()
        
        plt.subplot(5, 1, 3)
        plt.title("Amplitude and pre-distorted amplitude")
        plt.xlabel("Time (s)")
        plt.ylabel("Amplitude(t)")
        plt.plot(time, a, label="Original")
        plt.plot(time, a2, label="Pre-distorted")
        plt.legend()
        
        plt.subplot(5, 1, 4)
        plt.title("Phase")
        plt.xlabel("Time (s)")
        plt.ylabel("Phase(t) (rad)")
        plt.plot(time, phi)
 
        plt.subplot(5, 1, 5)
        plt.title("RF-PWM")
        plt.xlabel("Time (s)")
        plt.ylabel("RF-PWM(t)")
        plt.plot(time2, y)
        plt.show()

if __name__ == "__main__":
    cli()
