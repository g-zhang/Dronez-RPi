BINFILE = fly

# List of .cpp files
CPPFILES = main.cpp OnboardThreads/*.cpp

CFLAGS = -Wall -std=c++11

all:
	g++ $(CFLAGS) $(CPPFILES) -o $(BINFILE) -lpthread

clean:
	rm -f $(BINFILE)
