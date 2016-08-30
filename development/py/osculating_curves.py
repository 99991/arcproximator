# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi
import math
from common import *

fig = plt.figure()

g = fig.add_subplot(1, 3, 1)
#ax.text(center1[0] + 5, center1[1], r'$\mathbf{c}_1$', fontsize=20)
#ax.arrow(center0[0] + radius0*cos(pi/4), center0[1] + radius0*sin(pi/4), eps, -eps, head_width=5, head_length=5, fc='k', ec='k')
#ax.plot([center0[0], center0[0], center0[0] + radius0, center1[0], px], [center0[1] + radius0, center0[1], center0[1], center1[1], py], 'o--', lw=1, color='gray', ms=10)
#arc1 = patches.Arc(center1, radius1*2, radius1*2, 0.0, 180, angle)
#ax.add_patch(arc0)

def tpl(p):
    return (p.x, p.y)

def draw_contour(g, p):
    xs = [x for x, _ in p]
    ys = [y for _, y in p]
    g.plot(xs, ys, 'o--', lw=0.5, color='gray', ms=4)

def draw_cubic(g, p):
    p_commands = [
        Path.MOVETO,
        Path.CURVE3,
        Path.CURVE3,
        Path.CLOSEPOLY,
    ]
    p_path = Path(p, p_commands)
    p_patch = patches.PathPatch(p_path, edgecolor='none', facecolor='lightgray')
    
    g.add_patch(p_patch)
    draw_contour(g, [p[0], p[1], p[2], p[0]])

def draw_inverse_cubic(g, q):
    q_commands = [
        Path.MOVETO,
        Path.LINETO,
        Path.LINETO,
        Path.CURVE3,
        Path.CURVE3,
        Path.CLOSEPOLY,
    ]
    q_path = Path(q, q_commands)
    q_patch = patches.PathPatch(q_path, edgecolor='none', facecolor='darkgray')

    g.add_patch(q_patch)
    draw_contour(g, [q[0], q[1], q[2], q[0]])

def set_common(g):    
    g.set_xlim(-20, 220)
    g.set_ylim(-50, 120)
    g.set_aspect(1)
    g.xaxis.set_visible(False)
    g.yaxis.set_visible(False)

p = [
    (0, 0),
    (100, 100),
    (200, 0),
    (0, 0),
]

offset = -30
q = [
    (0, offset),
    (100, 100 + offset),
    (200, offset),
    (100, 100 + offset),
    (0, offset),
    (0, offset),
]

draw_cubic(g, p)
draw_inverse_cubic(g, q)

set_common(g)

g = fig.add_subplot(1, 3, 2)

p = [
    (0, 0),
    (50, 50),
    (100, 50),
    (0, 0),
]
draw_cubic(g, p)

p = [
    (100, 50),
    (150, 50),
    (200, 0),
    (0, 0),
]
draw_cubic(g, p)

draw_inverse_cubic(g, q)

set_common(g)

g = fig.add_subplot(1, 3, 3)

p = [
    (0, 0),
    (50, 50),
    (100, 50),
    (0, 0),
]
draw_cubic(g, p)

p = [
    (100, 50),
    (150, 50),
    (200, 0),
    (0, 0),
]
draw_cubic(g, p)


q = [
    (0, offset),
    (50, 50 + offset),
    (100, 50 + offset),
    (50, 50 + offset),
    (0, offset),
    (0, offset),
]
draw_inverse_cubic(g, q)

q = [
    (100, 50 + offset),
    (150, 50 + offset),
    (200, offset),
    (150, 50 + offset),
    (100, 50 + offset),
    (100, 50 + offset),
]
draw_inverse_cubic(g, q)

set_common(g)

#plt.show()
plt.savefig("osculating_curves.pdf", bbox_inches="tight")
