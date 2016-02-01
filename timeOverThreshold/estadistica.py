#!/usr/bin/python2
# coding: utf-8
import numpy
import sys

timeOverThreshold=[]
temp=[]

inputFile = open(sys.argv[1], 'r')

#Reading from file
for line in inputFile:
    line = line.strip()
    columns = line.split() 

    for i in range(len(columns)):
        temp.append(float(columns[i]))

    timeOverThreshold.append( temp )
    temp=[]

#Sorting Data
orderedData=sorted(timeOverThreshold,key=lambda x: x[0])

##
xValues=[]
for i in range(len(orderedData)):
    xValues.append(orderedData[i][0])

xValues = list(sorted(set(xValues)))
##

#Grouping data
yValues=[]
temp=[]
count=0
i=0

while i < len(orderedData) :
    if orderedData[i][0] == xValues[count] :
        temp.append(orderedData[i][1])
        if i == len(orderedData)-1:
            yValues.append(temp)
        i+=1

    else:
        yValues.append(temp)
        temp=[]
        count+=1

outputFile = open(sys.argv[2], 'w')

for i in range(len(xValues)):
    mean = numpy.mean(yValues[i])
    std = numpy.std(yValues[i])
    
    if len(yValues[i]) >= 3 :
        print >> outputFile, xValues[i], mean, std
