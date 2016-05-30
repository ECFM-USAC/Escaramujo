CFITSIO = $(FITSIOROOT)
CPP = g++
CC = gcc
#CFLAGS = -Wall -I$(CFITSIO) $(shell root-config --cflags) -O3 -s
CFLAGS =  -I$(CFITSIO) $(shell root-config --cflags) -O3 -s
LIBS = -L$(CFITSIO) -lcfitsio -lm $(shell root-config --glibs) -lTreeViewer
GLIBS = 
GLIBS += 
OBJECTS = qNet2root6000.o 
HEADERS = 

.SILENT: *.o

ALL : qNet2root6000.sh
	@echo 'Desempaquetador compilado con éxito.'

qNet2root6000.sh : $(OBJECTS)
	@$(CPP) $(OBJECTS) -o qNet2root6000.sh $(LIBS) $(GLIBS) $(CFLAGS)

qNet2root6000.o : qNet2root6000.cpp $(HEADERS)
	@echo 'Compilando despempaquetador.'
	@$(CPP) -c qNet2root6000.cpp -o qNet2root6000.o $(CFLAGS)

clean:
	@rm -f *~ *.o *.exe

