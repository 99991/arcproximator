# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi
import math
from common import *
import numpy as np

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

center0 = Point(50, 30)
center1 = Point(38.4, 60)
radius0 = 50
radius1 = 30
a = center0.polar(pi*0.7, radius0)
b = center0.polar(pi*0.4, radius0)
c = center1.polar(pi*1.3, radius1)
d = center1.polar(pi*1.86, radius1)
arc0 = Arc(center0, a, b, True)
arc1 = Arc(center1, c, d, False)

y0 = center1.y - radius1
y1 = center0.y + radius0

plt.plot([a.x, b.x, c.x, a.x], [y1, y1, y0, y1], ls='--', color='gray')
plt.plot([c.x, d.x, d.x], [y0, y0, y1], ls='--', color='gray')

x = np.linspace(a.x, b.x, 200)
y0 = center1.y - np.sqrt(radius1**2 - (x - center1.x)**2)
y1 = center0.y + np.sqrt(radius0**2 - (x - center0.x)**2)

g.fill_between(x, y0, y1, facecolor='0.7', interpolate=True)

draw_arc(arc0)
draw_arc(arc1)
"""

j0 = center.polar(pi*0.5, radius)
j1 = center.polar(pi*1.9, radius)

draw_from_join(j0, False, False, 'dashed')
#draw_from_join(j1, True, True, 'dotted')

a_tangent = a_tangent.scaled(tangent_length)
b_tangent = b_tangent.scaled(tangent_length)

g.text(j0.x - 10, j0.y + 10, r'$\mathbf{j}_0$', fontsize=20)
#g.text(j1.x + 10, j1.y - 0, r'$\mathbf{j}_1$', fontsize=20)
g.text(a.x + a_tangent.x + 3, a.y + a_tangent.y + 3, r'$\mathbf{u}_0$', fontsize=20)
g.text(b.x + b_tangent.x - 5, b.y + b_tangent.y + 5, r'$\mathbf{u}_1$', fontsize=20)
g.text(a.x + 10, a.y - 5, r'$\mathbf{a}_0$', fontsize=20)
g.text(b.x - 5, b.y - 20, r'$\mathbf{b}_1$', fontsize=20)
g.text(-35, -31, r'$\mathbf{r}_2$', fontsize=20)
g.text(32, -25, r'$\mathbf{r}_2$', fontsize=20)

g.text(center.x - 5, center.y - 20, r'$\mathbf{c}_2$', fontsize=20)

plt.plot([a.x, b.x, center.x], [a.y, b.y, center.y], 'o', lw=1, color='gray', ms=10)

plt.plot([a.x, center.x, b.x], [a.y, center.y, b.y], '--', color='gray')

plt.plot([center.x], [center.y], 'o', lw=1, color='gray', ms=10)
g.text(center.x - 5, center.y - 20, r'$\mathbf{c}_2$', fontsize=20)

g.arrow(a.x, a.y, a_tangent.x, a_tangent.y, head_width=5, head_length=5, fc='k', ec='k')
g.arrow(b.x, b.y, b_tangent.x, b_tangent.y, head_width=5, head_length=5, fc='k', ec='k')

control_points = [(p.x, p.y) for p in [
    a,
    a + a_tangent,
    b - b_tangent,
    b,
]]

commands = [
    Path.MOVETO,
    Path.CURVE4,
    Path.CURVE4,
    Path.CURVE4,
]

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
path = Path(control_points, commands)
patch = patches.PathPatch(path, facecolor='none')
g.add_patch(patch)
xs = [x for x, _ in control_points]
ys = [y for _, y in control_points]
#g.plot(xs, ys, 'o--', lw=1, color='gray', ms=10)
"""
g.set_xlim(12, 75)
g.set_ylim(22, 88)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("fill_between.pdf", bbox_inches="tight")
