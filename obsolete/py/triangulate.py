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

    def scaled(self, new_length):
        return new_length/self.length() * self

    def __str__(self):
        return "(%s, %s)"%(str(self[0]), str(self[1]))
    
    __repr__ = __str__

class Segment(tuple):
    
    def __new__(typ, a, b):
        return tuple.__new__(typ, (a, b))

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

def intersect_segment_circle(segment, circle):
    center = circle.center
    radius = circle.radius
    a, b = segment
    
    ba = b - a
    ba2 = ba.dot(ba)
    ca = center - a
    d = ba.dot(ca)
    
    f = d*d - ba2*(ca.dot(ca) - r*r)

    if f < 0:
        return []

    if f == 0:
        return []
        # TODO need this or not?
        # return [d/ba2]

    # f > 0
    g = math.sqrt(f)/ba2
    return [d - g, d + g]

def get_intersections(p, q, segments):
    pq = Segment(p, q)

    all_intersections = []
    
    for segment in segments:
        a, b = segment
        # ignore horizontal segments
        if a.y == b.y:
            continue
        
        intersections = intersect(pq, segment)

        for intersection in intersections:
            # let the point 'a' be the lower one
            if a.y > b.y:
                a, b = b, a
            
            # segments are half-open
            # so we ignore the upper segment point
            if intersection == b:
                continue

            all_intersections.append(intersection)

    return all_intersections

def sweep(mouse, points):
    draw_polygon(points, '#222222')
    
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
            
            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    segments = split_things(segments, split_points, split_segment)

    random.seed(1)
    for segment in segments:
        draw_line(segment[0], segment[1], random_hex_color())

    def fill(segment, delta):
        a, b = segment
        a_inter = get_intersections(a, a + delta, segments)

        a_inside = len(a_inter) & 1 == 1

        if a_inside:
            if delta.x > 0:
                a2 = min(a_inter)
                draw_line(a, a2, color='white')
                return [[a, a2]]
            else:
                a2 = max(a_inter)
                draw_line(a, a2, color='green')
                return [[a, a2]]
        
        return []
        

    new_segments = []
    for segment in segments:
        new_segments.extend(fill(segment, Point(+1000, 0)))
        new_segments.extend(fill(segment, Point(-1000, 0)))
    
    segments.extend(new_segments)

import Tkinter as tk
import random
import colorsys
import traceback

width = 800
height = 800

with open("star.txt", "rb") as f:
    s = f.read()
    xy = list(map(float, s.split()))
    xs = xy[0::2]
    ys = xy[1::2]
    points = [Point(x, y) for x, y in zip(xs, ys)]
"""
random.seed(1)
points = []
while len(points) < 10:
    x = random.randint(0, width)
    y = random.randint(0, height)
    p = Point(x, y)

    if not points or min(p.dist(q) for q in points) > 10:
        points.append(p)
"""
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

def flatten_once(values_values):
    return [value for values in values_values for value in values]

def draw_polygon(points, color='white'):
    xy = flatten_once((p.x, p.y) for p in points)
    canvas.create_polygon(xy, fill=color)

def redraw(mouse=Point(width/2, height/2)):
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
        sweep(mouse, points)
    except Exception as e:
        traceback.print_exc()

def identity(value):
    return value

def argmin(values, key=identity):
    return min(enumerate(points), key=lambda x: key(x[1]))[0]

def on_drag_left(event):
    mouse = Point(event.x, event.y)

    closest = min(points, key=lambda p: mouse.dist(p))
    closest.is_now(mouse)
    
    redraw(mouse)

redraw()

def on_closing():

    with open("points.txt", "wb") as f:
        s = "\n".join(str(p.x) + " " + str(p.y) for p in points)
        f.write(s)
    
    master.destroy()

master.protocol("WM_DELETE_WINDOW", on_closing)

canvas.bind("<B1-Motion>", on_drag_left)

tk.mainloop()
