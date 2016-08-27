# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
import numpy as np
from math import sin, cos, pi
import math

"""
def draw_common(ax, control_points):
    
    xs, ys = zip(*control_points)
    ax.plot(xs, ys, 'x--', lw=1, color='gray', ms=10)
    
    for i, p in enumerate(control_points):
        add = ""
        if i == 0:
            add = ', t = 0'
        if i == len(control_points) - 1:
            add = ', t = 1'
        
        ax.text(p[0] + 0.05, p[1], r'$\mathbf{p}_%d%s$'%(i, add))

    ax.set_aspect(1)

    ax.xaxis.set_visible(False)
    ax.yaxis.set_visible(False)
    
    ax.set_xlim(-0.1, 1.1)
    ax.set_ylim(-0.1, 1.1)
"""
fig = plt.figure()

ax = fig.add_subplot(1, 1, 1)

angle0 = 0.0
angle1 = 90.0 + 45
center = (100, 100)
radius = 50

arc = patches.Arc(center, radius*2, radius*2, 0.0, angle0, angle1)

angle0 = np.deg2rad(angle0)
angle1 = np.deg2rad(angle1)

cx, cy = center
a_x = cx + cos(angle0)*radius
ay = cy + sin(angle0)*radius
bx = cx + cos(angle1)*radius
by = cy + sin(angle1)*radius

ax.text(a_x + 3, ay - 1, r'$\mathbf{a}$', fontsize=20)
ax.text(bx - 8, by - 1, r'$\mathbf{b}$', fontsize=20)
ax.text(cx - 2, cy - 6, r'$\mathbf{c}$', fontsize=20)
ax.text((a_x + cx)*0.5 - 3, (ay + cy)*0.5 - 5, r'$r$', fontsize=20)
ax.text((bx + cx)*0.5 - 5, (by + cy)*0.5 - 3, r'$r$', fontsize=20)

eps = 1e-5
ax.arrow(cx, cy + radius, -eps, 0, head_width=5, head_length=5, fc='k', ec='k')

ax.plot([a_x, cx, bx], [ay, cy, by], 'o--', lw=1, color='gray', ms=10)

ax.add_patch(arc)

ax.set_xlim(40, 170)
ax.set_ylim(80, 160)
ax.set_aspect(1)
ax.xaxis.set_visible(False)
ax.yaxis.set_visible(False)

#plt.show()
plt.savefig("arc.pdf", bbox_inches="tight")
