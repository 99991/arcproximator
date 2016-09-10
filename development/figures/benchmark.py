import numpy as np
from matplotlib import pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
import scipy.misc

def read_array(path):
    with open(path, "rb") as f:
        lines = f.read().strip().split("\n")
        rows = []
        for line in lines:
            row = list(map(float, line.split()))
            rows.append(row)
        return rows

def make_graph(g, path, path_nvpr):
    rows = read_array(path_nvpr)
    rows = np.array(rows)

    x = rows[:, 0] * rows[:, 1]
    y = rows[:, 2]

    rows = read_array(path)
    n = len(rows)/4
    rows0 = rows[:n]
    rows1 = rows[n:n*2]
    rows2 = rows[n*2:n*3]
    rows3 = rows[n*3:]

    rows0 = np.array(rows0)
    x0 = rows0[:, 1] * rows0[:, 2]
    y0 = rows0[:, 3]
    
    rows1 = np.array(rows1)
    x1 = rows1[:, 1] * rows1[:, 2]
    y1 = rows1[:, 3]
    
    rows2 = np.array(rows2)
    x2 = rows2[:, 1] * rows2[:, 2]
    y2 = rows2[:, 3]
    
    rows3 = np.array(rows3)
    x3 = rows3[:, 1] * rows3[:, 2]
    y3 = rows3[:, 3]

    g.plot(x*1e-6, y, color='0.7', label="nvpr")
    g.plot(x0*1e-6, y0, ls="-.", color='black', label="stencil arcs")
    g.plot(x3*1e-6, y3, color='black', label="stencil segments")
    g.plot(x1*1e-6, y1, ls="dotted", color='black', label="trapezoids")
    g.plot(x2*1e-6, y2, '--', color='black', label="segments")

    g.legend(loc='upper center', bbox_to_anchor=(0.5, 1.05), ncol=2, fancybox=True, shadow=True)

    plt.xlabel('size [megapixels]')
    plt.ylabel('time [milliseconds]')
    
    g.set_xlim([0, 4096*4096*1e-6])
    g.set_ylim([0, 5])

fig = plt.figure(num=None, figsize=(14, 5), dpi=100)
"""
g = plt.subplot(2, 2, 3)
image = scipy.misc.imread("../py/beziers.png")
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)
g.imshow(image)

g = plt.subplot(2, 2, 4)
image = scipy.misc.imread("../py/beziers2.png")
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)
g.imshow(image)
"""
g = plt.subplot(1, 2, 1)
make_graph(g, 'results.txt', 'results_nvpr.txt')
g = plt.subplot(1, 2, 2)
make_graph(g, 'results2.txt', 'results_nvpr2.txt')

#plt.savefig("benchmark.pdf", bbox_inches="tight")
plt.show()
