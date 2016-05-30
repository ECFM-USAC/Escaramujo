#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from datetime import datetime
import numpy

Archivo = open(sys.argv[1], "r")

Pressure 	= None
Temperature = None
OnePpsDly 	= None
CPU_Vcc 	= None
TimeStamp	= None
GpsVld 		= None
GPsSatNo 	= None
OnePpsTime 	= None
CodeVer 	= None
SerNum 		= None
TMCregs 	= None
Cntlregs 	= None

CH0scalar 	= None
CH1scalar 	= None
CH2scalar 	= None
CH3scalar 	= None
CoinScalar 	= None
			
ST_FOUND = False
ST_ERROR = False

for line in Archivo :
	Fields = line.split(' ', 1)
	if ( len( Fields ) > 1 ):
		if ( not ST_FOUND ):
			if ( Fields[ 0 ] == 'ST' ):
				ST_FOUND = True
				OtherFields	= Fields[ 1 ].split()
				if( len( OtherFields ) == 13 ):
					Pressure 	= OtherFields[ 0]
					Temperature	= OtherFields[ 1]
					OnePpsDly 	= OtherFields[ 2]
					CPU_Vcc 	= OtherFields[ 3]
					TimeStamp	= datetime.strptime(OtherFields[ 4] + 
													OtherFields[ 5], "%H%M%S%d%m%y" )
					GpsVld 		= OtherFields[ 6]
					GPsSatNo 	= OtherFields[ 7]
					OnePpsTime 	= OtherFields[ 8]
					CodeVer 	= OtherFields[ 9]
					SerNum 		= OtherFields[10]
					TMCregs 	= OtherFields[11]
					Cntlregs 	= OtherFields[12]
				else:
					#print "Información de estado incompleta"
					ST_ERROR = True
		else:
			ST_FOUND = False
			if ( Fields[ 0 ] == 'DS' ) :
				OtherFields = Fields[ 1 ].split()
				if( len( OtherFields ) == 5 ):
					CH0scalar 	= int( OtherFields[0], 16 )
					CH1scalar 	= int( OtherFields[1], 16 )
					CH2scalar 	= int( OtherFields[2], 16 )
					CH3scalar 	= int( OtherFields[3], 16 )
					CoinScalar 	= int( OtherFields[4], 16 )
				else:
					#print "Información de escalares incompleta"
					ST_ERROR = True
			else:
				#print "No hay información de escalares"
				ST_ERROR = True
			if ( not ST_ERROR ):
				TimeStampFmt = TimeStamp.strftime("%d%b%y ") + str( int( TimeStamp.hour ) 	* 3600 +
																	int( TimeStamp.minute )	* 60 + 
																	int( TimeStamp.second ) )
				print (	TimeStampFmt 		+ ' ' +
						Temperature 		+ ' ' +
						Pressure			+ ' ' +
						str( CH0scalar )	+ ' ' +
						str( CH1scalar )	+ ' ' +
						str( CH2scalar )	)
			else:
				ST_ERROR = False