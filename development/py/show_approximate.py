from approximate import *

import Tkinter as tk
import random
import colorsys
import traceback

width = 800
height = 800

curve = CubicBezier([
    Point(100, 100),
    Point(100, 500),
    Point(400, 600),
    Point(300, 100),
])

curve = CubicBezier([
    Point(200, 100),
    Point(100, 100),
    Point(400, 200),
    Point(300, 200),
])

def foo(mouse):
    draw_line_strip(curve.points, 'grey')
    draw_line_strip(curve.sample())

    arcs = []
    subdivide(curve, 1.0, arcs)
    print("%d arcs"%len(arcs))
    for i, arc in enumerate(arcs):
        draw_arc(arc, color=colors[i % len(colors)])

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
