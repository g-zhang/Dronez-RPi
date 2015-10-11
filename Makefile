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

clean:
	rm -f $(BINFILE)
