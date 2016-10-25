PROG = spatialsimulator
MYLIB = libspatialsim.dylib
HEADERS = $(wildcard *.h)
ALL_SRCS := $(wildcard *.cpp)
SRCS := $(filter-out main.cpp, $(ALL_SRCS))
OBJS = $(SRCS:.cpp=.o)
CC = g++
CFLAGS = -Wall -Wextra -Weverything -g -c -O2 -fno-common -fPIC
LDFLAGS = -L/usr/local/lib -lsbml -lz -L.
MYLIBFLAGS = -dynamiclib -install_name $(MYLIB) -compatibility_version 1.0 -current_version 1.0.0
MYLIBDIR = darwin/
MYJAR = libspatialsimj.jar

.PHONY: all
all: $(PROG)
	@$(MAKE) deploy

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $<

$(MYLIB): $(OBJS)
	$(CC) -o $@ $^ $(MYLIBFLAGS) $(LDFLAGS)

$(PROG): main.o $(MYLIB)
	$(CC) -o $@ main.o $(LDFLAGS) -lspatialsim

.PHONY: clean
clean:
	rm -rf $(PROG) $(OBJS) main.o $(MYLIB) $(MYJAR) $(MYLIBDIR)

.PHONY: deploy
deploy:
	@echo "Creating jar"
	@if [ ! -d $(MYLIBDIR) ]; then\
	 @mkdir $(MYLIBDIR);\
	fi
	@cp $(MYLIB) $(MYLIBDIR)
	@jar cvf $(MYJAR) $(MYLIB)
