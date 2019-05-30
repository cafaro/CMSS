CC=icpc
CFLAGS=-O3 -std=c++14
LDFLAGS=-lm
SOURCES=hotitems.cc prng.cc massdal.cc xxhash.cc countmin.cc rand48.cc gf.cc
EXECUTABLE=cmss

all:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LDFLAGS)

clean:
	rm -f *~ $(EXECUTABLE)
