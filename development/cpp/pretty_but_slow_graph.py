from matplotlib import pyplot as plt
from collections import defaultdict
import numpy as np

path = "measurements.txt"

with open(path, "rb") as f:
    lines = f.read().strip().split("\n")
    rows = [line.split() for line in lines]
    rows = [(int(n), float(t_gpu)) for n, t_gpu, t_cpu in rows]

n = len(rows)
rows0 = rows[:n/2]
rows1 = rows[n/2:]

fig = plt.figure(num=None, figsize=(14, 5), dpi=100)

def plot(rows, i):
    rows.sort()
    ns = [n for n, t in rows]
    ts = [t for n, t in rows]
    
    g = plt.subplot(1, 2, i)
    g.plot(ns, ts, 'o', ms=1, color='black', alpha=0.05)#, label=label)
    #g.legend(loc="upper left", fancybox=True, shadow=True)
    plt.xlabel('number of rectangles')
    plt.ylabel('time [milliseconds]')

plot(rows0, 2)#, "drawn with intermediate buffer")
plot(rows1, 1)#, "drawn directly")
plt.savefig("api_overhead.pdf", bbox_inches="tight")
#plt.show()
