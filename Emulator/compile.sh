#!/bin/bash

g++ -c Cpu.cpp -Wall -Wpedantic

g++ main.cpp -o Emulator Cpu.o -Wall -Wpedantic -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
