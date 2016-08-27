# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi
import math
from common import *

fig = plt.figure()

g = fig.add_subplot(1, 1, 1)
#ax.text(center1[0] + 5, center1[1], r'$\mathbf{c}_1$', fontsize=20)
#ax.arrow(center0[0] + radius0*cos(pi/4), center0[1] + radius0*sin(pi/4), eps, -eps, head_width=5, head_length=5, fc='k', ec='k')
#ax.plot([center0[0], center0[0], center0[0] + radius0, center1[0], px], [center0[1] + radius0, center0[1], center0[1], center1[1], py], 'o--', lw=1, color='gray', ms=10)
#arc1 = patches.Arc(center1, radius1*2, radius1*2, 0.0, 180, angle)
#ax.add_patch(arc0)

def fill_loop(points, color):
    xs, ys = make_xy([points[-1]] + points)
    g.fill(xs, ys, color=color)
    
def draw_loop(points, color='black'):
    xs, ys = make_xy([points[-1]] + points)
    g.plot(xs, ys, color=color)
    
points = [
    Point(100, 100),
    Point(400, 250),
    Point(100, 400),
]

inner_points = [
    Point(130, 150),
    Point(350, 250),
    Point(130, 350),
]

fill_loop(points, 'lightgray')
fill_loop(inner_points, 'white')
draw_loop(points)
draw_loop(inner_points)

g.set_xlim(0, 500)
g.set_ylim(0, 500)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

plt.show()
#plt.savefig("stencil_polygon.pdf", bbox_inches="tight")
