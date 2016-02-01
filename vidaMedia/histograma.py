#!/usr/bin/python2
# coding: utf-8
import numpy as np
import math
import matplotlib
import matplotlib.pyplot as plt
from scipy.integrate import quad

datos=[]

entrada = open('filtro.dat', 'r')

for line in entrada:
    line = float(line.strip())
    datos.append(line)

n, bins,patches = plt.hist(datos,bins=27)
salida = open('salida.dat', 'w')

for i in range(0,27):
    if (0.5*bins[i]+0.5*bins[i+1] <=11000):
        print >> salida, 0.5*bins[i]+0.5*bins[i+1] , n[i]
