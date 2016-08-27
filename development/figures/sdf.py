# -*- coding: utf-8 -*-
from matplotlib import pyplot as plt
import numpy as np
import scipy
from scipy import misc

radius = 1
n = 512
x0 = -1.5
y0 = -1.5
x1 = +1.5
y1 = +1.5
x = np.linspace(x0, x1, n)
y = np.linspace(y0, y1, n)

X, Y = np.meshgrid(x, y)
r = np.sqrt(X*X + Y*Y) - radius

def make_circle_points(cx, cy, r, n=100):
    angle = np.linspace(0, 2*np.pi, n)
    x = cx + np.cos(angle)*r
    y = cy + np.sin(angle)*r
    return [x, y]

plt.imshow(r, cmap='Greys_r', extent=[x0, x1, y0, y1])
plt.colorbar()

x, y = make_circle_points(0, 0, radius)

plt.plot(x, y, color='black')

def mark(x, y, s, color='black', dx=0.07, dy=-0.03):
    plt.text(x+dx, y+dy, s, color=color)
    plt.plot([x], [y], 'o', color='gray')

mark(0, 0, '-1', 'white')
mark(0, 0.25, '-0.75', 'white')
mark(1, 0, '0')
mark(0, 1, '0', 'black', -0.03, 0.05)
mark(-1, 0, '0')
mark(-1, -1, '√2-1'.decode('utf-8'))
mark(np.sqrt(0.5), np.sqrt(0.5), '0')
mark(0, -0.75, '-0.25')
angle = np.pi*-0.25
mark(np.cos(angle)*1.5, np.sin(angle)*1.5, '0.5')

#plt.show()
plt.savefig("sdf.png", bbox_inches="tight")

"""
fig = plt.figure()

ax = fig.add_subplot(1, 1, 1)
ax.set_xlim(-1.5, 1.5)
ax.set_ylim(-1.5, 1.5)
ax.set_aspect(1)

x, y = make_circle_points(0, 0, 1)

ax.plot(x, y, color='black')

def mark(x, y, s, dx=0.07, dy=-0.03):
    ax.text(x+dx, y+dy, s)
    ax.plot([x], [y], 'o', color='gray')

mark(1, 0, '0')
mark(0, 0, '-1')
mark(0, 1, '0', -0.03, 0.05)
mark(-1, 0, '0')
mark(-1, -1, '√2'.decode('utf-8'))
mark(np.sqrt(0.5), -np.sqrt(0.5), '1')
mark(0, -0.5, '-0.5'.decode('utf-8'))

plt.show()
"""
