from common import *

class CubicBezier(object):
    def __init__(self, points):
        self.points = points

    def at(self, t):
        p0, p1, p2, p3 = self.points
        s = 1 - t
        return s*s*s*p0 + 3.0*s*s*t*p1 + 3.0*s*t*t*p2 + t*t*t*p3

    def sample(self, t0=0.0, t1=1.0, n=100):
        dt = (t1 - t0)/(n - 1.0)
        return [self.at(t0 + dt*i) for i in range(n)]

    def dist2(self, arc0, arc1, n=10):
        dt = 1.0/(n - 1)

        maximum = (-inf, None, None, None)

        arcs = [arc0, arc1]
        for i in range(n):
            t = dt*i
            p = self.at(t)
            d = min((p.dist2(q), t, p, q) for q in (arc.clamp(p) for arc in arcs))
            maximum = max(maximum, d)
            
        return maximum

    def split(self, t):
        a, b, c, d = self.points
        p0, p1, p2, p3 = self.points
        p01 = lerp(p0, p1, t)
        p12 = lerp(p1, p2, t)
        p23 = lerp(p2, p3, t)

        p012 = lerp(p01, p12, t)
        p123 = lerp(p12, p23, t)

        p0123 = lerp(p012, p123, t)

        curve0 = CubicBezier((p0, p01, p012, p0123))
        curve1 = CubicBezier((p0123, p123, p23, p3))
        
        return (curve0, curve1)


def subdivide(curve, max_err, arcs, depth=10):
    p0, p1, p2, p3 = curve.points

    u0 = (p1 - p0).normalized()
    u1 = (p3 - p2).normalized()

    #_, _, center = rotation_and_center(p0, p3, u0, u1)
    v = u0.det(u1)/(u0.dot(u1) - 1)*(p0 - p3)
    x = (p0.x + p3.x - v.y)/2
    y = (p0.y + p3.y + v.x)/2

    center = Point(x, y)
    
    radius = p0.dist(center)

    join = curve.at(0.5)

    join = center + radius*(join - center).normalized()

    arc0 = arc_from_points_and_normal(p0, join, u0.left(), join.is_left_of(p1, p0), False)
    arc1 = arc_from_points_and_normal(p3, join, u1.left(), join.is_right_of(p3, p2), True)

    d2, t, p, q = curve.dist2(arc0, arc1)

    if depth == 0:
        print("Warning: max depth reached bug error still big")
        arcs.append(arc0)
        arcs.append(arc1)
    else:
        if sqrt(d2) < max_err:
            arcs.append(arc0)
            arcs.append(arc1)
        else:
            curve0, curve1 = curve.split(t)
            subdivide(curve0, max_err, arcs, depth - 1)
            subdivide(curve1, max_err, arcs, depth - 1)
