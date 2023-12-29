all: defrag.c
	cc -g -Wall -O3 -DDEBUG=$(DEBUG) -o defrag defrag.c disk_image.c

clean:
	rm defrag disk_defrag

