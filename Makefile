# Application name
PROG = spatialsimulator

# Code
HEADERS = $(wildcard spatialsim/*.h)
ALL_SRCS := $(wildcard *.cpp)
SRCS := $(filter-out main.cpp, $(ALL_SRCS))
OBJS = $(SRCS:.cpp=.o)

# Version number
VER_MAJOR   := 1
VER_COMPAT  := $(VER_MAJOR).0
VER_CURRENT := $(VER_COMPAT).0

# Compiler/Linker flags
CC = g++
#CFLAGS = -Wall -g -c -O2 -fno-common -fPIC
CCFLAGS = -Wall -c -O2 -fno-common -fPIC
OPENCVFLAGS = `pkg-config --cflags opencv`
LDFLAGS = -L. -L/usr/local/lib -lsbml -lz
OPENCVLDFLAGS = `pkg-config --libs opencv`

UNAME_S := $(shell uname -s)
# MacOSX + MacPorts
ifeq ($(UNAME_S),Darwin)
	LDFLAGS := -L/opt/local/lib $(LDFLAGS)
	HDFFLAGS = -I/opt/local/include
	#HDFLDFLAGS = -L/opt/local/lib/hdf5-18/lib -lhdf5 -lhdf5_cpp
	HDFLDFLAGS = -lhdf5 -lhdf5_cpp
	MYLIB = libspatialsim.dylib
	MYLIBFLAGS = -dynamiclib -install_name $(MYLIB) -compatibility_version 1.0 -current_version 1.0.0
	MYLIBDIR = darwin/
	SBMLLIB = libsbml.5.dylib
	INSTALL_NAME_TOOL_CMD = install_name_tool -change $(SBMLLIB) ./$(SBMLLIB) $(MYLIB)
endif
# Linux (Docker image)
ifeq ($(UNAME_S),Linux)
	HDFFLAGS = -I/usr/include/hdf5/serial/
	HDFLDFLAGS = -lhdf5_cpp
	MYLIB = libspatialsim.so
	MYLIBFLAGS = -shared -fPIC -Wl,-soname,$(MYLIB).$(VER_MAJOR) -o $(MYLIB).$(VER_CURRENT)
	MYLIBDIR = linux-x86-64/
	SBMLLIB = libsbml.5.so
	INSTALL_NAME_TOOL_CMD = echo "Skipping install_name_tool..."
endif

MYJAR = libspatialsimj.jar

.PHONY: all
all: $(PROG)
	@$(MAKE) deploy

%.o: %.cpp $(HEADERS)
	$(CC) $(CCFLAGS) $(HDFFLAGS) $(OPENCVFLAGS) -c $<

$(MYLIB): $(OBJS)
	$(CC) -o $@ $^ $(MYLIBFLAGS) $(LDFLAGS) $(OPENCVLDFLAGS) $(HDFLDFLAGS) -v
	$(INSTALL_NAME_TOOL_CMD)

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
