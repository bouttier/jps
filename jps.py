#!/usr/bin/env python
import ctypes

libjps = ctypes.CDLL("libjps.so")

class Map:
    def __init__(self, _width, _height):
        self.jps = libjps
        self.width = _width
        self.height = _height
        width = ctypes.c_int(_width)
        height = ctypes.c_int(_height)
        self.map = ctypes.c_void_p(self.jps.map_create(width, height));

    def add(self, _x, _y):
        x = ctypes.c_int(_x)
        y = ctypes.c_int(_y)
        self.jps.map_add_point(self.map, x, y);

    def remove(self, _x, _y):
        x = ctypes.c_int(_x)
        y = ctypes.c_int(_y)
        self.jps.map_remove_point(self.map, x, y);

    def add_rectangle(self, _x, _y, _width, _height):
        x = ctypes.c_int(_x)
        y = ctypes.c_int(_y)
        width = ctypes.c_int(_width)
        height = ctypes.c_int(_height)
        self.jps.map_add_rectangle(self.map, x, y, width, height);

    def remove_rectangle(self, _x, _y, _width, _height):
        x = ctypes.c_int(_x)
        y = ctypes.c_int(_y)
        width = ctypes.c_int(_width)
        height = ctypes.c_int(_height)
        self.jps.map_del_rectangle(self.map, x, y, width, height);

    def add_circle(self, _x, _y, _radius):
        x = ctypes.c_int(_x)
        y = ctypes.c_int(_y)
        radius = ctypes.c_double(_radius)
        self.jps.map_add_circle(self.map, x, y, radius);

    def remove_circle(self, _x, _y, _radius):
        x = ctypes.c_int(_x)
        y = ctypes.c_int(_y)
        radius = ctypes.c_double(_radius)
        self.jps.map_remove_circle(self.map, x, y, radius);

    def print(self):
        libjps.map_print(self.map)

    def __del__(self):
        self.jps.map_destroy(self.map)

class Coord(ctypes.Structure):
    _fields_ = [("x", ctypes.c_int), ("y", ctypes.c_int)]

    def __str__(self):
        return "Coord(%d,%d)" %(self.x, self.y)
    

def compute(map, _start, _end):
    start = Coord(_start[0], _start[1])
    end = Coord(_end[0], _end[1])
    solArray = ctypes.POINTER(Coord)()
    solLen = libjps.jps_compute(map.map, start, end, ctypes.pointer(solArray))
    sol = list()
    for i in range(0, solLen):
        c = solArray[i]
        sol.append((c.x, c.y))
    sol.reverse()
    libjps.jps_free(ctypes.pointer(solArray))
    return sol

if __name__ == "__main__":
    m = Map(80, 20)
    m.add_rectangle(2, 4, 2, 2)
    m.add_circle(20, 10, 7)
    m.print()
    sol = compute(m, (3, 3), (25, 5))
    print(sol)
