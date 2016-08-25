from __future__ import print_function
from fractions import Fraction
from collections import defaultdict
import time
import math
import itertools
import functools

def second(t):
    return t[1]

@functools.total_ordering
class Point(object):
    
    def __init__(self, x=0, y=0):
        self.x = Fraction(x)
        self.y = Fraction(y)

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
    
    def __new__(typ, a, b, i=0):
        segment = tuple.__new__(typ, (a, b))
        segment.i = i
        return segment

    def other(self, p):
        a, b = self
        return a if a != p else b

    def middle(self):
        a, b = self
        return 0.5*(a + b)

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

    inv_det = Fraction(1, det)

    t = (cax*bay - cay*bax)*inv_det
    s = (cax*dcy - cay*dcx)*inv_det

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

def sweep_intersect(y, seg_ab):
    a, b = seg_ab
    
    if y < min(a.y, b.y) or y > max(a.y, b.y): return []

    if a.y == b.y: return []

    x = a.x + (y - a.y)*(a.x - b.x)/(a.y - b.y)

    return [Point(x, y)]

def triangulate(mouse, points):
    draw_polygon(points, '#222222')
    
    segments = []
    a = points[-1]
    for b in points:
        segments.append(Segment(a, b))
        a = b
    
    split_points = [[] for _ in range(len(segments))]

    # TODO split overlapping line segments properly

    all_intersections = [Point(0, 0)]
    for i in range(len(segments)):
        seg_ab = segments[i]
        for j in range(i+1, len(segments)):
            seg_cd = segments[j]
            intersections = intersect(seg_ab, seg_cd)
            
            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    segments = split_things(segments, split_points, split_segment)

    new_segments = []
    split_points = [[] for _ in range(len(segments))]

    # TODO handle case with overlapping sweep lines

    t = time.clock()
    
    vertices = list(sorted(set(a for a, b in segments), key=second))
    
    for c in vertices:
        intersections = []
        
        for i in range(len(segments)):
            
            seg_ab = segments[i]
            a, b = seg_ab
            
            # ignore horizontal segments
            if a.y == b.y:
                return
            
            for intersection in sweep_intersect(c.y, seg_ab):
                # segments are half-open
                # so we ignore the upper segment point
                if intersection.y == max(a.y, b.y):
                    continue

                intersections.append((intersection, i))

        left  = [(p, i) for p, i in intersections if p.x < c.x]
        right = [(p, i) for p, i in intersections if p.x > c.x]

        if len(left) & 1:
            p, i = max(left)
            #draw_line(c, p)
            new_segments.append(Segment(p, c))
            new_segments.append(Segment(p, c))
            split_points[i].append(p)
        
        if len(right) & 1:
            p, i = min(right)
            #draw_line(c, p)
            new_segments.append(Segment(p, c))
            new_segments.append(Segment(p, c))
            split_points[i].append(p)

    dt = time.clock() - t
    print(dt)

    segments = split_things(segments, split_points, split_segment)

    colors = ['#00ff00', '#ff0000', '#0000ff', '#ffff00', '#ff00ff', '#00ffff']

    segments.extend(new_segments)

    # remove zero length segments
    segments = [segment for segment in segments if segment[0].dist2(segment[1]) > 0]
    for i in range(len(segments)):
        segments[i].i = i

    vertex_edges = defaultdict(list)
    for segment in segments:
        a, b = segment
        vertex_edges[a].append(segment)
        vertex_edges[b].append(segment)

    for p, edges in vertex_edges.items():
        def key(edge):
            return (edge.other(p) - p).angle()
        edges.sort(key=key)

        # degree of each vertex must be even
        assert(len(edges) & 1 == 0)
    
    def draw_edges(edges, p):
        ab = edges[-1]
        for i, cd in enumerate(edges):
            a = ab.other(p)
            b = cd.other(p)
            radius = 30
            draw_circle(p.polar((a - p).angle(), radius), 5, color='grey')
            draw_circle(p.polar((b - p).angle(), radius), 5, color='grey')
            arc = Arc(p, radius, a, b)
            draw_arc(arc)
            points = arc.points(5)
            write(str(i), points[1])
            ab = cd

    if 0:
        #p = Point(Fraction(12152225082, 25102391), Fraction(40686091, 166241))
        p = Point(134, 514)
        edges = vertex_edges[p]
        for edge in vertex_edges[p]:
            print(edge)
        print(len(edges))
    
    def eat_face(debug):
        points = (p for segment in segments for p in segment)
        p = min(points, key=second)
        start = p
        edges = vertex_edges[p]
        """
        if debug:

            print("start at", p)
            for seg in segments:
                print(seg)
            for edge in edges:
                print((edge.other(p) - p).angle(), edge)
        """
        edge = edges[0]

        result = []

        max_face_size = 50
        for i in range(max_face_size):
            #print("Current edge:", edge.i, edge)
            # remove outgoing edge
            #print("edges:", edges)
            
            remove_exactly(edges, edge)
            remove_exactly(segments, edge)
            result.append(p)

            if debug and False:
                draw_line(edge[0], edge[1], color='gray')
                draw_circle(p, 5)
                write(str(i), edge.middle())
                #print(p, edge.other(p))

            
            p = edge.other(p)
            edges = vertex_edges[p]
            #print("new edges:", edges)
            index = find_exactly(edges, edge)
            assert(edges[index] is edge)
            del edges[index]
            
            if p == start:
                #print("Reached goal!")
                break
            
            for j in range(len(edges)):
                prev_edge = edges[index - j - 1]
                if prev_edge != edge:
                    #print("Found different edge: ", prev_edge.i, prev_edge)
                    break
            else:
                raise Exception("Could not find previous edge of", edge, "in", edges)
            # remove incomming edge
            edge = prev_edge
        else:
            raise Exception("max_face_size reached")

        return result

    #for i in range(6):
    while segments:
        try:
            points = eat_face(True if i == 666 else False)
        except Exception as e:
            traceback.print_exc()

        draw_polygon(points)
        
        if i == 666:
            for j, edge in enumerate(edges):
                write(str(j), edge.middle())
                draw_line(*edge)

    if 0:
        print("unconsumed segments:")
        for segment in segments:
            print(segment)
    
    if 1:
        random.seed(1)
        for i, segment in enumerate(segments):
            draw_line(segment[0], segment[1], colors[i % len(colors)])

    #print("done")

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
while len(points) < 5:
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

def write(text, p, color='white'):
    canvas.create_text(float(p[0]), height - 1 - float(p[1]), text=text, fill=color)

def draw_circle(center, radius, color='white', n=100):
    a = center + Point(radius, 0)
    draw_arc(Arc(center, radius, a, a), color)

def draw_line(a, b, color='white'):
    canvas.create_line(float(a[0]), height - 1 - float(a[1]), float(b[0]), height - 1 - float(b[1]), fill=color)

def lerp(a, b, u):
    return (1 - u)*a + u*b

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
    xy = flatten_once((float(p.x), height - 1 - float(p.y)) for p in points)
    canvas.create_polygon(xy, fill=color)

def redraw(mouse=Point(width/2, height/2)):
    canvas.delete('all')
    canvas.create_rectangle(0, 0, width, height, fill='black')
    draw_grid()

    try:
        triangulate(mouse, points)
    except Exception as e:
        traceback.print_exc()

def identity(value):
    return value

def argmin(values, key=identity):
    return min(enumerate(points), key=lambda x: key(x[1]))[0]

def on_drag_left(event):
    mouse = Point(event.x, height - 1 - event.y)

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
