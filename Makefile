CXX=g++
INCLUDE=-I include
LDFLAGS=-lglfw -ldl

all: get_image

get_image: main.cpp
	$(CXX) -o $@ $(INCLUDE) $(LDFLAGS) $?
