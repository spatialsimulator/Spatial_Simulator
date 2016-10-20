PROG = spatialsimulator
MYLIB = libspatialsim.dylib
HEADERS = $(wildcard *.h)
ALL_SRCS := $(wildcard *.cpp)
SRCS := $(filter-out main.cpp, $(ALL_SRCS))
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
CC = g++
CFLAGS = -Wall -Wextra -Weverything -g -c -O2 -fno-common -fPIC
LDFLAGS = -L/usr/local/lib -lsbml -lz -L.
MYLIBFLAGS = -dynamiclib -install_name $(MYLIB) -compatibility_version 1.0 -current_version 1.0.0

.PHONY: all
all: $(PROG)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $<

$(MYLIB): $(OBJS)
	$(CC) -o $@ $^ $(MYLIBFLAGS) $(LDFLAGS)

$(PROG): main.o $(MYLIB)
	$(CC) -o $@ main.o $(LDFLAGS) -lspatialsim

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
