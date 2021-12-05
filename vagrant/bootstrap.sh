#!/usr/bin/env bash

# update
sudo apt-get update

mkdir -p /home/vagrant/tools

# utils
sudo apt-get install -y \
    firefox \
    tmux \
    tig \
    python-pip \
    python3-pip \
    gtk+3.0 \
    pavucontrol \
    alsa \
    alsa-utils \
    adb \
    cmake

pip3 install \
   numpy==1.19.5 \
   matplotlib==3.3.4 \
   PyQt5==5.9.2 \
   pandas==1.1.5 \
   tabulate==0.8.9 \
   tqdm==4.48.0 \
   click==7.1.2


# sdr tools
sudo apt-get install -y \
    gnuradio \
    qsstv \
    fldigi \
    gqrx-sdr \


sudo apt-get install -y build-essential cmake git pkg-config libboost-dev \
   libboost-date-time-dev libboost-system-dev libboost-filesystem-dev \
   libboost-thread-dev libboost-chrono-dev libboost-serialization-dev \
   libboost-program-options-dev libboost-test-dev liblog4cpp5-dev \
   libuhd-dev gnuradio-dev gr-osmosdr libblas-dev liblapack-dev \
   libarmadillo-dev libgflags-dev libgoogle-glog-dev libhdf5-dev \
   libgnutls-openssl-dev libmatio-dev libpugixml-dev libpcap-dev \
   libprotobuf-dev protobuf-compiler libgtest-dev googletest \
   python3-mako python3-six

cd /home/vagrant/tools/
git clone https://github.com/gnss-sdr/gnss-sdr
cd gnss-sdr/build
git checkout 66a908fc42debcb7157acd773bccc056233bdc9a
cmake ..
make
sudo make install
sudo ldconfig


cd /home/vagrant/tools/
git clone https://github.com/gnss-sdr/gnss-sdr
sudo apt-get install -y gfortran libgfortran5 libqt5widgets5 libqt5network5 \
     libqt5printsupport5 libqt5multimedia5-plugins qtmultimedia5-dev libqt5serialport5-dev \
     libqt5sql5-sqlite libfftw3-single3 libgomp1 libboost-all-dev \
     libusb-1.0-0-dev qtbase5-dev qttools5-dev libudev-dev
wget https://physics.princeton.edu/pulsar/k1jt/wsjtx-2.4.0.tgz
tar -xvzf wsjtx-2.4.0.tgz
cd wsjtx-2.4.0/
mkdir build
cd build
cmake -DWSJT_SKIP_MANPAGES=ON -DWSJT_GENERATE_DOCS=OFF ../
cmake --build .
sudo cmake --build . --target install

# sdr dongles
sudo apt-get install -y \
    hackrf \
    uhd-host \
    rtl-sdr

sudo uhd_images_downloader

cd /etc/udev/rules.d
sudo wget https://raw.githubusercontent.com/keenerd/rtl-sdr/master/rtl-sdr.rules
sudo wget https://raw.githubusercontent.com/EttusResearch/uhd/master/host/utils/uhd-usrp.rules
sudo wget https://raw.githubusercontent.com/mossmann/hackrf/master/host/libhackrf/53-hackrf.rules
sudo udevadm control --reload-rules
sudo usermod -a -G plugdev vagrant


# toolchains
sudo apt-get install -y \
    clang \
    mingw-w64 

cd /home/vagrant/tools
wget https://dl.google.com/android/repository/android-ndk-r21d-linux-x86_64.zip
unzip android-ndk-r21d-linux-x86_64.zip
echo "export NDK=/home/vagrant/tools/android-ndk-r21d" >> /home/vagrant/.profile
source /home/vagrant/.profile



cd /home/vagrant/tools
wget https://pkg.8devices.com/carambola2/v2.9/OpenWrt-8devices-Toolchain-v2.9-ar71xx-generic_gcc-5.3.0_musl-1.1.14.Linux-x86_64.tar.bz2
tar -xf OpenWrt-8devices-Toolchain-v2.9-ar71xx-generic_gcc-5.3.0_musl-1.1.14.Linux-x86_64.tar.bz2
echo "export TOOLCHAIN_MIPS=/home/vagrant/tools/OpenWrt-8devices-Toolchain-v2.9-ar71xx-generic_gcc-5.3.0_musl-1.1.14.Linux-x86_64/toolchain-mips_34kc_gcc-5.3.0_musl-1.1.14/" >> /home/vagrant/.profile
source /home/vagrant/.profile

# noise-sdr
cd /home/vagrant/
ssh-keyscan -H github.com > ~/.ssh/known_hosts
git clone git@github.com:eurecom-s3/noise-sdr.git
cd noise-sdr
git checkout release

mkdir -p /vagrant/bin/

cd /home/vagrant/noise-sdr/fldigi-noise-sdr
make ARCH=x86
make ARCH=x86 OS=windows
make ARCH=v8 OP=CIVAC
make ARCH=v8 OP=ION
make ARCH=v7
make ARCH=mips
cp build/fldigi-* /vagrant/bin

cd /home/vagrant/noise-sdr/offline-noise-sdr/transmit
make ARCH=x86
make ARCH=x86 OS=windows
make ARCH=v8 OP=CIVAC
make ARCH=v8 OP=ION
make ARCH=v7
make ARCH=mips
cp build/offline-* /vagrant/bin

cd /home/vagrant/noise-sdr/gnuradio/
git clone git@github.com:giocamurati/gr-lora_sdr-noise-sdr.git
cd /home/vagrant/noise-sdr/gnuradio/gr-lora_sdr-noise-sdr
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
echo 'export PYTHONPATH=$PYTHONPATH:/home/vagrant/lora_sdr/lib/python2.7/dist-packages/' >> /home/vagrant/.profile
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/vagrant/lora_sdr/lib/' >> /home/vagrant/.profile
echo 'export GRC_BLOCKS_PATH=$GRC_BLOCKS_PATH:/home/vagrant/noise-sdr/gnuradio/gr-lora_sdr-noise-sdr/grc' >> /home/vagrant/.profile
source /home/vagrant/.profile
pip install pandas==0.24.2 unidecode==1.2.0

## Create null sink
pulseaudio --start
pacmd load-module module-null-sink sink_name=MySink

## Copy some configs
cd /home/vagrant/noise-sdr/
mkdir -p /home/vagrant/.config/gqrx/
mkdir -p /home/vagrant/.fldigi/
cp configs/gqrx-example-b210.conf /home/vagrant/.config/gqrx/
cp configs/fldigi_def_example.xml /home/vagrant/.fldigi/fldigi_def.xml
cp configs/WSJT-X-example.ini /home/vagrant/.config/WSJT-X.ini
