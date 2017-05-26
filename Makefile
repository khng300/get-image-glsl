CXX=g++
INCLUDE=-I. -I include
LDFLAGS=-lglfw -ldl -lEGL

all: get_image

get_image: main.cpp common.h egl.o glfw.o
	$(CXX) -o $@ $(INCLUDE) $(LDFLAGS) $+

egl.o: egl.cpp egl.h
	$(CXX) -c $(INCLUDE) $?

glfw.o: glfw.cpp glfw.h glad.c
	$(CXX) -c $(INCLUDE) $?

PHONY: clean

clean:
	rm -f get_image *.o
