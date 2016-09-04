# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi
import math
from common import *

fig = plt.figure()

g = fig.add_subplot(1, 1, 1)

def degrees(p, center):
    return  math.degrees((p - center).angle())

def draw_arc(arc):
    angle0 = degrees(arc.a, arc.center)
    angle1 = degrees(arc.b, arc.center)
    width = 2*arc.radius()
    if arc.clockwise:
        angle1, angle0 = angle0, angle1
    g.add_patch(patches.Arc(arc.center, width, width, 0.0, angle0, angle1))

def draw_line(a, b, ls='-'):
    plt.plot([a.x, b.x], [a.y, b.y], ls=ls, color='black')

def draw_dot(p):
    plt.plot([p.x], [p.y], 'o', lw=1, color='gray', ms=10)

a = Point(0, 0)
b = Point(34, 34)
ba = b - a

arc0 = Arc(a + ba.right(), a, b + ba.right(), True)
arc1 = Arc(a - ba.right(), a, a - 2*ba.right(), False)

draw_line(a, b, '-')
draw_line(a, a + Point(0, 100), 'dotted')
draw_line(a, a - Point(0, 100), 'dotted')
c = a.polar(pi*-0.45, 70)
draw_line(a, c, '-')
draw_line(a, a.polar(pi*-0.3, 30))
draw_line(a, a.polar(pi*-0.1, 50))

draw_arc(arc0)
draw_arc(arc1)

#plt.plot([j.x], [j.y], 'o', lw=1, color='gray', ms=10)

draw_dot(a)
g.text(a.x - 10, a.y - 11, r'$\mathbf{p}$', fontsize=20)
g.text(a.x - 70, a.y + 5, r'ray', fontsize=20)
g.arrow(a.x, a.y, -80, 0, head_width=5, head_length=5, fc='k', ec='k')

"""
g.annotate(
    'Bézier curve'.decode("utf-8"),
    xy=(6, 10),
    xycoords='data',
    xytext=(75, -65),
    textcoords='data',
    size=15,
    # bbox=dict(boxstyle="round", fc="0.8"),
    arrowprops=dict(arrowstyle="simple",
        fc="0.6", ec="none",
        connectionstyle="arc3,rad=-0.4"
    ),
)
"""

g.set_xlim(-100, 100)
g.set_ylim(-100, 100)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("ray.pdf", bbox_inches="tight")
