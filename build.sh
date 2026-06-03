#!/bin/sh
clang -std=c11 -Wall -Wextra -g0 -O0 src/main.c -lX11 -lGL -lm -o moottori_linux