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

r = r*0.5 + 0.5

misc.imsave("sdf2.png", r)

