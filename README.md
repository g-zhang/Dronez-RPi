# Dronez-RPi
* Flight software program for Harris autonomous drone project (MDP 2014-2015)
* This is a single Unix makefile project that is meant to be compiled on the Raspberry Pi (Raspbian)

## Directories 
* /OnboardThreads - Contains modules and threads of the program

## Compiling
* First install prerequisites:

### Open CV Installation
```
sudo apt-get install build-essential
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev 
sudo apt-get install libopencv-dev
````

### Raspicam Installation
```
wget http://skylineservers.dl.sourceforge.net/project/raspicam/raspicam-0.1.3.zip
unzip raspicam-0.1.3.zip
cd raspicam-0.1.3.zip
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

* Then:
```
make
```
