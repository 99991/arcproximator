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

g.plot([a.x],[a.y],marker=r'$\circlearrowleft$',ms=100, color='#dddddd', alpha=0.5)


arc0 = Arc(a + ba.right(), a, b + ba.right(), True)
arc1 = Arc(a - ba.right(), a, a - 2*ba.right(), False)

draw_line(a, b, '-')
c = a.polar(pi*-0.45, 70)
draw_line(a, c, '-')
draw_line(a, a.polar(pi*-0.3, 30))
draw_line(a, a.polar(pi*-0.1, 50))
draw_line(a, a.polar(pi*0.8, 70))
draw_line(a, a.polar(pi*1.2, 80))
draw_line(a, a.polar(pi*1.4, 30))

draw_arc(arc0)
draw_arc(arc1)

#plt.plot([j.x], [j.y], 'o', lw=1, color='gray', ms=10)

draw_dot(a)
#g.text(a.x + 70, a.y + 5, r'x-axis', fontsize=20)
g.arrow(a.x, a.y, 80, 0, head_width=5, head_length=5, fc='k', ec='k')
g.text(a.x + 40, a.y + 15, r'$0$', fontsize=20)
g.text(a.x + 36, a.y + 38, r'$1$', fontsize=20)
g.text(a.x - 20, a.y + 60, r'$2$', fontsize=20)
g.text(a.x - 64, a.y + 41, r'$3$', fontsize=20)
g.text(a.x - 33, a.y - 18, r'$4$', fontsize=20)
g.text(a.x - 17, a.y - 40, r'$5$', fontsize=20)
g.text(a.x + 14, a.y - 66, r'$6$', fontsize=20)
g.text(a.x + 20, a.y - 30, r'$7$', fontsize=20)
g.text(a.x + 50, a.y - 18, r'$8$', fontsize=20)

g.set_xlim(-100, 100)
g.set_ylim(-100, 100)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("order.pdf", bbox_inches="tight")
