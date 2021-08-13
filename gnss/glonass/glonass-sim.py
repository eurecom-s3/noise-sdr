#!/usr/bin/python3.6

# Giovanni Camurati

import click
import numpy as np
from tqdm import tqdm
import cmath

### UTILS ###

codelen = 511
# chiprate = 51100 #511e3 # Hz
# symbolrate = 10 #100 #50 # Hz
df = 562.5e3 # Hz

def ca():
    """
    Generate a C/A code.

    This function uses a simple shift register to generate the C/A code for
    the GLONASS L1 C/A code. This code is the same for all satellites.
    """
    code = [0]*codelen
    shiftreg = [1]*9
    for i in range(codelen):
        code[i] = shiftreg[6]
        feedback = shiftreg[8] ^ shiftreg[4]
        shiftreg[1:9] = shiftreg[0:8]
        shiftreg[0] = feedback
    return code

def hamming_code(data):
    """
    Compute the Hamming code according to the doc.

    This function takes a list of 77 data bits and appends 8 check bits
    according to the algorithm specified in 
    https://www.unavco.org/help/glossary/docs/ICD_GLONASS_4.0_(1998)_en.pdf
    """

    data += [0]*8

    i = [ 9, 10, 12, 13, 15, 17, 19, 20, 22, 24, 26, 28, 30, 32, 34, 35, 37, 39,
             41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 66, 68, 70, 72,
             74, 76, 78, 80, 82, 84]

    j = [ 9, 11, 12, 14, 15, 18, 19, 21, 22, 25, 26, 29, 30, 33, 34, 36, 37, 40,
             41, 44, 45, 48, 49, 52, 53, 56, 57, 60, 61, 64, 65, 67, 68, 71, 72,
             75, 76, 79, 80, 83, 84]

    k = [10, 11, 12, 16, 17, 18, 19, 23, 24, 25, 26, 31, 32, 33, 34, 38, 39, 40,
             41, 46, 47, 48, 49, 54, 55, 56, 57, 62, 63, 64, 65, 69, 70, 71, 72,
             77, 78, 79, 80, 85]

    l = [13, 14, 15, 16, 17, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34, 42, 43, 44,
             45, 46, 47, 48, 49, 58, 59, 60, 61, 62, 63, 64, 65, 73, 74, 75, 76, 
             77, 78, 79, 80]

    m = [20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 50, 51, 52, 
             53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 81, 82, 83, 84,
             85]

    n = [i for i in range(35, 66)]

    p = [i for i in range(66, 86)]
    
    q = [i for i in range(9, 86)]

    beta1 = 0
    for idx in i:
        beta1 ^= data[85-idx]

    beta2 = 0
    for idx in j:
        beta2 ^= data[85-idx]

    beta3 = 0
    for idx in k:
        beta3 ^= data[85-idx]

    beta4 = 0
    for idx in l:
        beta4 ^= data[85-idx]

    beta5 = 0
    for idx in m:
        beta5 ^= data[85-idx]

    beta6 = 0
    for idx in n:
        beta6 ^= data[85-idx]

    beta7 = 0
    for idx in p:
        beta7 ^= data[85-idx]

    beta8 = beta1 ^ beta2 ^ beta3 ^ beta4 ^ beta5 ^ beta6 ^ beta7
    for idx in q:
        beta8 ^= data[85-idx]

    data[85-8] = beta8
    data[85-7] = beta7
    data[85-6] = beta6
    data[85-5] = beta5
    data[85-4] = beta4
    data[85-3] = beta3
    data[85-2] = beta2
    data[85-1] = beta1

    return data

def relative_code(data):
    """
    Transform into relative code.

    This functions takes a stream of bits and encodes them with a relative code,
    that is, bit[i] = bit[i] ^ bit[i-1]
    """

    for i in range(1,len(data)):
        data[i] = data[i] ^ data[i-1]
    return data

def bibinary_code(data):
    """
    Transform into bi-binary code.

    This function takes a stream of bits and turns them into a bi-binary code,
    that is, 0 becomes 01 and 1 becomes 10.
    """

    dataout = []
    for d in data:
        dataout.append(d)
        dataout.append(d ^ 1)

    return dataout

def timestamp():
    """
    This function returns the precomputed timestamp.
    """
    
    timestamp = "111110001101110101000010010110"
    return [int(bit) for bit in timestamp]

