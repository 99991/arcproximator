from __future__ import print_function
from fractions import Fraction
import math
import itertools
import functools

@functools.total_ordering
class Point(object):
    
    def __init__(self, x=0, y=0):
        self.x = float(x)
        self.y = float(y)

    def __getitem__(self, index):
        if index == 0:
            return self.x
        if index == 1:
            return self.y
        raise ValueError("Index must be 0 for x or 1 for y, not %d"%index)

    def __lt__(self, other):
        if self[0] == other[0]:
            return self[1] < other[1]
        return self[0] < other[0]

    def __eq__(self, other):
        return self[0] == other[0] and self[1] == other[1]
    
    def __ne__(self, other):
        return self[0] != other[0] or self[1] != other[1]
    
    def __add__(self, other):
        return Point(self[0] + other[0], self[1] + other[1])

    def __sub__(self, other):
        return Point(self[0] - other[0], self[1] - other[1])

    def __mul__(self, other):
        return Point(self[0]*other, self[1]*other)

    __rmul__ = __mul__

    def is_now(self, other):
        self.x = other.x
        self.y = other.y

    def dot(self, other):
        return self[0]*other[0] + self[1]*other[1]

    def det(self, other):
        return self[0]*other[1] - self[1]*other[0]

    def left(self):
        return Point(-self[1], self[0])

    def is_right_of(self, a, b):
        return (self - a).det(b - a) > 0
    
    def is_left_of(self, a, b):
        return (self - a).det(b - a) < 0

    def length2(self):
        return self.dot(self)
    
    def length(self):
        return math.sqrt(self.length2())

    def dist2(self, other):
        return (self - other).length2()
    
    def dist(self, other):
        return (self - other).length()

    def angle(self):
        return math.atan2(self[1], self[0])

    def polar(self, angle, radius):
        return self + Point(radius*math.cos(angle), radius*math.sin(angle))

    def __str__(self):
        return "(%s, %s)"%(str(self[0]), str(self[1]))
    
    __repr__ = __str__

def calculate_slope(a, b):
    dx = b[0] - a[0]
    dy = b[1] - a[1]

    # must not be -inf so vertical segments
    # that go up or down are grouped together
    if dx == 0: return float("inf")

    return dy/dx
    #return Fraction(dy, dx)

current_x = 0

def calculate_y(a, b):
    global current_x
    
    slope = calculate_slope(a, b)

    if slope == float("inf"): return a[1]

    y_intercept = a[1] - slope*a[0]

    return slope*current_x + y_intercept

@functools.total_ordering
class Segment(tuple):
    
    def __new__(typ, a, b):
        return tuple.__new__(typ, (a, b))

    def current_y(self):
        return calculate_y(self[0], self[1])

    def slope(self):
        return calculate_slope(self[0], self[1])

    def __lt__(self, other):
        self_y = self.current_y()
        other_y = other.current_y()

        if self_y == other_y:
            self_slope = self.slope()
            other_slope = other.slope()

            #if self_slope == other_slope:

            return self_slope < other_slope
        
        return self_y < other_y

    def __eq__(self, other):
        if self.current_y() != other.current_y():
            return False
        if self.slope() != other.slope():
            return False
        return self is other

    def __ne__(self, other):
        if self.current_y() != other.current_y():
            return True
        if self.slope() != other.slope():
            return True
        return self is not other
        

class Arc(object):

    def __init__(self, center, radius, a, b, clockwise=False):
        self.center = center
        self.radius = radius
        self.a = a
        self.b = b
        self.clockwise = clockwise

    def points(self, n):
        a, b, center, radius = self.a, self.b, self.center, self.radius

        a_angle = (a - center).angle()
        b_angle = (b - center).angle()

        if self.clockwise:
            if b_angle >= a_angle:
                b_angle -= math.pi*2
        else:
            if b_angle <= a_angle:
                b_angle += math.pi*2

        delta = (b_angle - a_angle)/(n - 1)

        return [center.polar(a_angle + i*delta, radius) for i in range(n)]

def intersect(seg_ab, seg_cd):
    a, b = seg_ab
    c, d = seg_cd

    if a == c or a == d or b == c or b == d:
        # ignore segments with same endpoints
        return []
    
    bax = b[0] - a[0]
    bay = b[1] - a[1]
    dcx = d[0] - c[0]
    dcy = d[1] - c[1]

    det = bax*dcy - bay*dcx

    if det == 0: return []

    #det = Fraction(det)
    
    cax = c[0] - a[0]
    cay = c[1] - a[1]

    t = (cax*bay - cay*bax)/det
    s = (cax*dcy - cay*dcx)/det

    if t >= 0 and t <= 1 and s >= 0 and s <= 1:
        qx = a[0] + s*bax
        qy = a[1] + s*bay

        return [Point(qx, qy)]

    return []

def arc_from_points(a, b, c):
    # creates an arc through points (a, b, c)
    
    denominator = a.det(b) + b.det(c) + c.det(a)

    if denominator == 0:
        raise ValueError("Circle from points ambiguous: (%s, %s, %s)"%(str(a), str(b), str(c)))
    
    p = a.dot(a)*(c - b) + b.dot(b)*(a - c) + c.dot(c)*(b - a)

    center = 0.5/denominator * p.left()
    
    radius = (a - center).length()
    
    return Arc(center, radius, a, c, b.is_left_of(a, c))
