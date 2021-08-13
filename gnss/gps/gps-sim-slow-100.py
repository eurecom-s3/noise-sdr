#!/usr/bin/python3.6

# Generate GPS signal, based on:
# https://natronics.github.io/blag/2014/gps-spreading/

from math import sin, pi
from matplotlib import pyplot as plt
import cmath
import numpy as np

f = 154 * 10.23e6      # carrier frequency
f_prn = 1023000/100 #1023000 #10230 #1023000   # PRN frequency
f_s  = 8000000/100 #4e6  #80e3

fs = f_s
f_prn = f_prn

# PRN assignments
SV = {
   1: [2,6],
   2: [3,7],
   3: [4,8],
   4: [5,9],
   5: [1,9],
   6: [2,10],
   7: [1,8],
   8: [2,9],
   9: [3,10],
  10: [2,3],
  11: [3,4],
  12: [5,6],
  13: [6,7],
  14: [7,8],
  15: [8,9],
  16: [9,10],
  17: [1,4],
  18: [2,5],
  19: [3,6],
  20: [4,7],
  21: [5,8],
  22: [6,9],
  23: [1,3],
  24: [4,6],
  25: [5,7],
  26: [6,8],
  27: [7,9],
  28: [8,10],
  29: [1,6],
  30: [2,7],
  31: [3,8],
  32: [4,9],
}

# Shift register
def shift(register, feedback, output):
    """GPS Shift Register
    
    :param list feedback: which positions to use as feedback (1 indexed)
    :param list output: which positions are output (1 indexed)
    :returns output of shift register:
    
    """
    
    # calculate output
    out = [register[i-1] for i in output]
    if len(out) > 1:
        out = sum(out) % 2
    else:
        out = out[0]
        
    # modulo 2 add feedback
    fb = sum([register[i-1] for i in feedback]) % 2
    
    # shift to the right
    for i in reversed(range(len(register[1:]))):
        register[i+1] = register[i]
        
    # put feedback in position 1
    register[0] = fb
    
    return out

# Generate C/A code
def PRN(sv):
    """Build the CA code (PRN) for a given satellite ID
    
    :param int sv: satellite code (1-32)
    :returns list: ca code for chosen satellite
    
    """
    
    # init registers
    G1 = [1 for i in range(10)]
    G2 = [1 for i in range(10)]

    ca = [] # stuff output in here
    
    # create sequence
    for i in range(1023):
        g1 = shift(G1, [3,10], [10])
        g2 = shift(G2, [2,3,6,8,9,10], SV[sv]) # <- sat chosen here from table
        
        # modulo 2 add and append to the code
        ca.append((g1 + g2) % 2)

    # return C/A code!
    return ca

# find ca code for sat 24, and make 0 into -1 to use in BPSK
sats = [[-1 if x==0 else x for x in PRN(idx)] for idx in range(1,33)]
sats = [sats[0]]+sats
prn = lambda idx, t: sats[idx][int(t*f_prn)%1023]

# Generate ~1ms of data for a plain carrier, and a GPS signal
def satellite(idx, duration):
    signal = []
    for i in range(int(f_s*duration)):
        t = i / f_s
        signal.append(prn(idx,t))
    
    amplitude = 1
    # phase = np.array([0 if s == 1 else -np.pi for s in signal])
    
    output = [cmath.rect(amplitude, 0 if s == 1 else -np.pi) for s in signal]
    output = np.array(output, dtype=np.complex64)
    return output

duration = 10
satellites = [24]
output = satellite(satellites[0], duration)
for idx in satellites[1:]:
    output += satellite(idx, duration)
output = output / len(satellites)
output.tofile("/tmp/time")
