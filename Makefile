#DIRS
RPIDIR = ~/raspberrypi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/
ROADDIR = OnboardThreads/RoadDetection

#BINS
BINFILE = fly
RPIBIN = fly-RPi

# LIBS
OPENCV_LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui

# CPP FILES
MAINCPP = main.cpp
TESTCPP = tester.cpp
ROADDETECTIONCPP = OnboardThreads/RoadDetection.cpp
# List of .cpp files
CPPFILES = sharedvars.cpp OnboardThreads/*.cpp OnboardThreads/*.c
CPPFILES_NOCV = sharedvars.cpp $(filter-out $(ROADDETECTIONCPP), $(wildcard OnboardThreads/*.c*))


# COMPILER FLAGS
CFLAGS = -Wall -std=c++11 -O3

# FLAVORS
all:
	g++ $(CFLAGS) $(MAINCPP) $(CPPFILES) $(OPENCV_LIBS) -o $(BINFILE) -lpthread

nocv:
	g++ $(CFLAGS) $(MAINCPP) $(CPPFILES_NOCV) -o $(RPIBIN) -lpthread

test:
	g++ $(CFLAGS) $(TESTCPP) $(CPPFILES) $(OPENCV_LIBS) -o $(BINFILE) -lpthread

testnocv:
	g++ $(CFLAGS) $(TESTCPP) $(CPPFILES_NOCV) -o $(BINFILE) -lpthread

rpi:
	$(RPIDIR)arm-linux-gnueabihf-g++ $(CFLAGS) $(MAINCPP) $(CPPFILES_NOCV) -o $(RPIBIN) -lpthread


clean:
	rm -f $(BINFILE)
