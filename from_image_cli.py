#!/usr/bin/env python2
from PIL import Image
from jps import *
import sys

if __name__ == "__main__":
	if len(sys.argv)<2:
		print "\nUtilisation :\n\t"+sys.argv[0]+" image.(png|jpeg)\n"
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

	print(map)
	sol = compute(map, (3, 3), (95, 95))
	print(sol)
