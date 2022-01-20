cc  = gcc
src = $(wildcard src/*.c)
arg = -lncurses
out = bin/yfile

build:
	mkdir -p bin
	$(cc) $(src) $(arg) -s -o $(out)

debug:
	mkdir -p bin
	$(cc) $(src) $(arg) -g -o $(out)