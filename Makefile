PROG = spatialsimulator
MYLIB = libspatialsim.dylib
HEADERS = $(wildcard spatialsim/*.h)
ALL_SRCS := $(wildcard *.cpp)
SRCS := $(filter-out main.cpp, $(ALL_SRCS))
OBJS = $(SRCS:.cpp=.o)
CC = g++
CFLAGS = -Wall -Wextra -Weverything -g -c -O2 -fno-common -fPIC
HDFFLAGS = -I/opt/local/include
OPENCVFLAGS = `pkg-config --cflags opencv`
LDFLAGS = -L/usr/local/lib -lsbml -lz -L.
OPENCVLDFLAGS = `pkg-config --libs opencv`
HDFLDFLAGS = -L/opt/local/lib/hdf5-18/lib -lhdf5 -lhdf5_cpp
MYLIBFLAGS = -dynamiclib -install_name $(MYLIB) -compatibility_version 1.0 -current_version 1.0.0
MYLIBDIR = darwin/
MYJAR = libspatialsimj.jar
SBMLLIB = libsbml.5.dylib

.PHONY: all
all: $(PROG)
	@$(MAKE) deploy

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(HDFFLAGS) $(OPENCVFLAGS) -c $<

$(MYLIB): $(OBJS)
	$(CC) -o $@ $^ $(MYLIBFLAGS) $(LDFLAGS) $(OPENCVLDFLAGS) $(HDFLDFLAGS) -v
	install_name_tool -change $(SBMLLIB) ./$(SBMLLIB) $(MYLIB)

$(PROG): main.o $(MYLIB)
	$(CC) -o $@ main.o $(LDFLAGS) $(OPENCVLDFLAGS) $(HDFLDFLAGS) -lspatialsim

.PHONY: clean
clean:
	rm -rf $(PROG) $(OBJS) main.o $(MYLIB) $(MYJAR)

.PHONY: deploy
deploy:
	@echo "Creating jar"
	@if [ ! -d $(MYLIBDIR) ]; then\
	 @mkdir $(MYLIBDIR);\
	fi
	@cp $(MYLIB) $(MYLIBDIR)
	@jar cvf $(MYJAR) $(MYLIBDIR)
