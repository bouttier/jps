#!/usr/bin/env python2

from jps import *

m = Map((80, 20))
m.add_rectangle(2, 4, 2, 2)
m.add_circle(20, 10, 7)
print(m)

sol = compute(m, (3, 3), (25, 5))
print(sol)
