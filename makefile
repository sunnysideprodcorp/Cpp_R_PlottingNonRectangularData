CXX=g++
CXXFLAGS += -c -Wall -ggdb -std=c++11
LDFLAGS  += -ggdb


all: demo_exe

demo.o: demo.cpp
	$(CXX) $(CXXFLAGS) demo.cpp

demo_exe: demo.o
	$(CXX) -o demo_exe demo.o $(LDFLAGS) 

clean:
	rm -rf *o demo_exe
