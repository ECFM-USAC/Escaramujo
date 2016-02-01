CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
CFLAGS = -Wall -I$(CFITSIO) $(shell root-config --cflags) -O3
LIBS = -L$(CFITSIO) -lcfitsio -lm $(shell root-config --glibs) -lTreeViewer
GLIBS = 
GLIBS += 
OBJECTS = eficiencia.o 
HEADERS = 

ALL : eficiencia
	echo "Listo!"
	rm eficiencia.o

eficiencia : $(OBJECTS)
	$(CPP) $(OBJECTS) -o eficiencia $(LIBS) $(GLIBS) $(CFLAGS)

eficiencia.o : main.cpp $(HEADERS)
	$(CPP) -c main.cpp -o eficiencia.o $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
