PROG = main
CFLAGS = -Wall -Wextra -g -c -O2
CC = g++
OBJS = main.o setInfoFunction.o initializeFunction.o boundaryFunction.o astFunction.o freeFunction.o searchFunction.o calcPDE.o outputFunction.o checkStability.o

$(PROG): $(OBJS)
	$(CC) -L/usr/local/lib -lsbml -lz -o $(PROG) $(OBJS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp
setInfoFunction.o: setInfoFunction.cpp
	$(CC) $(CFLAGS) setInfoFunction.cpp
initializeFunction.o: initializeFunction.cpp
	$(CC) $(CFLAGS) initializeFunction.cpp
boundaryFunction.o: boundaryFunction.cpp
	$(CC) $(CFLAGS) boundaryFunction.cpp
astFunction.o: astFunction.cpp
	$(CC) $(CFLAGS) astFunction.cpp
freeFunction.o: freeFunction.cpp
	$(CC) $(CFLAGS) freeFunction.cpp
searchFunction.o: searchFunction.cpp
	$(CC) $(CFLAGS) searchFunction.cpp
calcPDE.o: calcPDE.cpp
	$(CC) $(CFLAGS) calcPDE.cpp
outputFunction.o: outputFunction.cpp
	$(CC) $(CFLAGS) outputFunction.cpp
checkStability.o: checkStability.cpp
	$(CC) $(CFLAGS) checkStability.cpp

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
