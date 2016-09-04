# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
import numpy as np
from math import sin, cos, pi, sqrt
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

radius0 = 50
radius1 = 80
center0 = (10, 50)
center1 = (center0[0] + radius0 + radius1, center0[1])

angle = 300
arc0 = patches.Arc(center0, radius0*2, radius0*2, 0.0, 0, 90)
arc1 = patches.Arc(center1, radius1*2, radius1*2, 0.0, 180, angle)

angle = np.deg2rad(angle)

px = center1[0] + cos(angle)*radius1
py = center1[1] + sin(angle)*radius1
ax.text(center0[0] + radius0 + 3, center0[1] + 5, r'$\mathbf{J}$', fontsize=20)
ax.text(center0[0] + radius0 + 3, center0[1] - 10, r'$\mathbf{a}_1$', fontsize=20)
ax.text(center0[0] + radius0 - 11, center0[1] - 9, r'$\mathbf{b}_0$', fontsize=20)
ax.text(px - 4, py - 13, r'$\mathbf{b}_1$', fontsize=20)
ax.text(center0[0] - 15, center0[1] + radius0, r'$\mathbf{a}_0$', fontsize=20)
ax.text(center0[0] + 3, center0[1] + radius0/2, r'$r_0$', fontsize=20)
ax.text(center0[0] + radius0/2 - 7, center0[1] - 9, r'$r_0$', fontsize=20)
ax.text(center1[0] - radius1/2 - 3, center1[1] - 9, r'$r_1$', fontsize=20)
ax.text((center1[0] + px)*0.5 + 2, (center1[1] + py)*0.5 + 2, r'$r_1$', fontsize=20)
ax.text(center0[0] - 15, center0[1], r'$\mathbf{c}_0$', fontsize=20)
ax.text(center1[0] + 5, center1[1], r'$\mathbf{c}_1$', fontsize=20)

ax.text(center0[0] + 20, center0[1] + radius0 + 5, r'$\mathbf{u}_0$', fontsize=20)
ax.text(195, 0, r'$\mathbf{u}_1$', fontsize=20)

eps = 1e-5
ax.arrow(center0[0] + radius0*cos(pi/4), center0[1] + radius0*sin(pi/4), eps, -eps, head_width=5, head_length=5, fc='k', ec='k')
ax.arrow(center1[0], center1[1] - radius1, eps, 0, head_width=5, head_length=5, fc='k', ec='k')

# tangent
#ax.arrow(center0[0] + radius0, center0[1] + 50, 0, -100, head_width=5, head_length=5, fc='k', ec='k')
#ax.text(center0[0] + radius0 + 5, center0[1] + 50, r'tangent', fontsize=15)

arrow_length = 40
ax.arrow(center0[0], center0[1] + radius0, arrow_length, 0, head_width=5, head_length=5, fc='k', ec='k')
dx = px - center1[0]
dy = py - center1[1]
d = sqrt(dx*dx + dy*dy)
f = arrow_length/d
dx, dy = (-dy*f, dx*f)
ax.arrow(px, py, dx, dy, head_width=5, head_length=5, fc='k', ec='k')

ax.plot([center0[0], center0[0], center0[0] + radius0, center1[0], px], [center0[1] + radius0, center0[1], center0[1], center1[1], py], 'o--', lw=1, color='gray', ms=10)

ax.add_patch(arc0)
ax.add_patch(arc1)

ax.set_xlim(-20, 230)
ax.set_ylim(-50, 130)
ax.set_aspect(1)
ax.xaxis.set_visible(False)
ax.yaxis.set_visible(False)

#plt.show()
plt.savefig("biarc.pdf", bbox_inches="tight")
