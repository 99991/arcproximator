from common import *
import Tkinter as tk
import random
import colorsys
import traceback

width = 800
height = 800

class CubicBezier(object):
    def __init__(self, points):
        self.points = points

    def at(self, t):
        p0, p1, p2, p3 = self.points
        s = 1 - t
        return s*s*s*p0 + 3.0*s*s*t*p1 + 3.0*s*t*t*p2 + t*t*t*p3

    def sample(self, t0=0.0, t1=1.0, n=100):
        dt = (t1 - t0)/(n - 1.0)
        return [self.at(t0 + dt*i) for i in range(n)]

    def dist2(self, arc0, arc1, n=10):
        dt = 1.0/(n - 1)

        maximum = (-inf, None, None, None)

        arcs = [arc0, arc1]
        for i in range(n):
            t = dt*i
            p = self.at(t)
            d = min((p.dist2(q), t, p, q) for q in (arc.clamp(p) for arc in arcs))
            maximum = max(maximum, d)
            
        return maximum

    def split(self, t):
        a, b, c, d = self.points
        ab = lerp(a, b, t)
        bc = lerp(b, c, t)
        cd = lerp(c, d, t)

        abc = lerp(ab, bc, t)
        bcd = lerp(bc, cd, t)
        abcd = lerp(abc, bcd, t)

        curve0 = CubicBezier((a, ab, abc, abcd))
        curve1 = CubicBezier((abcd, bcd, cd, d))
        
        return (curve0, curve1)

curve = CubicBezier([
    Point(100, 100),
    Point(100, 500),
    Point(400, 600),
    Point(300, 100),
])

def arc_from_points_and_normal(a, b, a_normal, clockwise):
    ba = b - a
    d = ba.dot(a_normal)
    
    radius = ba.dot(ba)*0.5/d
    
    center = a + radius*a_normal
    
    return Arc(center, a, b, clockwise)

def subdivide(curve, max_err_squared, depth=10):
    p0, p1, p2, p3 = curve.points
    p10 = p1 - p0
    p21 = p2 - p1
    p23 = p2 - p3
    
    tangent0 = p10.normalized()
    tangent1 = p23.normalized()

    co = tangent0.dot(tangent1)
    si = tangent0.det(tangent1)

    x = (p0.x + p3.x - si*(p0.y - p3.y)/(1.0 + co))*0.5
    y = (p0.y + p3.y + si*(p0.x - p3.x)/(1.0 + co))*0.5

    center = Point(x, y)
    radius = p0.dist(center)

    join = curve.at(0.5)

    join = center + (join - center).scaled(radius)

    clockwise0 = join.is_right_of(p0, p1)
    clockwise1 = join.is_right_of(p3, p2)

    normal0 = tangent0.right() if clockwise0 else tangent0.left()
    normal1 = tangent1.right() if clockwise1 else tangent1.left()

    arc0 = arc_from_points_and_normal(p0, join, normal0, clockwise0)
    arc1 = arc_from_points_and_normal(p3, join, normal1, clockwise1)
    
    arc1.reverse()
    
    d2, t, p, q = curve.dist2(arc0, arc1)

    if depth == 0:
        print("Warning: max depth reached bug error still big")

        draw_arc(arc0, 'red')
        draw_arc(arc1, 'orange')
    else:
        if d2 < max_err_squared:
            draw_arc(arc0, 'yellow')
            draw_arc(arc1, 'green')
        else:
            curve0, curve1 = curve.split(t)
            subdivide(curve0, max_err_squared, depth - 1)
            subdivide(curve1, max_err_squared, depth - 1)

    """
    draw_line(d[2], d[3])
    
    draw_arc(arc0, 'red')
    draw_arc(arc1, 'green')

    draw_circle(join, 5, 'red')
    
    draw_circle(center, radius)
    """

def foo(mouse):
    draw_line_strip(curve.points, 'grey')
    draw_line_strip(curve.sample())
    
    subdivide(curve, 100)

master = tk.Tk()
canvas = tk.Canvas(master, width=width, height=height)
canvas.pack()

def draw_grid(dx=100, dy=100, color='gray'):
    for x in range(0, width, dx):
        canvas.create_line(x, 0, x, height, dash=(4, 4), fill=color)
    for y in range(0, height, dy):
        canvas.create_line(0, y, width, y, dash=(4, 4), fill=color)

def write(text, p, color='white', font=("Purisa", 20)):
    canvas.create_text(float(p[0]), height - 1 - float(p[1]), text=text, fill=color, font=font)

def draw_circle(center, radius, color='white', n=100):
    a = center + Point(radius, 0)
    draw_arc(Arc(center, radius, a, a), color)

def draw_line(a, b, color='white'):
    canvas.create_line(float(a[0]), height - 1 - float(a[1]), float(b[0]), height - 1 - float(b[1]), fill=color)

def draw_line_strip(points, color='white'):
    for a, b in zip(points[1:], points):
        draw_line(a, b, color)

def draw_arc(arc, color='white', n = 100):
    points = arc.points(n)
    for a, b in zip(points, points[1:]):
        draw_line(a, b, color)

def draw_polygon(points, color='white'):
    xy = flatten_once((float(p.x), height - 1 - float(p.y)) for p in points)
    canvas.create_polygon(xy, fill=color)

def redraw(mouse=Point(width/2, height/2)):
    canvas.delete('all')
    canvas.create_rectangle(0, 0, width, height, fill='black')
    draw_grid()

    try:
        foo(mouse)
    except Exception as e:
        traceback.print_exc()

def on_drag_left(event):
    mouse = Point(event.x, height - 1 - event.y)

    closest = min(curve.points, key=lambda p: mouse.dist(p))
    closest.is_now(mouse)
    
    redraw(mouse)

def on_move(event):
    mouse = Point(event.x, height - 1 - event.y)
    redraw(mouse)

redraw()

def on_closing():
    
    master.destroy()

master.protocol("WM_DELETE_WINDOW", on_closing)

canvas.bind("<B1-Motion>", on_drag_left)
canvas.bind("<Motion>", on_move)

tk.mainloop()
