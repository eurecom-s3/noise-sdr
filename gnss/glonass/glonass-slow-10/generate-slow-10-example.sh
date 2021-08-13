python3.6 ../glonass-sim.py signal --fs 80000 --duration 250 -s 0 --sr 10 --cr 51100 -o /tmp/glonass-slow-10.complex --nav
python3.6 ../../../offline-noise-sdr/generate/rf-pwm.py generate --fs-in 80e3 --fs-out 0.4e6 --f-if 56.25e3 --input-file /tmp/glonass-slow-10.complex --output-file /tmp/glonass-slow-10.rfpwm
