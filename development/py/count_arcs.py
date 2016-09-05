from approximate import *
from matplotlib import pyplot as plt

r = 1
curves = [
    CubicBezier([Point(0, r), Point(0, 0), Point(r, 0), Point(r, r)]),
    CubicBezier([Point(0, 0), Point(r, 0), Point(0, r), Point(r, r)]),
    CubicBezier([Point(0, 0), Point(r, r), Point(0, r), Point(r, 0)]),
]

def f(curve, err):
    arcs = []
    subdivide(curve, err, arcs)
    return len(arcs)

names = [
    "U-shape",
    "S-shape",
    "Spike",
]

styles = ['--', ':', '-']
markers = ['o', 'x', '+']

fig = plt.figure(num=None, figsize=(14, 5), dpi=100)

g = plt.subplot(1, 2, 1)

for name, marker, curve, style in zip(names, markers, curves, styles):
    n = 70
    errs = []
    ns = []
    last_n = None
    style = ''
    for err in (0.1**(0.1*i) for i in range(1, n)):
        n = f(curve, err)
        if n != last_n:
            g.loglog(errs, ns, marker=marker, ls=style, ms=5, color='black', mfc='none')
            ns = []
            errs = []
        ns.append(n)
        errs.append(err)
        last_n = n
    g.loglog(errs, ns, marker=marker, ls=style, ms=5, label=name, color='black', mfc='none')

g.legend(fancybox=True, shadow=True)

g.set_xlabel('distance')
g.set_ylabel('number of arcs')

g = plt.subplot(1, 2, 2)

for curve, style, name in zip(curves, styles, names):
    points = curve.sample()

    x = [p.x for p in points]
    y = [p.y for p in points]

    g.plot(x, y, ls=style, color='black', label=name)

g.legend(loc='upper center', bbox_to_anchor=(0.3, 1.0), fancybox=True, shadow=True)
g.set_xlim(-0.1*r, 1.1*r)
g.set_ylim(-0.1*r, 1.1*r)
g.set_aspect(1)
g.xaxis.set_visible(False)
g.yaxis.set_visible(False)
#-0.3114 x + 1.972
#-0.3069 x + 1.287
#-0.2906 x + 0.745
plt.savefig("count_arcs.pdf", bbox_inches="tight")
#plt.show()

if 0:
    import numpy as np

    r = 1
    curves = [
        CubicBezier([Point(0, r), Point(0, 0), Point(r, 0), Point(r, r)]),
        CubicBezier([Point(0, 0), Point(r, 0), Point(0, r), Point(r, r)]),
        CubicBezier([Point(0, 0), Point(r, r), Point(0, r), Point(r, 0)]),
    ]

    curve = curves[1]

    x = np.array([0.1**(0.1*i) for i in range(20, 50)])
    y = np.array([f(curve, value) for value in x])

    p = np.poly1d(np.polyfit(np.log(x), np.log(y), 1))
    print(p)

    plt.loglog(x, y)
    t = np.linspace(np.min(x), np.max(x))
    s = t**p[1]*np.exp(p[0])
    plt.loglog(t, s)
    plt.show()
