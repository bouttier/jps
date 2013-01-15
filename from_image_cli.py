#!/usr/bin/env python2
#-*- coding: utf-8 -*-
from PIL import Image
from jps import *
import sys

if __name__ == "__main__":
    if len(sys.argv) !=2 and len(sys.argv) !=6:
        print "\nUtilisation :\n\t"+sys.argv[0]+" image.(png|jpeg) [xDepart yDepart xFin yFin]\n"
        sys.exit(1)


    mapImage = Image.open(sys.argv[1])

    width, height = mapImage.size
    map = Map((width, height))

    voidColor = mapImage.getpixel((1,1))
    count = 0
    for p in mapImage.getdata():
        x,y = count % width, count // width
        if p == voidColor:
            map.remove((x, y))
        else:
            map.add((x, y))
        count += 1

    if len(sys.argv) == 6:
        xBegin = int(sys.argv[2])
        yBegin = int(sys.argv[3])
        xEnd =  int(sys.argv[4])
        yEnd = int(sys.argv[5])
        if xBegin<0 or xBegin>=width or xEnd<0 or xEnd>=width\
                or yBegin<0 or yBegin>=height or yEnd<0 or yEnd>=height\
                or xBegin>xEnd or yBegin>yEnd:
            print(u"Les coordonnées que vous avez entrées sont incorrectes.")
            sys.exit(1)

    else:
        xBegin = 1
        yBegin = 1
        xEnd = width - 1
        yEnd = height - 1

    sol = compute(map, (xBegin, yBegin), (xEnd, yEnd))
    print(map)
    print(u"\nDépart  : ("+str(xBegin)+","+str(yBegin)+")")
    print(u"Arrivée : ("+str(xEnd)+","+str(yEnd)+")")
    if sol:
        print(sol)
    else:
        print("Il n'y a pas de solution.")
