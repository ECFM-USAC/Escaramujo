CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
CFLAGS = -Wall -I$(CFITSIO) $(shell root-config --cflags) -O3
LIBS = -L$(CFITSIO) -lcfitsio -lm $(shell root-config --glibs) -lTreeViewer
GLIBS = 
GLIBS += 
OBJECTS = programa.o 
HEADERS = 

ALL : programa
	echo "Listo!"
	rm programa.o

programa : $(OBJECTS)
	$(CPP) $(OBJECTS) -o programa $(LIBS) $(GLIBS) $(CFLAGS)

programa.o : main.cpp $(HEADERS)
	$(CPP) -c main.cpp -o programa.o $(CFLAGS)

clean:
	rm -f *~ *.o *.exe
