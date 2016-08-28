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

def degrees(p, center):
    return  math.degrees((p - center).angle())

def draw_arc(arc, ls):
    angle0 = degrees(arc.a, arc.center)
    angle1 = degrees(arc.b, arc.center)
    width = 2*arc.radius()
    if arc.clockwise:
        angle1, angle0 = angle0, angle1
    g.add_patch(patches.Arc(arc.center, width, width, 0.0, angle0, angle1, ls=ls))
    #g.add_patch(patches.Arc(arc.center, 5, 5, 0.0, 0, 360))

center = Point(0, 0)
radius = 50

a_angle = pi*0.0
b_angle = pi*0.9
tangent_length = 70

a = center.polar(a_angle, radius)
b = center.polar(b_angle, radius)

a_tangent = Point(+1, +2).normalized()
b_tangent = Point(-2, +1).normalized()

_, _, center = rotation_and_center(a, b, a_tangent, -b_tangent)
radius = center.dist(a)
circle = patches.Arc(center, radius*2, radius*2, 0.0, 0, 360)
g.add_patch(circle)

def draw_from_join(j, cw0, cw1, ls):
    a_normal = a_tangent.left()
    b_normal = b_tangent.right()

    arc0 = arc_from_points_and_normal(a, j, a_normal, cw0)
    arc1 = arc_from_points_and_normal(b, j, b_normal, cw1)

    plt.plot([j.x], [j.y], 'o', lw=1, color='gray', ms=10)

    draw_arc(arc0, ls)
    draw_arc(arc1, ls)

j0 = center.polar(pi*0.5, radius)
j1 = center.polar(pi*1.9, radius)

#draw_from_join(j0, False, False, 'dashed')
draw_from_join(j1, True, True, 'dashed')

a_tangent = a_tangent.scaled(tangent_length)
b_tangent = b_tangent.scaled(tangent_length)

#g.text(j0.x - 10, j0.y + 10, r'$\mathbf{j}_0$', fontsize=20)
g.text(j1.x + 10, j1.y - 0, r'$\mathbf{j}_1$', fontsize=20)
g.text(a.x + a_tangent.x + 3, a.y + a_tangent.y + 3, r'$\mathbf{u}_0$', fontsize=20)
g.text(b.x + b_tangent.x - 5, b.y + b_tangent.y + 5, r'$\mathbf{u}_1$', fontsize=20)
g.text(a.x + 10, a.y - 5, r'$\mathbf{a}_0$', fontsize=20)
g.text(b.x - 5, b.y - 20, r'$\mathbf{b}_1$', fontsize=20)
g.text(center.x - 5, center.y - 20, r'$\mathbf{c}_2$', fontsize=20)

plt.plot([a.x, b.x, center.x], [a.y, b.y, center.y], 'o', lw=1, color='gray', ms=10)

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

path = Path(control_points, commands)
patch = patches.PathPatch(path, facecolor='none')
g.add_patch(patch)
xs = [x for x, _ in control_points]
ys = [y for _, y in control_points]
#g.plot(xs, ys, 'o--', lw=1, color='gray', ms=10)

g.set_xlim(-150, 200)
g.set_ylim(-140, 100)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("undesired_biarc.pdf", bbox_inches="tight")
