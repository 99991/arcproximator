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

def make_circular_points(center, radius, n, delta_angle=None, offset=pi/2):
    if delta_angle == None:
        delta_angle = pi*2/n
    points = []
    for i in range(n):
        angle = i*delta_angle + offset
        points.append(center.polar(angle, radius))
    return points


center = Point(0, 0)
radius = 140
points = make_circular_points(center, radius, 5, pi*2/5*3)

s = intersect((points[0], points[1]), (points[2], points[3]), False)
s = s[0]

inner_radius = s.dist(center)

points.append(points[0])
xs, ys = make_xy(points)
g.fill(xs, ys, color='lightgray')

inner_points = make_circular_points(center, inner_radius, 5, 2*pi/5, pi/10*3)
inner_points.append(inner_points[0])
xs, ys = make_xy(inner_points)
g.fill(xs, ys, color='white')
g.plot(xs, ys, color='black')

def shoot_ray(r, direction, points):
    a = points[-1]
    intersections = []
    for b in points:
        intersections.extend(intersect((a, b), (r, r + direction), False))
        a = b
    intersections.sort(key=lambda p: (p - r).dot(direction))
    if len(intersections) % 2 == 1:
        color = 'gray'
    else:
        color = 'white'
    g.plot([r.x], [r.y], 'o', lw=1, color=color, ms=10)
    g.arrow(r.x, r.y, direction.x, direction.y, head_width=5, head_length=5, fc='k', ec='k')
    for i, s in enumerate(intersections):
        g.plot([s.x], [s.y], 'o', lw=5, color='white', mew=1, ms=5)
        g.text(s.x - 14, s.y + 2, r'$%d$'%i, fontsize=20)

xs, ys = make_xy(points)
g.plot(xs, ys, color='black')

# naturally grown variable names
other = Point(-70, 30)
third = Point(150, -70)
shoot_ray(center, Point(100, 0), points)
shoot_ray(other, Point(150, 75), points)
shoot_ray(third, Point(-250, 0), points)

g.text(center.x - 22, center.y - 20, "outside", fontsize=15)
g.text(other.x - 12, other.y - 20, "inside", fontsize=15)
g.text(third.x - 22, third.y - 20, "outside", fontsize=15)

g.set_xlim(-150, 200)
g.set_ylim(-150, 150)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("even_odd_rule.pdf", bbox_inches="tight")
