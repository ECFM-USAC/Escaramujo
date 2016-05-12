#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import signal
import sys
import threading
import time 
import serial
import numpy as np
import string

# Configuración del puerto serial

SERIAL_PORT_NAME 		= "/dev/ttyUSB0" #Cambiar al nombre del puerto serial que corresponda
SERIAL_PORT_BAUDRATE	= 115200
# Posibles valores: FIVEBITS, SIXBITS, SEVENBITS, EIGHTBITS
SERIAL_PORT_BYTE_SIZE	= serial.EIGHTBITS
# Posibles valores: PARITY_NONE, PARITY_EVEN, PARITY_ODD PARITY_MARK, PARITY_SPACE
SERIAL_PORT_PARITY		= serial.PARITY_NONE
# Posibles valores: STOPBITS_ONE, STOPBITS_ONE_POINT_FIVE, STOPBITS_TWO
SERIAL_PORT_STOPBITS	= serial.STOPBITS_ONE
SERIAL_PORT_TIMEOUT		= 5
SERIAL_PORT_SOFT_CTRL	= True
# para otras configuraciones ver documentación de pySerial

# Tamaño del buffer de datos (kB)
Data_buffer_size 	= 131072

# Configuración de los archivos de salida

File_name_prefix 	= "DC_ACB-"
File_name_ext 		= ".txt"

# Configuración de inicio de comunicación
INIT_CMDS = [ 'V1', 'V2', 'DG', 'ST 3 1' , 'CE' ]

# Configuración de fin de comunicación
END_CMDS = [ 'CD', 'ST 0' ]

# Código de caracter de fin de linea (tecla ENTER)
ENDLN_COD = 13

# Tiempo de espera entre comandos (segundos)
TIME_CMD_DLY = 2

# Configuración timers
# Timer 0
TIMER_0_ENABLE = False
TIMER_0_PERIOD = 20
TIMER_0_OFFSET = 0
TIMER_0_INSTRU = { 	's' }

# Timer 1
TIMER_1_ENABLE = False
TIMER_1_PERIOD = 20
TIMER_1_OFFSET = 10
TIMER_1_INSTRU = {  'C' }

# Inicia script
#
# Inician definiciones

def start_script():
#	os.system('clear')
#	os.system('setterm -cursor off')
	print "Enviando comandos iniciales..."
	global COLLECT 
	COLLECT = True
	indef_thread.start()
	time.sleep(1)
	for line in INIT_CMDS:
		Puerto.write( line + ENDLN_CHAR )
		print( '>> ' + line + ENDLN_CHAR )
		time.sleep( TIME_CMD_DLY )
	time.sleep(5)
	if (TIMER_0_ENABLE): timer0.start() 
	if (TIMER_1_ENABLE): timer1.start()


def stop_script():
	print "\nEnviando comandos finales..."
	global COLLECT
	for line in END_CMDS:
		Puerto.write( line + ENDLN_CHAR )
		print( '>> ' + line + ENDLN_CHAR )
		time.sleep( TIME_CMD_DLY )
	time.sleep(5)
	COLLECT = False
	print "\nFinalizando escritura de datos..."
	indef_thread.join()
	Puerto.close()
	print "Script finalizado correctamente"
#	os.system('setterm -cursor on')
	sys.exit(0)


def signal_term_handler(signal, frame):
    print '\nSIGTERM recibido, finalizando el script'
    stop_script()


def timer(Name, Wait_time, Offset, Instruct):
#	print Name + " inicializado"
	time.sleep(Offset)
	while True:
		time.sleep(Wait_time)
#		print time.strftime("%H%M%S")
		for line in Instruct:
			Puerto.writelines( line + ENDLN_CHAR )
			time.sleep( TIME_CMD_DLY )
 

def read_port():
	global Byte_count
	DataBuffer = [ None ] * Number_of_Bytes
	while(COLLECT):
		Byte_count = 0
		while( COLLECT and ( Byte_count < Number_of_Bytes )):
			DataBuffer[ Byte_count ] = Puerto.read(1)
#			sys.stdout.write( DataBuffer[ Byte_count ] )
			if ( DataBuffer[ Byte_count ] ):
				Byte_count += 1
