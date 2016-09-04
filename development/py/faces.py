# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from math import sin, cos, pi
import math
from common import *

fig = plt.figure()

g = fig.add_subplot(1, 1, 1)

def degrees(p, center):
    return  math.degrees((p - center).angle())

def draw_arc(arc):
    angle0 = degrees(arc.a, arc.center)
    angle1 = degrees(arc.b, arc.center)
    width = 2*arc.radius()
    if arc.clockwise:
        angle1, angle0 = angle0, angle1
    g.add_patch(patches.Arc(arc.center, width, width, 0.0, angle0, angle1))

def draw_line(a, b, ls='-'):
    plt.plot([a.x, b.x], [a.y, b.y], ls=ls, color='black')

def draw_dot(p):
    plt.plot([p.x], [p.y], 'o', lw=1, color='gray', ms=10)

x0 = [150, 200, 200, 200,   0, 100,   0,  50, 150]
y0 = [100, 100, 300, 400, 400, 300, 100, 100, 100]
x1 = [0, 200, 150, 100,  50, 0]
y1 = [0,   0, 100, 200, 100, 0]
x2 = [100, 200]
y2 = [300, 300]


plt.plot(x0, y0, '-', color='black')
plt.plot(x1, y1, '-', color='black')
plt.plot(x2, y2, '--', color='black')

def arrow(x0, y0, x1, y1, fc="0.0", angle='-0.5'):
    g.annotate(
        '',
        xytext=(x0, y0),
        xy=(x1, y1),
        xycoords='data',
        textcoords='data',
        size=20,
        arrowprops=dict(arrowstyle="simple",
            fc=fc, ec="none",
            connectionstyle="arc3,rad=" + angle
        ),
    )

arrow(200, 360, 160, 400, "0.4")
arrow(60, 400, 40, 360, "0.4")
arrow(80, 320, 120, 300, "0.4")
arrow(170, 300, 200, 330, "0.4")

arrow(200, 250, 170, 300)
arrow(120, 300, 80, 260)
arrow(24, 145, 17, 103, angle="-0.3")
arrow(33, 100, 70, 140, angle="-0.3")
arrow(80, 180, 120, 180, angle='-1.5')
arrow(135, 130, 160, 100)
arrow(180, 100, 200, 150)

arrow(150, 0, 175, 50, "0.8")
arrow(160, 75, 120, 100, "0.8")
arrow(70, 100, 30, 65, "0.8")
arrow(20, 40, 40, 0, "0.8")

g.text(210, 340, "1", fontsize=20, color='0.4')
g.text(110, 410, "2", fontsize=20, color='0.4')
g.text( 40, 320, "3", fontsize=20, color='0.4')
g.text(133, 305, "4", fontsize=20, color='0.4')

g.text(208, 184, "1", fontsize=20)
g.text(133, 262, "2", fontsize=20)
g.text(24, 203, "3", fontsize=20)
g.text(3, 74, "4", fontsize=20)
g.text(56, 156, "5", fontsize=20)
g.text(127, 152, "6", fontsize=20)
g.text(175, 75, "7", fontsize=20)

g.text(185, 41, "1", fontsize=20, color='0.8')
g.text(84, 75, "2", fontsize=20, color='0.8')
g.text(-8, 26, "3", fontsize=20, color='0.8')
g.text(84, 5, "4", fontsize=20, color='0.8')

g.set_xlim(-50, 250)
g.set_ylim(-50, 450)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)

#plt.show()
plt.savefig("faces.pdf", bbox_inches="tight")
