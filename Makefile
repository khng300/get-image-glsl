CXX=g++
INCLUDE=-I include
LDFLAGS=-lglfw -ldl

all: get_image

get_image: main.cpp common.h
	$(CXX) -o $@ $(INCLUDE) $(LDFLAGS) $?
