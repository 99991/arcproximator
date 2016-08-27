# -*- coding: utf-8 -*-
from matplotlib import pyplot as plt
import numpy as np
import scipy
import scipy.misc

image = scipy.misc.imread("triangle_white.png")

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.imshow(image, extent=[0, 1, 0, 1])



plt.show()
"""
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

plt.savefig("sdf.png")
"""
"""

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
