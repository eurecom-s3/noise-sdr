python3.6 ../glonass-sim.py signal --fs 1.5e6 --duration 5 -s 0 --sr 100 --cr 511000 -o /tmp/glonass.complex 
python3.6 ../../../offline-noise-sdr/generate/rf-pwm.py generate --fs-in 1.5e6 --fs-out 3e6 --f-if 0.8750e6 --input-file /tmp/glonass.complex --output-file /tmp/glonass-example.rfpwm
