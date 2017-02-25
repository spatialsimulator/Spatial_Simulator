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
OPENCVFLAGS = `pkg-config --cflags opencv`
OPENCVLD_PATH_FLAGS = `pkg-config --libs-only-L opencv`
OPENCVLD_LIB_FLAGS  = `pkg-config --libs-only-l opencv`
LDFLAGS = -L. -L/usr/local/lib -lsbml -lz

UNAME_S := $(shell uname -s)
# MacOSX + MacPorts
ifeq ($(UNAME_S),Darwin)
	CCFLAGS = -Wall -c -O2 -fno-common -fPIC
	LDFLAGS := -L/opt/local/lib $(LDFLAGS)
	HDFFLAGS = -I/opt/local/include
	#HDFLDFLAGS = -L/opt/local/lib/hdf5-18/lib -lhdf5 -lhdf5_cpp
	HDFLDFLAGS = -lhdf5 -lhdf5_cpp
	MYLIB = libspatialsim.dylib
	MYLIBFLAGS = -dynamiclib -install_name $(MYLIB) -compatibility_version $(VER_COMPAT) -current_version $(VER_CURRENT)
	MYLIBDIR = darwin/
	SBMLLIB = libsbml.5.dylib
	POST_LINK_CMD = install_name_tool -change $(SBMLLIB) ./$(SBMLLIB) $(MYLIB)
	INSTALL_PREFIX = /usr/local
endif
# Linux (Docker image)
ifeq ($(UNAME_S),Linux)
	#CCFLAGS = -Wall -c -O2 -fPIC -D_GLIBCXX_USE_CXX11_ABI=0
	CCFLAGS = -Wall -c -O2 -fPIC
	HDFFLAGS = -I/usr/include/hdf5/serial/
	HDFLDFLAGS = -lhdf5_cpp -lhdf5_serial
	MYLIB = libspatialsim.so
	MYLIBFLAGS = -shared -fPIC -Wl,-no-as-needed -Wl,-soname,$(MYLIB) #.$(VER_MAJOR)
	MYLIBDIR = linux-x86-64/
	SBMLLIB = libsbml.5.so
	POST_LINK_CMD = @echo "Skipping install_name_tool..."
	INSTALL_PREFIX = /usr
endif

MYJAR = libspatialsimj.jar

.PHONY: all
all: $(PROG)
	@$(MAKE) deploy

%.o: %.cpp $(HEADERS)
	$(CC) $(CCFLAGS) $(HDFFLAGS) $(OPENCVFLAGS) -c $<

$(MYLIB): $(OBJS)
	$(CC) -o $@ $^ $(MYLIBFLAGS) $(OPENCVLD_PATH_FLAGS) $(LDFLAGS) $(OPENCVLD_LIB_FLAGS) $(HDFLDFLAGS)
	$(POST_LINK_CMD)

$(PROG): main.o $(MYLIB)
	$(CC) -o $@ main.o $(OPENCVLD_PATH_FLAGS) -lspatialsim $(LDFLAGS) $(OPENCVLD_LIB_FLAGS) $(HDFLDFLAGS)

.PHONY: deploy
deploy: $(PROG)
	@echo "Creating jar"
	test -d $(MYLIBDIR) || mkdir $(MYLIBDIR)
	@cp $(MYLIB) $(MYLIBDIR)
	@jar cvf $(MYJAR) $(MYLIBDIR)

.PHONY: install
install: $(PROG)
	@echo "Install $(PROG) and $(MYLIB)"
	install -m 0755 $(PROG) $(INSTALL_PREFIX)/bin
	install -m 0644 $(MYLIB) $(INSTALL_PREFIX)/lib

.PHONY: uninstall
uninstall:
	@echo "Uninstall $(PROG) and $(MYLIB)"
	rm -f $(INSTALL_PREFIX)/bin/$(PROG)
	rm -f $(INSTALL_PREFIX)/lib/$(MYLIB)

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS) main.o $(MYLIB) $(MYJAR)

