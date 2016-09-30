.PHONY: all

all:
	gcc -O3 -D_GNU_SOURCE cdext.c -o cdext
