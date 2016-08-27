# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
import numpy as np
from math import sin, cos, pi
import math

if 0:
    # 0.1 degree radians looks almost smooth
    
    fig = plt.figure()

    nx = 5
    ny = 5
    n = nx*ny
    for i in range(n):

        angle = pi*i/n
        x0 = -1
        y0 = 0
        x1 = 0
        y1 = 0
        x2 = x1 + cos(angle)
        y2 = y1 + sin(angle)
        
        ax = fig.add_subplot(ny, nx, i + 1)
        ax.set_title("%.1f deg, %.3f rad"%(angle/pi*180, angle))
        ax.set_xlim(-1.1, 1.1)
        ax.set_ylim(-1.1, 1.1)
        ax.plot([x0, x1, x2], [y0, y1, y2])

    plt.show()

def bezier_at(p, t):
    s = 1 - t
    x = s*s*s*p[0][0] + 3.0*s*s*t*p[1][0] + 3.0*s*t*t*p[2][0] + t*t*t*p[3][0]
    y = s*s*s*p[0][1] + 3.0*s*s*t*p[1][1] + 3.0*s*t*t*p[2][1] + t*t*t*p[3][1]
    return list(zip(x, y, t))

def angle_between(a, b, c):
    abx = b[0] - a[0]
    aby = b[1] - a[1]
    ab = math.sqrt(abx*abx + aby*aby)
    abx /= ab
    aby /= ab
    acx = c[0] - b[0]
    acy = c[1] - b[1]
    ac = math.sqrt(acx*acx + acy*acy)
    acx /= ac
    acy /= ac
    dot = abx*acx + aby*acy
    return math.acos(dot)

def discard_unnecessary_points(points, max_err):
    
    while len(points) > 3:
        err, i = min(
            (angle_between(a, b, c), i)
            for a, b, c, i in zip(points, points[1:], points[2:], range(1, len(points)+1)))

        old_points = points

        # remove point with smallest error
        points = points[:i] + points[i+1:]
        
        err = max(
            angle_between(a, b, c)
            for a, b, c, i in zip(points, points[1:], points[2:], range(1, len(points)+1)))
        
        if err > max_err:
            # error exceeded
            # restore points which had less error
            return old_points

def approximate_bezier(p, max_err, n=100):
    # first approximation
    points = bezier_at(p, np.linspace(0, 1, n))

    for _ in range(100):
        # add some random points
        random_points = bezier_at(p, np.random.random(10))
        points.extend(random_points)
        points.sort(key=lambda p:p[2])
        # discard the bad ones
        points = discard_unnecessary_points(points, max_err)
    
    return points


if 0:
    p = np.array([
        (0.3, 0.0),
        (1.0, 1.0),
        (0.0, 1.0),
        (0.7, 0.0),
    ])
    
    points = approximate_bezier(p, 0.1)
    
    errs = list(
        angle_between(a, b, c)
        for a, b, c, i in zip(points, points[1:], points[2:], range(1, len(points)+1)))

    print("mean:", np.mean(errs))
    print("max:", np.max(errs))
    print("stddev:", np.std(errs))
    print("points:", len(points))
    print("max diff:", np.max(errs - np.mean(errs)))
    
    x, y, t = zip(*points)
    plt.plot(x, y, 'o-')
    plt.show()

control_points = np.array([
    (0.0, 0.0),
    (0.0, 1.0),
    (1.0, 0.0),
    (1.0, 1.0),
])

commands = [
    Path.MOVETO,
    Path.CURVE4,
    Path.CURVE4,
    Path.CURVE4,
]

path = Path(control_points, commands)

fig = plt.figure()
ax = fig.add_subplot(1, 3, 1)
ax.set_title('Bézier curve'.decode('utf-8'))
patch = patches.PathPatch(path, facecolor='none')
ax.add_patch(patch)

def draw_common(control_points):
    """
    xs, ys = zip(*control_points)
    ax.plot(xs, ys, 'x--', lw=1, color='gray', ms=10)
    
    ax.text(+0.05, +0.00, 'P0')
    ax.text(+0.10, +1.00, 'P1')
    ax.text(+0.85, +0.00, 'P2')
    ax.text(+0.90, +1.00, 'P3')
    """

    ax.set_aspect(1)

    ax.xaxis.set_visible(False)
    ax.yaxis.set_visible(False)
    
    ax.set_xlim(-0.1, 1.1)
    ax.set_ylim(-0.1, 1.1)

draw_common(control_points)

ax = fig.add_subplot(1, 3, 2)

points = approximate_bezier(control_points, 20*pi/180)
x, y, t = zip(*points)
ax.set_title(("%d segments, 20°"%(len(points) - 1)).decode('utf-8'))
plt.plot(x, y, color='black')

draw_common(control_points)

ax = fig.add_subplot(1, 3, 3)

points = approximate_bezier(control_points, 5*pi/180)
x, y, t = zip(*points)
ax.set_title(("%d segments, 5°"%(len(points) - 1)).decode('utf-8'))
plt.plot(x, y, color='black')

draw_common(control_points)

#plt.show()
plt.savefig("bezier_segments.pdf", bbox_inches="tight")
