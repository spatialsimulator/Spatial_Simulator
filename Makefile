PROG = main
CFLAGS = -Wall -Wextra -g -c -O2
OPENCVFLAGS = `pkg-config --libs opencv`
HDF5LFLAGS = -L/opt/local/lib/hdf5-18/lib -lhdf5 -lhdf5_cpp
CC = g++
OBJS = main.o setInfoFunction.o initializeFunction.o boundaryFunction.o astFunction.o freeFunction.o searchFunction.o calcPDE.o outputFunction.o checkStability.o outputImage.o outputHDF.o checkFunc.o

$(PROG): $(OBJS)
	$(CC) -L/usr/local/lib -lsbml -lz $(OPENCVFLAGS) $(HDF5LFLAGS) -o $(PROG) $(OBJS)

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
outputImage.o: outputImage.cpp
	$(CC) $(CFLAGS) `pkg-config --cflags opencv` outputImage.cpp
outputHDF.o: outputHDF.cpp
	$(CC) $(CFLAGS) -I/opt/local/lib/hdf5-18/include outputHDF.cpp
checkFunc.o: checkFunc.cpp
	$(CC) $(CFLAGS) checkFunc.cpp

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
