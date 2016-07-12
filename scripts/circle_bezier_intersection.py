"""
from sympy import *
import re

def bezier(px, py, t):
    s = 1.0 - t
    x = s*s*s*px[0] + 3.0*s*s*t*px[1] + 3.0*s*t*t*px[2] + t*t*t*px[3]
    y = s*s*s*py[0] + 3.0*s*s*t*py[1] + 3.0*s*t*t*py[2] + t*t*t*py[3]
    return (x, y)

var('ax bx cx dx ay by cy dy x y r t')

bx, by = bezier([ax, bx, cx, dx], [ay, by, cy, dy], t)
distance_squared = (bx - x)**2 + (by - y)**2 - r**2
p = Poly(diff(distance_squared, t).expand(), t)
coeffs = p.all_coeffs()

for coeff in coeffs:
    s = str(coeff)
    def replace_pow(m):
        base = m.group(1)
        exponent = int(m.group(2))
        return "(" + "*".join([base]*exponent) + ")"
    print(re.sub("([a-z]+)\*\*([0-9]+)", replace_pow, s))

px = [300, 200, 300, 400]
py = [300, 200, 100, 200]
center_x = 300
center_y = 220
radius = 50

px = [100, 500, 50, 400]
py = [100, 150, 300, 500]

center_x = 431.161841
center_y = 445.466778
radius = 62.808699

center_x = 61.709054
center_y = 406.327567
radius = 308.711475

d = distance_squared.subs([
    (ax, px[0]),
    (bx, px[1]),
    (cx, px[2]),
    (dx, px[3]),
    (ay, py[0]),
    (by, py[1]),
    (cy, py[2]),
    (dy, py[3]),
    (x, center_x),
    (y, center_y),
    (r, radius),
]).expand()


# get coefficients of polynomial
coeffs = np.array(Poly(d, t).all_coeffs()).astype(np.float64)

# make polynomial from coefficients
p = np.poly1d(coeffs)

# calculate derivative
dp = p.deriv()

# calculate roots
roots = dp.r

# remove imaginary roots
roots = roots[np.abs(roots.imag) < 1e-5].real

# remove roots not in interval [0, 1]
roots = roots[roots >= 0]
roots = roots[roots <= 1]

print("roots:", roots)

fig = plt.figure()
ax1 = fig.add_subplot(2, 1, 1)
ax2 = fig.add_subplot(2, 1, 2)

if 1:
    t = np.linspace(-0.1, 1.1, 1000)
    d0 = p(t)
    bezier_x, bezier_y = bezier(px, py, t)
    d1 = dist(center_x, center_y, bezier_x, bezier_y)

    ax1.plot(roots, p(roots), 'ro')
    ax1.plot(t, d0)
    ax1.plot(t, d1)

if 1:
    rx, ry = bezier(px, py, roots)

    t = np.linspace(0, 1, 1000)
    x0, y0 = bezier(px, py, t)
    x1, y1 = circle(center_x, center_y, radius, t)

    ax2.set_aspect('equal', 'datalim')
    ax2.plot(rx, ry, 'ro')
    ax2.plot(x0, y0)
    ax2.plot(x1, y1)
    ax2.plot(px, py)

plt.show()
"""

import numpy as np
import numpy.linalg
from matplotlib import pyplot as plt
from sympy import *
import re

def dist(ax, ay, bx, by, r = 0):
    return (ax - bx)**2 + (ay - by)**2 - r**2

def bezier(px, py, t):
    s = 1 - t
    x = s*s*s*px[0] + 3*s*s*t*px[1] + 3*s*t*t*px[2] + t*t*t*px[3]
    y = s*s*s*py[0] + 3*s*s*t*py[1] + 3*s*t*t*py[2] + t*t*t*py[3]
    return (x, y)

def circle(cx, cy, r, t):
    angle = 2*np.pi*t
    x = cx + np.cos(angle)*r
    y = cy + np.sin(angle)*r
    return (x, y)

var('ax bx cx dx ay by cy dy x y t, r')

bezier_x, bezier_y = bezier([ax, bx, cx, dx], [ay, by, cy, dy], t)
d = dist(bezier_x, bezier_y, x, y, r)

center_x = 60
center_y = 400
radius = 300
px = [center_x, -100, 500, center_x + radius]
py = [center_y - radius, 300, 100, center_y]

if 1:
    coeffs = Poly(d, t).all_coeffs()
    for coeff in coeffs:
        s = str(coeff)
        def replace(m):
            base = m.group(1)
            exponent = int(m.group(2))
            return "(" + "*".join([base]*exponent) + ")"
        print(re.sub("([a-z]+)\*\*([0-9]+)", replace, s)+",")

# substitute values for variables
d = d.subs([
    (ax, px[0]),
    (bx, px[1]),
    (cx, px[2]),
    (dx, px[3]),
    (ay, py[0]),
    (by, py[1]),
    (cy, py[2]),
    (dy, py[3]),
    (x, center_x),
    (y, center_y),
    (r, radius),
]).expand()

# get coefficients of polynomial
coeffs = np.array(Poly(d, t).all_coeffs()).astype(np.float64)

# make polynomial from coefficients
p = np.poly1d(coeffs)

# calculate roots
roots = p.r

print("roots:", roots)

# remove imaginary roots
roots = roots[np.abs(roots.imag) < 1e-5].real

"""
# remove roots not in interval [0, 1]
roots = roots[roots >= 0]
roots = roots[roots <= 1]
"""

fig = plt.figure()
ax1 = fig.add_subplot(2, 1, 1)
ax2 = fig.add_subplot(2, 1, 2)

if 1:
    t = np.linspace(-0.1, 1.1, 1000)
    d0 = p(t)
    bezier_x, bezier_y = bezier(px, py, t)
    d1 = dist(center_x, center_y, bezier_x, bezier_y, radius)
    d1 = np.abs(d1)

    ax1.plot(roots, p(roots), 'ro')
    ax1.plot(t, d0)
    ax1.plot(t, d1)

if 1:
    rx, ry = bezier(px, py, roots)

    t = np.linspace(0, 1, 1000)
    x0, y0 = bezier(px, py, t)
    x1, y1 = circle(center_x, center_y, radius, t)

    ax2.set_aspect('equal', 'datalim')
    ax2.plot(rx, ry, 'ro')
    ax2.plot(x0, y0)
    ax2.plot(x1, y1)
    ax2.plot(px, py)

plt.show()