def fixed_nav():
    """
    Generate a fixed navigation message.

    This function generates a fixed navigation message, copied from a test
    vector of https://github.com/gnss-sdr/gnss-sdr used in
    src/tests/unit-tests/system-parameters/glonass_gnav_nav_message_test.cc
    """

    navmsg = []
    strings = [""]*19
    # String 1
    strings[ 0] = "00001000000010000110010000111110110101011101000000101010110001000110101010110"
    strings[ 1] = "00001000000010000110010000111110110101011101000000101010110001000110101010110"
    strings[ 2] = "00001000000010000110010000111110110101011101000000101010110001000110101010110"
    strings[ 3] = "00001000000010000110010000111110110101011101000000101010110001000110101010110"
    strings[ 4] = "00001000000010000110010000111110110101011101000000101010110001000110101010110"
    # String 2-15
    strings[ 5] = "00010000100010010000010101011000010110010110000000101011001100000010111100001"
    strings[ 6] = "00011100000000010011010011101000111110110100110011010011011101100100111100111"
    strings[ 7] = "00100100001010111001000001000001000000000000000000000110001001000011001010101"
    strings[ 8] = "00101001000011000000000000000000000000001100111100011000000000000000011001000"
    strings[ 9] = "00110101001101000011001111000111000011010110000001101011110010000001011000111"
    strings[10] = "00111011010100010000100001101011111100001011010010111111101011101000101111000"
    strings[11] = "01000101001110000000011111100011010000001100000010001001110111000010101011110"
    strings[12] = "01001110100010010111000100000101000101011111010010111111101010111000101001010"
    strings[13] = "01010101010000000000111011111111011110010110000010001010100011000011110001101"
    strings[14] = "01011101110110110111000110011110111011110011010010111111110001101001000001100"
    strings[15] = "01100101010011000000111101101001101001000101000010001111100001000011100010101"
    strings[16] = "01101110111001001111101000010001101000100111010010111111101001001010100110100"
    strings[17] = "01110101010100000001000110000111101001101111000011101101000010000011110011010"
    strings[18] = "01111011101010100011101010101001111011000011010010111111111000101010100110010"
    
    for string in strings:
        bits = [int(bit) for bit in string]
        
        # Hamming Code
        bits = hamming_code(bits)

        # Relative Code
        bits = relative_code(bits)

        # Bi-binary Code
        bits = bibinary_code(bits)

        # Timestamp
        bits += timestamp()
       
        navmsg += bits

    return navmsg

def generate_signal(fs, duration, nav, satellites, chiprate, symbolrate):
    """
    Generate the desired GLONASS signal.

    This function generates a GLONASS (complex) signal for a given duration at a
    certain sampling rate. It can generate a simple C/A code or also include
    a navigation message. After adding the samples of each satellite, the
    resulting signal is normalized.
    """
    nsamples = int(fs * duration)
    signal = np.zeros(nsamples, dtype=np.complex64)
    code = ca()
    navmsg = fixed_nav()
    navlen = len(navmsg)

    for k in tqdm(satellites):
        print(k)
        # Time samples loop
        for i in tqdm(range(nsamples)):
            t = i / fs
            symbol = code[int(chiprate*t)%codelen]
            if nav:
                symbol ^= navmsg[int(symbolrate*t)%navlen]
            phi = 0 if symbol == 1 else -np.pi
            sample = np.exp(1j*(2*np.pi*k*df*t+phi))
            # sample = cmath.rect(1, 0 if symbol == 1 else -np.pi)
            signal[i] += sample
    # signal = 0.5 * (signal - np.min(signal)) / np.max(signal)
    return signal

### CLI ###

@click.group()
def cli():
    """
    Simple GLONASS simulation CLI.
    """

@cli.command()
def test():
    """
    Run some tests.

    This function runs some tests, but it is not exhaustive.
    """

    # Check the Hamming code generation
    true = [int(bit) for bit in
            "0000100000001000011001000011111011010101110100000010101011000100011010101011000101111"]

    computed = hamming_code([int(bit) for bit in
    "00001000000010000110010000111110110101011101000000101010110001000110101010110"])
    assert true == computed, "Hamming code test failed"
    print("Hamming code test passed")

@cli.command()
def ca_code():
    """
    Generate and print the C/A code.
    """
    print(''.join([str(chip) for chip in ca()]))

@cli.command()
@click.option("--fs", default=2.5e6, show_default=True,
              help="Sampling rate in [Hz].")
@click.option("--duration", default=codelen/511e3, show_default=True,
              help="Duration in [s].")
@click.option("--nav/--no-nav", default=False, show_default=True,
              help="Add navigation data or not.")
@click.option("--satellite", "-s", multiple=True, default=[1,2], show_default=True,
              help="Index of the satellite to generate.")
@click.option("--output-file-name", "-o", type=click.Path(dir_okay=False),
              default="/tmp/glonass", show_default=True,
              help="Name of the output file.")
@click.option("--cr", default=511e3, show_default=True,
              help="Chip rate in [chip/s]. Use 51100 for slow-10 mode.")
@click.option("--sr", default=100, show_default=True,
              help="Symbol rate in [symbol/s]. Use 10 for slow-10 mode.")
def signal(fs, duration, nav, satellite, output_file_name, cr, sr):
    """
    Generate the desired GLONASS signal.
    """
    chiprate = cr
    symbolrate = sr
    satellites = [int(s) for s in satellite]
    signal = generate_signal(fs, duration, nav, satellites, chiprate, symbolrate)
    signal.tofile(output_file_name)

if __name__ == "__main__":
    cli()
