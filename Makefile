
all: tail-f

tail-f: tail-f.o tail-f.h debug.h

clean:
	rm -f *~
	rm -f *.o
	rm -f tail-f

.PHONY: all clean
