CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
CFLAGS = -Wall -I$(CFITSIO) $(shell root-config --cflags) -O3
LIBS = -L$(CFITSIO) -lcfitsio -lm $(shell root-config --glibs) -lTreeViewer
GLIBS = 
GLIBS += 
OBJECTS = vidaMedia.o 
HEADERS = 

ALL : vidaMedia
	@echo "Compilación finalizada con éxito."

vidaMedia : $(OBJECTS)
	$(CPP) $(OBJECTS) -o vidaMedia $(LIBS) $(GLIBS) $(CFLAGS)

vidaMedia.o : main.cpp $(HEADERS)
	$(CPP) -c main.cpp -o vidaMedia.o $(CFLAGS)

clean:
	@rm -f *~ *.o *.exe
