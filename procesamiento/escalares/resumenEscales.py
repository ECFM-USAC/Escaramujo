#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from datetime import datetime, timedelta
import numpy as np
import matplotlib.pyplot as plt
import select

Pressure 	= []
Temperature = []
OnePpsDly 	= []
CPU_Vcc 	= []
TimeStamp	= []
GpsVld 		= []
GPsSatNo 	= []
OnePpsTime 	= []
CodeVer 	= None
SerNum 		= None
TMCregs 	= []
Cntlregs 	= []

CH0scalar 	= []
CH1scalar 	= []
CH2scalar 	= []
CH3scalar 	= []
CoinScalar 	= []

CST_Delta = timedelta( hours = -6 )

N_args = len(sys.argv)
Data = []

Valid_lines = 0
Invalid_lines = []


def stats_print( Data_array ):
	print "Min\t\t" + str( np.amin( Data_array ) )
	print "Max\t\t" + str( np.amax( Data_array ) )
	print "Mean\t\t" + str( np.around( np.mean( Data_array ) ) )
	print "Desv\t\t" + str( np.around( np.std( Data_array ) ) )


def HCut_filter( Data_array, F_val ):
	FData = []
	for  Val in Data_array  :
		if ( Val <= F_val ) :
			FData.append( Val )
	return FData


def LCut_filter( Data_array, F_val ):
	FData = []
	for  Val in Data_array  :
		if ( Val >= F_val ) :
			FData.append( Val )
	return FData


def HLCut_filter( Data_array, L_val, H_val ):
	FData = LCut_filter( Data_array, L_val )
	FData = HCut_filter( FData, H_val )
	return FData


if( N_args > 1 ):
	for i in range( 1, N_args ):
		try:
			Archivo = open(sys.argv[i], "r")
		except:
			print "No se puede abrir " +  sys.argv[i]
			continue
		Data = Data + Archivo.readlines()
else:
	if ( select.select([sys.stdin,],[],[],0.0)[0] ):
		Data = sys.stdin.readlines()
	else:
		print "No hay datos de entrada"


N_lines = len( Data )

for Cur_line in range(0, N_lines ) :
	Fields_Cl = Data[ Cur_line ].split(' ', 1)
	if ( len( Fields_Cl ) > 1 ):
		if ( Fields_Cl[ 0 ] == 'ST' ):
			if ( ( Cur_line + 1 ) < N_lines ) :
				Fields_Nl = Data[ Cur_line + 1 ].split(' ', 1)
				if ( Fields_Nl[ 0 ] == 'DS' ) :
					OtherFields_Cl = Fields_Cl[ 1 ].split()
					OtherFields_Nl = Fields_Nl[ 1 ].split()
					if( ( len( OtherFields_Cl ) == 13 ) and ( len( OtherFields_Nl ) == 5 ) ):
						Valid_lines += 1
						Pressure.append( 	OtherFields_Cl[ 0] )
						Temperature.append(	OtherFields_Cl[ 1] )
#						OnePpsDly.append(	OtherFields_Cl[ 2] )
						CPU_Vcc.append(		int( OtherFields_Cl[ 3] )/1000.0 )
						TimeStamp.append(	datetime.strptime(OtherFields_Cl[ 4] + 
														OtherFields_Cl[ 5], "%H%M%S%d%m%y" ) )
#						GpsVld.append(		OtherFields_Cl[ 6] )
#						GPsSatNo.append(	OtherFields_Cl[ 7] )
#						OnePpsTime.append(	OtherFields_Cl[ 8] )
#						CodeVer 	= 		OtherFields_Cl[ 9]
#						SerNum 		= 		OtherFields_Cl[10]
#						TMCregs.append(		OtherFields_Cl[11] )
#						Cntlregs.append(	OtherFields_Cl[12] )
						CH0scalar.append(	int( OtherFields_Nl[0], 16 ) )
						CH1scalar.append(	int( OtherFields_Nl[1], 16 ) )
						CH2scalar.append(	int( OtherFields_Nl[2], 16 ) )
						CH3scalar.append(	int( OtherFields_Nl[3], 16 ) )
						CoinScalar.append(	int( OtherFields_Nl[4], 16 ) )
					else:
						#print "Información de estado incompleta"
						Invalid_lines.append( "II-" + str( Cur_line ) )
						#print Data[ Cur_line ]
				else:
					#print "No hay información de escalares"
					Invalid_lines.append( "SI-" + str( Cur_line ) )
					#print Data[ Cur_line ]
					
print "Input lines: " + str( N_lines )
print "Valid lines: " + str( Valid_lines )
print "Invalid lines: " + str( len( Invalid_lines ) )
print repr( Invalid_lines ) 

print "CH0"
stats_print( CH0scalar )
print "CH1"
stats_print( CH1scalar )
print "CH2"
stats_print( CH2scalar )

#Fil_Data = HCut_filter( CH2scalar, 4000 )

#stats_print( Fil_Data )

#plt.hist(Fil_Data, bins=100)
#plt.show()

# 	if ( not ST_ERROR ):
# 		TS_UTC = TimeStamp.strftime("%d%b%y %H:%M:%S ")
# 		TS_LT = ( TimeStamp + CST_Delta ).strftime("%d%b%y %H:%M:%S ")
# 		TS_NF_UTC = TimeStamp.strftime("%y%m%d%H%M%S ")
# 		TS_NF_LT = ( TimeStamp + CST_Delta ).strftime("%y%m%d%H%M%S ")
# 		TS_SECS = str( 	int( ( TimeStamp.day - First_time.day ) * 86400 ) +
# 						int( TimeStamp.hour ) 	* 3600 +
# 						int( TimeStamp.minute )	* 60 + 
# 						int( TimeStamp.second ) ) + ' '
# 		TimeStampFmt = TS_UTC + TS_NF_UTC
# 		print (	TimeStampFmt 		+ ' ' +
# 				Temperature 		+ ' ' +
# 				Pressure			+ ' ' +
# 				str( CH0scalar )	+ ' ' +
# 				str( CH1scalar )	+ ' ' +
# 				str( CH2scalar )	+ ' ' +
# 				str( CoinScalar )	)
# 	else:
# 		ST_ERROR = False