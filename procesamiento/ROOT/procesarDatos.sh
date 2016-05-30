#!/bin/bash
#Script para procesar datos del Proyecto Escaramujo
#Por andrés valle - ECFM USAC - Guatemala
#Útima revisión 12 - Mayo - 2016

#Los datos crudos se encuetran en el directorio "datos/"
#Los datos procesados se encuentran en el directorio "root/"
FILES=datos/*.txt

#Revisar si el desempaquetador ya está compilado
make -C desempaquetador/

#Revisar si existe la carpeta "root"
mkdir -p root

for f in $FILES
do

	 file=${f##*/} #Quitar la dirección del nombre del archivo 
	 base=${file%.*} #Quitar la extensión del nombre del archivo
	 ext=${file##*.} #Sólo la extensión
	
	#Sólo queremos analizar los archivos de datos en la carpeta
	#éstos tienen la extensión ".txt"
	if [ $ext == "txt" ]; then
		root="root/${base}.root" #Si el archivo ya fue procesado debe haber
										 #un archivo .root con el mismo nombre en la carpeta root/
		 
		#Revisar si el archivo ya fue procesado, 
		if [ ! -f $root ]; then
			desempaquetador/qNet2root6000.sh $f $root

		else 
			 echo "El archivo $base ya fue procesado."

		fi

	fi
	
done
