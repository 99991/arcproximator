import numpy as np
import numpy.linalg
from matplotlib import pyplot as plt

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

ax = 239.595230
ay = 10.400486
bx = 239.595230
by = 10.719569
cx = 239.435687
cy = 10.879111
dx = 239.116600
dy = 10.879111
center_x = 239.116602
center_y = 10.400484
radius = 0.478628



px = [ax, bx, cx, dx]
py = [ay, by, cy, dy]

t = np.linspace(0, 1, 100)

bezier_x, bezier_y = bezier(px, py, t)
circle_x, circle_y = circle(center_x, center_y, radius, t)

plt.axes().set_aspect('equal', 'datalim')
plt.plot(bezier_x, bezier_y)
plt.plot(circle_x, circle_y)
plt.plot(px, py)
plt.show()
