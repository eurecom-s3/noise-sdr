python3.6 gps-sim-slow-100.py
python3.6 ../../offline-noise-sdr/generate/rf-pwm.py generate --fs-in 80e3 --fs-out 0.4e6 --f-if 15e3 --input-file /tmp/time --output-file /tmp/gps-slow-100-sat24-f015e3.rfpwm
adb push /tmp/gps-slow-100-sat24-f015e3.rfpwm /data/local/tmp/