#        		sys.stdout.write( Pgr_bar( Number_of_Bytes , Byte_count ) )
#        		sys.stdout.flush()
		File_name = File_name_prefix + time.strftime("%d%b%y_%H%M%S") + File_name_ext
		Archivo=open(File_name, 'w+')
		j = 0
		while( j < Byte_count ):
			if( DataBuffer[ j ] ): 
				Archivo.write( DataBuffer[ j ] )
			j += 1
		Archivo.close()
		if ( COLLECT ):
			sys.stdout.write( Pgr_bar( Number_of_Bytes , Byte_count ) )
			sys.stdout.flush()
		print "\n   -> " + File_name + "\n"

# The next code was downloaded from 
# http://stackoverflow.com/questions/566746/how-to-get-console-window-width-in-python

def getTerminalSize():
    import os
    env = os.environ
    def ioctl_GWINSZ(fd):
        try:
            import fcntl, termios, struct, os
            cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ,
        '1234'))
        except:
            return
        return cr
    cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
    if not cr:
        try:
            fd = os.open(os.ctermid(), os.O_RDONLY)
            cr = ioctl_GWINSZ(fd)
            os.close(fd)
        except:
            pass
    if not cr:
        cr = (env.get('LINES', 25), env.get('COLUMNS', 80))

        ### Use get(key[, default]) instead of a try/catch
        #try:
        #    cr = (env['LINES'], env['COLUMNS'])
        #except:
        #    cr = (25, 80)
    return int(cr[1]), int(cr[0])


def Pgr_bar(Max_val, Cur_val):
	(term_width, term_height) = getTerminalSize()
	label = "Buffering"
	fraction = Cur_val / (Max_val + 0.0)
	hash_width = term_width - 2*len(str(Max_val)) - len(label) - 18
	n_hash =  np.round( hash_width * fraction )
	h_string = ""
	for i in range(hash_width):
		if ( i <= n_hash ):
			h_string += "#"
		else:
			h_string += " "
	PgrBarStr =	"\r " + label + " [" + h_string + "] " + string.rjust( str( int( np.round( fraction * 100) ) ) , 3 ) + " % ( " + string.rjust( str( Cur_val ), len(str(Max_val))) + "/" + str( Max_val ) + " )"
	return PgrBarStr


# Inicia secuencia principal

COLLECT = False
ENDLN_CHAR = chr( ENDLN_COD )
Byte_count = 0
Number_of_Bytes = Data_buffer_size * 1024

signal.signal(signal.SIGTERM, signal_term_handler)

Puerto = serial.Serial(	port=SERIAL_PORT_NAME, 
						baudrate=SERIAL_PORT_BAUDRATE, 
						bytesize=SERIAL_PORT_BYTE_SIZE,  
						parity=SERIAL_PORT_PARITY, 
						stopbits=SERIAL_PORT_STOPBITS,
						timeout=SERIAL_PORT_TIMEOUT,
						xonxoff=SERIAL_PORT_SOFT_CTRL	)

indef_thread = threading.Thread(target=read_port)
indef_thread.daemon = True

# Timer 0
if (TIMER_0_ENABLE):
	timer0 = threading.Thread(target=timer, args = ("TIMER 0", TIMER_0_PERIOD, TIMER_0_OFFSET, TIMER_0_INSTRU))
	timer0.daemon = True

# Timer 1
if (TIMER_1_ENABLE):
	timer1 = threading.Thread(target=timer, args = ("TIMER 1", TIMER_1_PERIOD, TIMER_1_OFFSET, TIMER_1_INSTRU))
	timer1.daemon = True

My_pid = os.getpid()

print "\n\nATENCIÓN: ¡Este script va a correr indefinidamente!"
print " "
print "Para terminar, presione CTRL-C"
print "o ejecute \"kill " + repr(My_pid) + "\" desde la linea de comandos\n"

start_script()

print ""
try:
    while True:
        time.sleep(10)
        sys.stdout.write( Pgr_bar( Number_of_Bytes , Byte_count ) )
        sys.stdout.flush()
except KeyboardInterrupt:
    print "\n\nCRTL-C recibido, finalizando el script"
    stop_script()
