# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi, sqrt, asin, acos, atan
import math
from common import *

fig = plt.figure()

g = fig.add_subplot(1, 1, 1)
#ax.text(center1[0] + 5, center1[1], r'$\mathbf{c}_1$', fontsize=20)
#ax.arrow(center0[0] + radius0*cos(pi/4), center0[1] + radius0*sin(pi/4), eps, -eps, head_width=5, head_length=5, fc='k', ec='k')
#ax.plot([center0[0], center0[0], center0[0] + radius0, center1[0], px], [center0[1] + radius0, center0[1], center0[1], center1[1], py], 'o--', lw=1, color='gray', ms=10)
#arc1 = patches.Arc(center1, radius1*2, radius1*2, 0.0, 180, angle)
#ax.add_patch(arc0)

def degrees(p, center):
    return  math.degrees((p - center).angle())

def draw_arc(arc):
    angle0 = degrees(arc.a, arc.center)
    angle1 = degrees(arc.b, arc.center)
    width = 2*arc.radius()
    if arc.clockwise:
        angle1, angle0 = angle0, angle1
    g.add_patch(patches.Arc(arc.center, width, width, 0.0, angle0, angle1))
    #g.add_patch(patches.Arc(arc.center, 5, 5, 0.0, 0, 360))

center = Point(10, 10)
radius = 50
a = center.polar(pi*0.0, radius)
b = center.polar(pi*0.8, radius)
arc = Arc(center, a, b)

v = (a - center).left()
w = (b - center).right()

ba = b - a

middle = (a + b)*0.5
pc = middle - center
d = radius - ((a + b)*0.5 - center).length()

peak = middle + pc.scaled(d)
g.text(25, 51, r'$d$', fontsize=20)
g.text(62, 6, r'$\mathbf{a}$', fontsize=20)
g.text(-33, 31, r'$\mathbf{b}$', fontsize=20)
g.text(62, 47, r'$\mathbf{p}$', fontsize=20)
g.text(-12, 71, r'$\mathbf{q}$', fontsize=20)
g.text(6, 3, r'$\mathbf{c}$', fontsize=20)

plt.plot([middle.x, peak.x], [middle.y, peak.y], color='black')

p = a + d/sqrt(v.length2() - v.dot(ba)**2/ba.length2())*v
q = b + d/sqrt(w.length2() - w.dot(ba)**2/ba.length2())*w

#plt.plot([a.x, center.x, b.x], [a.y, center.y, b.y], color='black')
plt.plot([a.x, p.x, q.x], [a.y, p.y, q.y], 'o-', color='gray', ls='--')
plt.plot([a.x, q.x, b.x, a.x], [a.y, q.y, b.y, a.y], 'o-', color='gray', ls='--')
plt.plot([center.x], [center.y], 'o', color='gray', ls='--')

draw_arc(arc)

g.set_xlim(-40, 80)
g.set_ylim(-10, 80)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("arc_cover.pdf", bbox_inches="tight")
