CXX=g++
CFLAGS=-g
INCLUDE=-I. -I include
LDFLAGS=-lglfw -ldl -lEGL

all: get_image

get_image: main.cpp common.h egl.o glfw.o glad.o lodepng.o
	$(CXX) $(CFLAGS) -o $@ $(INCLUDE) $(LDFLAGS) $+

egl.o: egl.cpp egl.h
	$(CXX) $(CFLAGS) -c $(INCLUDE) $?

glfw.o: glfw.cpp glfw.h glad.c
	$(CXX) $(CFLAGS) -c $(INCLUDE) $?

glad.o: glad.c
	$(CXX) $(CFLAGS) -c $(INCLUDE) $?

lodepng.o: lodepng.cpp
	$(CXX) $(CFLAGS) -c $(INCLUDE) $?

PHONY: clean

clean:
	rm -f get_image *.o *.h.gch
