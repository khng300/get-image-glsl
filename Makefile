CXX=g++
INCLUDE=-I include
LDFLAGS=-lglfw -ldl -lEGL

all: get_image

get_image: main.cpp common.h egl.o
	$(CXX) -o $@ $(INCLUDE) $(LDFLAGS) $?

egl.o: egl.h egl.cpp
	$(CXX) -c $(INCLUDE) $?
