from __future__ import print_function
from fractions import Fraction
from collections import defaultdict
import time
import math
import inspect
import itertools
import functools
from math import sin, cos, pi, sqrt
from point import *

inf = float("inf")
colors = ['#00ff00', '#ff0000', '#0000ff', '#ffff00', '#ff00ff', '#00ffff']

def sign(x):
    if x < 0: return -1
    if x > 0: return +1
    return 0

def LINE():
    """Returns the current line number in our program."""
    return inspect.currentframe().f_back.f_lineno

def first(t):
    return t[0]

def second(t):
    return t[1]

class Segment(object):

    def __init__(self, a, b):
        self.a = a
        self.b = b

    def __iter__(self):
        yield self.a
        yield self.b
        raise StopIteration

    def __eq__(self, other):
        a, b = self
        return a == other.a and b == other.b
    
    def __ne__(self, other):
        a, b = self
        return a != other.a or b != other.b
    
    def other(self, p):
        a, b = self
        if a == p: return b
        if b == p: return a
        raise ValueError("%s must be endpoint of %s"%(str(p), str(self)))

    def endpoints(self):
        a, b = self
        return (a, b)

    def lerp(self, u=0.5):
        a, b = self
        return lerp(a, b, u)

    def split_at_points(self, points, segments):
        a, b = self
        ba = b - a
        ba2 = ba.dot(ba)
        def pseudo_dist_along_line(p):
            return ba.dot(p - a)
        points.sort(key=pseudo_dist_along_line)
        def on_line(p):
            d = pseudo_dist_along_line(p)
            return d > 0 and d < ba2
    
        points = list(filter(on_line, points))
        points.append(b)
        for b in points:
            segments.append(Segment(a, b))
            a = b

    def length_squared(self):
        a, b = self
        return a.dist_squared(b)

    def intersections_horizontal(self, p):
        a, b = self
    
        if p.y < min(a.y, b.y) or p.y >= max(a.y, b.y): return [[], [], []]

        if a.y == b.y: return [[], [], []]

        x = a.x + (p.y - a.y)*(a.x - b.x)/(a.y - b.y)
        
        q = Point(x, p.y)

        if x < p.x: return [[q], [], []]
        if x > p.x: return [[], [], [q]]
        return [[], [q], []]

    def __reversed__(self):
        b, a = self
        return Segment(a, b)

    def left_oriented(self):
        a, b = self
        if a.x < b.x: return self
        return Segment(b, a)

    def points(self, n):
        a, b = self
        return [lerp(a, b, 1.0/(n-1)*i) for i in range(n)]

    def set_endpoints(self, a, b):
        self.a = a
        self.b = b

    def tangent_vector(self, p):
        q = self.other(p)
        return q - p
    
    def __str__(self):
        a, b = self
        return "Seg(%s, %s)"%(str(a), str(b))
    
    __repr__ = __str__
    
class Arc(object):

    def __init__(self, center, a, b, clockwise=False):
        self.center = center
        self.a = a
        self.b = b
        self.clockwise = clockwise

    def endpoints(self):
        return (self.a, self.b)

    def set_endpoints(self, a, b):
        self.a = a
        self.b = b

    def other(self, p):
        a, b = self.a, self.b
        if a == p: return b
        if b == p: return a
        raise ValueError("%s must be endpoint of %s"%(str(p), str(self)))

    def tangent_vector(self, p):
        a, b, c = self.a, self.b, self.center
        if a == p:
            if self.clockwise:
                return (p - c).right()
            else:
                return (p - c).left()
        if b == p:
            if self.clockwise:
                return (p - c).left()
            else:
                return (p - c).right()
        raise ValueError("%s must be endpoint of %s"%(str(p), str(self)))
    
    def length_squared(self):
        return self.b.x - self.a.x

    def radius(self):
        return self.a.dist(self.center)

    def sees(self, p):
        if self.clockwise:
            return p.is_left_of(self.a, self.b)
        else:
            return p.is_right_of(self.a, self.b)

    def __reversed__(self):
        b, a = self.a, self.b
        return Arc(self.center, a, b, not self.clockwise)

    def left_oriented(self):
        if self.a.x < self.b.x:
            return self
        else:
            return reversed(self)
        
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

    def get_x_monotone_split_points(self, qa, qb, left, right, a, b):
        if self.clockwise:
            if qa == 0:
                if qb == 0: return [right, left] if a.y < b.y else []
                if qb == 1: return [right, left]
                if qb == 2: return [right]
                if qb == 3: return [right]
            if qa == 1:
                if qb == 0: return []
                if qb == 1: return [right, left] if a.x >= b.y else []
                if qb == 2: return [right]
                if qb == 3: return [right]
            if qa == 2:
                if qb == 0: return [left]
                if qb == 1: return [left]
                if qb == 2: return [left, right] if a.y >= b.y else []
                if qb == 3: return [left, right]
            if qa == 3:
                if qb == 0: return [left]
                if qb == 1: return [left]
                if qb == 2: return []
                if qb == 3: return [left, right] if a.y <= b.y else []
        else:
            if qa == 0:
                if qb == 0: return [left, right] if a.y >= b.y else []
                if qb == 1: return []
                if qb == 2: return [left]
                if qb == 3: return [left]
            if qa == 1:
                if qb == 0: return [left, right]
                if qb == 1: return [left, right] if a.y <= b.y else []
                if qb == 2: return [left]
                if qb == 3: return [left]
            if qa == 2:
                if qb == 0: return [right]
                if qb == 1: return [right]
                if qb == 2: return [right, left] if a.y <= b.y else []
                if qb == 3: return []
            if qa == 3:
                if qb == 0: return [right]
                if qb == 1: return [right]
                if qb == 2: return [right, left]
                if qb == 3: return [right, left] if a.y >= b.y else []

    def split_x_monotone(self, arcs):
        a, b, c, radius = self.a, self.b, self.center, self.radius()
        
        left = Point(c.x - radius, c.y)
        right = Point(c.x + radius, c.y)
        
        qa = (a - c).quadrant()
        qb = (b - c).quadrant()
        points = self.get_x_monotone_split_points(qa, qb, left, right, a, b)
        points = [p for p in points if p != a and p != b]
        points.append(b)
        for b in points:
            arcs.append(Arc(c, a, b, self.clockwise))
            a = b

    def split_at_points(self, points, arcs):
        points.sort(key=first)
        a, b, c = self.a, self.b, self.center
        points = [p for p in points if a.x < p.x < b.x]
        points.append(b)
        for b in points:
            if a != b:
                arcs.append(Arc(c, a, b, self.clockwise))
            a = b
        
    def intersections_horizontal(self, p):
        a, b, c = self.a, self.b, self.center
        r2 = a.dist2(c)

        dy = p.y - c.y
        if dy*dy > r2:
            return []

        return []

    def __str__(self):
        a, b, c = self.a, self.b, self.center
        return "Arc(%s, %s, %s, %d)"%(str(c), str(a), str(b), 1 if self.clockwise else 0)
    
    __repr__ = __str__

def find_exactly(values, value):
    for i, other in enumerate(values):
        if value is other:
            return i
    raise ValueError("Could not find", value, "in", values)

def remove_exactly(values, value):
    del values[find_exactly(values, value)]
    
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

def less_than_180(v):
    return v.y > 0 or (v.y == 0 and v.x >= 0)

def orientation(v, w):
    return cmp(v.det(w), 0)

def cmp_vector_circular(v, w):
    if less_than_180(v):
        if less_than_180(w): return orientation(w, v)
        else: return -1
    else:
        if less_than_180(w): return +1
        else: return orientation(w, v)
