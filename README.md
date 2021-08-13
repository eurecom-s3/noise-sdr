# noise-sdr

Welcome to the Noise-SDR project!

### Noise-SDR in a nutshell

Noise-SDR uses a leakage-based fully-digital software-defined approach to shape
electromagnetic noise produces by unprivileged software into generic radio
signals, arbitrarily modulated in amplitude/frequency/phase. Despite some limitations on
power, frequency and bandwidth, it can transmit data with a variety of analog
and digital radio protocols. 
Noise-SDR currently runs on x86_64 (Linux and Windows), ARMV7A/ARMV8A (Android),
and MIPS32 (OpenWrt). Please check the academic paper for more details.

<img src="docs/images/overview.jpg" alt="Noise-SDR Overview" width="40%" height="auto"/>
<!--<img src="docs/images/overview2.jpg" alt="Noise-SDR Overview 2" width="40%" height="auto"/>-->
<!--<img src="docs/images/thor22-rtty45-psk125r-3.jpg" alt="Noise-SDR Example" width="40%" height="auto"/>-->

***Note:*** This is just an initial release. You can already find the code and how to
(cross)compile it. Running fldigi-noise-sdr is straightforward (check the help menu).
We assume you have some familiarity with running code on various platforms,
EM leakage, popular SDR tools and radio protocols
(many resources online are available). We plan to follow up with more tutorials
and more detailed instructions.

### Publications

Giovanni Camurati and Aurélien Francillon, "Noise-SDR: Arbitrary Modulation of Electromagnetic Noise from Unprivileged Software and Its Impact on Emission Security", to appear at IEEE S&P 2022.

### Authors

This work was developed at [EURECOM][eurecom] by [Giovanni Camurati][gc] (now at
[ETH Zurich][eth-syssec]) and
[Aurélien Francillon][af].

Please contact [camurati@eurecom.fr](mailto:camurati@eurecom.fr) for any
question.

### Acknowledgements

We would like to thank Andrea Possemato, Giulia Clerici, Matteo Guarrera, Elie Bursztein,
Jean-Michel Picod, the anonymous reviewers and the shepherd for their feedback,
help or interesting discussions.
This work was partially supported by a Google Faculty Research Award.

We are grateful for the many online resources and open-source tools that made
our work possible, in particular:
[Fldigi][fldigi],
[GNURadio][gnuradio],
[gr-lora_sdr][gr-lora_sdr],
[gnss-sdr][gnss-sdr],
[qsstv][qsstv],
[wsjt-x][wsjt-x],
[sigidwiki][sigidwiki],
[system-bus-radio][system-bus-radio], and
[drammer][drammer].
Please see the academic paper for more and more detailed citations.

### Install

You can either install Noise-SDR manually or use Vagrant and Virtualbox to
automatically create a virtual machine. In either case, look at 
```noise-sdr/vagrant/Vagrantfile``` and ```noise-sdr/vagrant/boostrap.sh``` for
self-explaining installation instructions. Note that the actual Noise-SDR code
is only a small part of this, the rest being popular SDR tools required, for
example, for reception.

```
git clone https://github.com/eurecom-s3/noise-sdr.git
cd noise-sdr/vagrant
vagrant up
vagrant ssh
```

### Usage

#### Transmit side

Once you have compiled Noise-SDR, run in on your machine, or push it to a phone
or other device to run it there (use the correct architecture).

```
fldigi-noise-sdr-XXXX --help
```

There is also an offline version of Noise-SDR.
In offline-noise-sdr you can find both a python script to generate RF-PWM timing
from IQ files
```
./rf-pwm.py generate --help
```
and a version of noise-sdr that reads these files
```
./offline-noise-sdr-XXXX --help
```

#### Receive side

Use your favorite tool for reception. For example, we use gqrx (and a USRP B210)
and Fldigi, using a virtual audio sink to connect them.

### Traces

You can find some pre-collected IQ traces [here][traces], and some pre-computed
RF-PWM files for offline-noise-sdr [here][rfpwm]. 
Precompiled binaries are [here][binary].

[eurecom]: http://www.eurecom.fr/en
[af]: http://s3.eurecom.fr/~aurel/
[gc]: https://giocamurati.github.io
[eth-syssec]: https://syssec.ethz.ch/
[traces]: https://1drv.ms/u/s!AqlwqphoQwZ5qSpScQH9IPzaBstx?e=dvnG1Z
[rfpwm]: https://1drv.ms/u/s!AqlwqphoQwZ5qSmq3aEv_CQwIx48?e=sVTFIY
[binary]: https://1drv.ms/u/s!AqlwqphoQwZ5qSjIfMp2wkCCqNYe?e=80meEv

[fldigi]: https://www.w1hkj.com/
[gnuradio]: https://www.gnuradio.org/
[gr-lora_sdr]: https://github.com/tapparelj/gr-lora_sdr
[gnss-sdr]: https://gnss-sdr.org/
[qsstv]: http://users.telenet.be/on4qz/
[wsjt-x]: https://physics.princeton.edu/pulsar/k1jt/wsjtx.html
[sigidwiki]: https://www.sigidwiki.com/wiki/Signal_Identification_Guide
[system-bus-radio]: https://github.com/fulldecent/system-bus-radio
[drammer]: https://github.com/vusec/drammer