"""
def swap(values, i, j):
    temp = values[i]
    values[i] = values[j]
    values[j] = temp

def insert_sorted(values, value):
    i = len(values)
    values.append(value)
    while i > 0 and values[i-1] > values[i]:
        swap(values, i, i-1)
        i -= 1
    return i

def compare_curves(current_point):

    x = current_point[0]
    
    def compare(seg_ab, seg_cd):
        a, b = seg_ab
        c, d = seg_ab
        y_ab = calculate_y(a, b, x)
        y_cd = calculate_y(c, d, x)

        if y_ab == y_cd:
            slope_ab = calculate_slope(*seg_ab)
            slope_cd = calculate_slope(*seg_cd)
            return slope_ab < slope_cd

        return y_ab < y_cd

    return compare
"""
from bintrees import AVLTree

def split_segment(segment, points):
    a, b = segment
    points.append(a)
    points.append(b)
    ba = b - a
    points.sort(key=lambda p: ba.dot(p - a))
    segments = []
    for a, b in zip(points, points[1:]):
        segments.append(Segment(a, b))
    return segments

def split_things(things, split_points, split_func):
    new_things = []
    
    for thing, points in zip(things, split_points):
        new_things.extend(split_func(thing, points))
    
    return new_things

def sweep(points):
    segments = []
    a = points[-1]
    for b in points:
        segments.append(Segment(a, b))
        a = b
    
    n = len(segments)
    split_points = [[] for _ in range(n)]

    all_intersections = [Point(0, 0)]
    for i in range(n):
        seg_ab = segments[i]
        for j in range(i+1, n):
            seg_cd = segments[j]
            intersections = intersect(seg_ab, seg_cd)
            """
            for intersection in intersections:
                closest = min(p in all_intersection, key=p.dist2(intersection))
                if intersection.dist2(closest) < 1e-10:
                    print("Merge")
            """
            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    segments = split_things(segments, split_points, split_segment)

    random.seed(1)
    for segment in segments:
        draw_line(segment[0], segment[1], random_hex_color())

    event_points = []
    for segment in segments:
        a, b = segment
        if a > b:
            a, b = b, a
        event_points.append((a, 0, segment))
        event_points.append((b, 1, segment))

    event_points.sort()

    n = len(segments)
    for i in range(n):
        ab = segments[i]
        for j in range(i+1, n):
            cd = segments[j]
            if ab == cd:
                print("Duplicate segment", i, j)
                print(ab)
                print(cd)
                print(ab.slope())
                print(cd.slope())
                print(ab.current_y())
                print(cd.current_y())
                import sys
                sys.exit(0)
    
    current_curves = AVLTree()
    for p, event_type, segment in event_points:
        global current_x
        current_x = p[0]
        
        if event_type == 0:
            current_curves.insert(segment, None)
        
        if event_type == 1:
            current_curves.remove(segment)


import Tkinter as tk
import random
import colorsys
import traceback

width = 800
height = 800

random.seed(1)
points = []
while len(points) < 10:
    x = random.randint(0, width)
    y = random.randint(0, height)
    p = Point(x, y)

    if not points or min(p.dist(q) for q in points) > 10:
        points.append(p)

master = tk.Tk()
canvas = tk.Canvas(master, width=width, height=height)
canvas.pack()

def draw_grid(dx=100, dy=100, color='gray'):
    for x in range(0, width, dx):
        canvas.create_line(x, 0, x, height, dash=(4, 4), fill=color)
    for y in range(0, height, dy):
        canvas.create_line(0, y, width, y, dash=(4, 4), fill=color)

#canvas.create_text(300, 300, text="hello, world!")

def draw_circle(center, radius, color='white', n = 100):
    a = center + Point(radius, 0)
    draw_arc(Arc(center, radius, a, a), color)

def draw_line(a, b, color='white'):
    canvas.create_line(float(a[0]), float(a[1]), float(b[0]), float(b[1]), fill=color)

def draw_arc(arc, color='white', n = 100):
    points = arc.points(n)
    for a, b in zip(points, points[1:]):
        draw_line(a, b, color)

def rgb_to_hex(rgb):
    return "#" + "".join("%02x"%min(c*256, 255) for c in rgb)

def random_hex_color():
    rgb = colorsys.hsv_to_rgb(random.random(), 1.0, 1.0)
    return rgb_to_hex(rgb)

def redraw():
    canvas.delete('all')
    canvas.create_rectangle(0, 0, width, height, fill='black')
    draw_grid()

    """
    random.seed(1)
    a = points[-1]
    for i, b in enumerate(points):
        draw_line(a, b, random_hex_color())
        a = b
    """

    try:
        sweep(points)
    except Exception as e:
        traceback.print_exc()

def identity(value):
    return value

def argmin(values, key=identity):
    return min(enumerate(points), key=lambda x: key(x[1]))[0]

def on_drag_left(event):
    p = Point(event.x, event.y)

    closest = min(points, key=lambda q: p.dist(q))
    closest.is_now(p)
    
    redraw()

redraw()
    
canvas.bind("<B1-Motion>", on_drag_left)

tk.mainloop()
