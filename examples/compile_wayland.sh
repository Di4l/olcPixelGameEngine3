#!/usr/bin/bash

wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.c
wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.h
gcc -c xdg-shell.c

g++ -ggdb -fmax-errors=5 -std=c++20 -I. -lpng -lGL -lwayland-client -lwayland-egl -lEGL -Wall -lpthread $1.cpp xdg-shell.o -o $1
