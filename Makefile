CXX = g++
CXXFLAGS = -std=c++11
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = test.out
LIB = bf-memory/bf-memory.a
INC = -I./bf-memory/include

all : lib
	$(CXX) $(CXXFLAGS) -g $(SRCS) $(LIB) $(INC)

lib : 
	cd bf-memory && make

clean :
	rm -f $(TARGET)
	rm -f *.o
	cd bf-memory && make clean