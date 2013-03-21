
all: tail-f seek

tail-f: tail-f.c tail-f.h debug.h

seek: seek.c debug.h

clean:
	rm -f *~
	rm -f *.o
	rm -f tail-f
	rm -f seek seek.bin

.PHONY: all clean
