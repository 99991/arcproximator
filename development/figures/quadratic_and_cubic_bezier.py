# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
import numpy as np
from math import sin, cos, pi
import math

#ax.set_title('Bézier curve'.decode('utf-8'))

def bezier_at(p, t):
    s = 1 - t
    x = s*s*s*p[0][0] + 3.0*s*s*t*p[1][0] + 3.0*s*t*t*p[2][0] + t*t*t*p[3][0]
    y = s*s*s*p[0][1] + 3.0*s*s*t*p[1][1] + 3.0*s*t*t*p[2][1] + t*t*t*p[3][1]
    return list(zip(x, y, t))

def draw_common(ax, control_points):
    
    xs, ys = zip(*control_points)
    ax.plot(xs, ys, 'o--', lw=1, color='gray', ms=10)
    
    for i, p in enumerate(control_points):
        add = ""
        if i == 0:
            add = ', t = 0'
        if i == len(control_points) - 1:
            add = ', t = 1'
        
        ax.text(p[0] + 0.05, p[1], r'$\mathbf{P}_%d%s$'%(i, add))

    ax.set_aspect(1)

    ax.xaxis.set_visible(False)
    ax.yaxis.set_visible(False)
    
    ax.set_xlim(-0.1, 1.1)
    ax.set_ylim(-0.1, 1.1)

fig = plt.figure()

control_points = np.array([
    (0.0, 0.0),
    (0.5, 1.0),
    (0.7, 0.1),
])

commands = [
    Path.MOVETO,
    Path.CURVE3,
    Path.CURVE3,
]

path = Path(control_points, commands)

ax = fig.add_subplot(1, 2, 1)
patch = patches.PathPatch(path, facecolor='none')
ax.add_patch(patch)

draw_common(ax, control_points)

ax.text(0.35, 0.35, r'$\mathbf{B}_2(t)$')

control_points = np.array([
    (0.3, 0.0),
    (0.0, 1.0),
    (0.9, 0.1),
    (0.7, 0.9),
])

commands = [
    Path.MOVETO,
    Path.CURVE4,
    Path.CURVE4,
    Path.CURVE4,
]

path = Path(control_points, commands)

ax = fig.add_subplot(1, 2, 2)
patch = patches.PathPatch(path, facecolor='none')
ax.add_patch(patch)

draw_common(ax, control_points)

ax.text(0.32, 0.35, r'$\mathbf{B}_3(t)$')

#plt.show()
plt.savefig("quadratic_and_cubic_bezier.pdf", bbox_inches="tight")
