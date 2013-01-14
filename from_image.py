#!/usr/bin/env python2
from PIL import Image
from jps import *
import sys

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print "\nUtilisation:\n\t"+str(sys.argv[0])+" map\n"
		sys.exit(1)

	mapImage = Image.open(sys.argv[1])

	width, height = mapImage.size
	m = Map((width, height))

	voidColor = mapImage.getpixel((1,1))
	count = 0
	for p in mapImage.getdata():
		x,y = count % width, count // width
		if p == voidColor:
			m.remove((x, y))
		else:
			m.add((x, y))
		count += 1

	print(m)

	sol = compute(m, (1, 1), (39, 39))
	print(sol)
