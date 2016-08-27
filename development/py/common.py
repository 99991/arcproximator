from __future__ import print_function
from fractions import Fraction
from collections import defaultdict
import time
import math
import inspect
import itertools
import functools

inf = float("inf")
colors = ['#00ff00', '#ff0000', '#0000ff', '#ffff00', '#ff00ff', '#00ffff']

def LINE():
    """Returns the current line number in our program."""
    return inspect.currentframe().f_back.f_lineno

def first(t):
    return t[0]

def second(t):
    return t[1]

@functools.total_ordering
class Point(object):
    
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

    def __hash__(self):
        return hash((self.x, self.y))

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

    def __neg__(self):
        return Point(-self[0], -self[1])

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

    def right(self):
        return Point(self[1], -self[0])

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

    def normalized(self):
        return self.scaled(1)

    def __str__(self):
        x, y = self.x, self.y
        return "(%f, %f, (%s, %s))"%(float(x), float(y), str(x), str(y))
    
    __repr__ = __str__

def find_exactly(values, value):
    for i, other in enumerate(values):
        if value is other:
            return i
    raise ValueError("Could not find", value, "in", values)

def remove_exactly(values, value):
    del values[find_exactly(values, value)]

class Segment(tuple):
    
    def __new__(typ, a, b):
        return tuple.__new__(typ, (a, b))

    def other(self, p):
        a, b = self
        return a if a != p else b

    def lerp(self, u=0.5):
        a, b = self
        return lerp(a, b, u)

class Arc(object):

    def __init__(self, center, a, b, clockwise=False):
        self.center = center
        self.a = a
        self.b = b
        self.clockwise = clockwise

    def radius(self):
        return self.a.dist(self.center)

    def sees(self, p):
        if self.clockwise:
            return p.is_left_of(self.a, self.b)
        else:
            return p.is_right_of(self.a, self.b)

    def reverse(self):
        a, b = self.a, self.b
        self.a = b
        self.b = a
        self.clockwise = not self.clockwise
    """
    def is_over_180(self):
        if self.clockwise:
            return self.b.is_right_of(self.a, self.center)
        else:
            return self.b.is_left_of(self.a, self.center)
    """
    def encloses(self, p):
        a, b, c = self.a, self.b, self.center
        if self.clockwise:
            # if arc is over 180 degrees
            if b.is_left_of(a, c):
                return p.is_right_of(c, a) and p.is_left_of(c, b)
            else:
                return p.is_right_of(c, a) or p.is_left_of(c, b)
        else:
            # if arc is over 180 degrees
            if b.is_left_of(a, c):
                return p.is_left_of(c, a) or p.is_right_of(c, b)
            else:
                return p.is_left_of(c, a) and p.is_right_of(c, b)

    def clamp(self, p):
        a, b, c = self.a, self.b, self.center
        if self.encloses(p):
            return c + (p - c).scaled(self.radius())
        else:
            if p.dist2(a) < p.dist2(b):
                return a
            else:
                return b

    def points(self, n):
        a, b, center, radius = self.a, self.b, self.center, self.radius()

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

def intersect(seg_ab, seg_cd, frac=True):
    a, b = seg_ab
    c, d = seg_cd
    
    if a == c or a == d or b == c or b == d:
        # ignore segments with same endpoints
        return []
    
    ba = b - a
    dc = d - c
    ca = c - a
    
    ba_det_dc = ba.det(dc)
    ca_det_dc = ca.det(dc)
    ca_det_ba = ca.det(ba)

    # parallel segments
    if ba_det_dc == 0:
        
        if ca_det_dc == 0:
            intersections = []
            
            ba2 = ba.dot(ba)
            dc2 = dc.dot(dc)
            
            if 0 <= ba.dot(c - a) <= ba2: intersections.append(c)
            if 0 <= ba.dot(d - a) <= ba2: intersections.append(d)
            if 0 <= dc.dot(a - c) <= dc2: intersections.append(a)
            if 0 <= dc.dot(b - c) <= dc2: intersections.append(b)
            
            return intersections
        else:
            # parallel, but not on same line
            return []

    if frac:
        t = Fraction(ca_det_ba, ba_det_dc)
        s = Fraction(ca_det_dc, ba_det_dc)
    else:
        t = ca_det_ba/ba_det_dc
        s = ca_det_dc/ba_det_dc
        
    if t >= 0 and t <= 1 and s >= 0 and s <= 1:
        return [a + s*ba]

    return []

def arc_from_points(a, b, c):
    # creates an arc through points (a, b, c)
    
    denominator = a.det(b) + b.det(c) + c.det(a)

    if denominator == 0:
        raise ValueError("Circle from points ambiguous: (%s, %s, %s)"%(str(a), str(b), str(c)))
    
    p = a.dot(a)*(c - b) + b.dot(b)*(a - c) + c.dot(c)*(b - a)

    center = 0.5/denominator * p.left()
    
    return Arc(center, a, c, b.is_left_of(a, c))

def arc_from_points_and_normal(a, b, a_normal, clockwise):
    ba = b - a
    d = ba.dot(a_normal)
    
    radius = ba.dot(ba)*0.5/d
    
    center = a + radius*a_normal
    
    return Arc(center, a, b, clockwise)

def rotation_and_center(a, b, tangent0, tangent1):
    co = tangent0.dot(tangent1)
    si = tangent0.det(tangent1)

    x = (a.x + b.x - si*(a.y - b.y)/(1.0 + co))*0.5
    y = (a.y + b.y + si*(a.x - b.x)/(1.0 + co))*0.5

    return (co, si, Point(x, y))

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
    points = filter(lambda p: p not in segment, points)
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
    radius = circle.radius()
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

def sweep_intersect(y, seg_ab):
    a, b = seg_ab
    
    if y < min(a.y, b.y) or y > max(a.y, b.y): return []

    if a.y == b.y: return []

    x = a.x + (y - a.y)*(a.x - b.x)/(a.y - b.y)

    return [Point(x, y)]

def identity(value):
    return value

def argmin(values, key=identity):
    return min(enumerate(points), key=lambda x: key(x[1]))[0]

def lerp(a, b, u):
    return (1 - u)*a + u*b

def rgb_to_hex(rgb):
    return "#" + "".join("%02x"%min(c*256, 255) for c in rgb)

def random_hex_color():
    rgb = colorsys.hsv_to_rgb(random.random(), 1.0, 1.0)
    return rgb_to_hex(rgb)

def flatten_once(values_values):
    return [value for values in values_values for value in values]

def make_xy(points):
    x = [p.x for p in points]
    y = [p.y for p in points]
    return [x, y]
