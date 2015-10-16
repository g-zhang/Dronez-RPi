#location of raspberry pi cross compiler
RPIDIR = ~/raspberrypi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/
RPIBIN = fly-RPi

BINFILE = fly

MAINCPP = main.cpp
TESTCPP = tester.cpp
# List of .cpp files
CPPFILES = sharedvars.cpp OnboardThreads/*.cpp

CFLAGS = -Wall -std=c++11

all:
	g++ $(CFLAGS) $(MAINCPP) $(CPPFILES) -o $(BINFILE) -lpthread

test:
	g++ $(CFLAGS) $(TESTCPP) $(CPPFILES) -o $(BINFILE) -lpthread

rpi:
	$(RPIDIR)arm-linux-gnueabihf-g++ $(CFLAGS) $(MAINCPP) $(CPPFILES) -o $(RPIBIN) -lpthread

clean:
	rm -f $(BINFILE)
