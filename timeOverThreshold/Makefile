CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
CFLAGS = -Wall -I$(CFITSIO) $(shell root-config --cflags) -O3
LIBS = -L$(CFITSIO) -lcfitsio -lm $(shell root-config --glibs) -lTreeViewer
GLIBS = 
GLIBS += 
OBJECTS = tot.o 
HEADERS = 

ALL : tot
	echo "Listo!"
	rm tot.o

tot : $(OBJECTS)
	$(CPP) $(OBJECTS) -o tot $(LIBS) $(GLIBS) $(CFLAGS)

tot.o : main.cpp $(HEADERS)
	$(CPP) -c main.cpp -o tot.o $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
