#!/bin/bash
g++ -std=c++11 -o ../build/main $@ -lglut -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lGLEW -lglfw -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system #-Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith   -Wmissing-declarations   -Winline   -Wundef   -Wcast-qual   -Wshadow   -Wwrite-strings   -Wno-unused-parameter   -Wfloat-equal   -pedantic  -ansi
cd ../build/
./main