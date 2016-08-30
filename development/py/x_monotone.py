# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi
import math
from common import *

def set_common(g):    
    g.set_xlim(-150, 150)
    g.set_ylim(-150, 150)
    g.set_aspect(1)
    g.xaxis.set_visible(False)
    g.yaxis.set_visible(False)

def degrees(p, center):
    return  math.degrees((p - center).angle())

def draw_arc(g, arc):
    angle0 = degrees(arc.a, arc.center)
    angle1 = degrees(arc.b, arc.center)
    width = 2*arc.radius()
    if arc.clockwise:
        angle1, angle0 = angle0, angle1
    g.add_patch(patches.Arc(arc.center, width, width, 0.0, angle0, angle1))
    plt.plot([arc.a.x, arc.b.x], [arc.a.y, arc.b.y], 'o', lw=1, color='gray', ms=10)


center = Point(0, 0)
radius = 100
r = 150
p = [
    center.polar(pi*+1.4, radius),
    center.polar(pi*+1.0, radius),
    center.polar(pi*+0.5, radius),
    center.polar(pi*+0.0, radius),
    center.polar(pi*-0.4, radius),
]

fig = plt.figure()

g = fig.add_subplot(2, 2, 1)

arc = Arc(center, p[0], p[4], True)
draw_arc(g, arc)

plt.plot([-r, +r], [0, 0], ls='dashed', color='gray')

set_common(g)

g = fig.add_subplot(2, 2, 2)

arc = Arc(center, p[0], p[1], True)
draw_arc(g, arc)
arc = Arc(center, p[1] + Point(0, 30), p[3] + Point(0, 30), True)
draw_arc(g, arc)
arc = Arc(center, p[3], p[4], True)
draw_arc(g, arc)

set_common(g)

g = fig.add_subplot(2, 2, 3)

arc = Arc(center, p[0], p[4], True)
draw_arc(g, arc)

plt.plot([0, 0], [0, +r], ls='dashed', color='gray')
plt.plot([0, 0], [0, -r], ls='dashed', color='gray')
plt.plot([0, +r], [0, 0], ls='dashed', color='gray')
plt.plot([0, -r], [0, 0], ls='dashed', color='gray')

set_common(g)

g = fig.add_subplot(2, 2, 4)

for a, b in zip(p, p[1:]):
    offset = (a + b).scaled(20)
    a = a + offset
    b = b + offset
    arc = Arc(center, a, b, True)
    draw_arc(g, arc)

set_common(g)

#plt.show()
plt.savefig("xy_monotone.pdf", bbox_inches="tight")
