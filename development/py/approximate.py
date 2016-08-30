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
        ab = lerp(a, b, t)
        bc = lerp(b, c, t)
        cd = lerp(c, d, t)

        abc = lerp(ab, bc, t)
        bcd = lerp(bc, cd, t)
        abcd = lerp(abc, bcd, t)

        curve0 = CubicBezier((a, ab, abc, abcd))
        curve1 = CubicBezier((abcd, bcd, cd, d))
        
        return (curve0, curve1)


def subdivide(curve, max_err_squared, arcs, depth=10):
    p0, p1, p2, p3 = curve.points
    p10 = p1 - p0
    p21 = p2 - p1
    p23 = p2 - p3

    tangent0 = p10.normalized()
    tangent1 = p23.normalized()

    co, si, center = rotation_and_center(p0, p3, tangent0, tangent1)
    radius = p0.dist(center)

    join = curve.at(0.5)

    join = center + (join - center).scaled(radius)

    clockwise0 = join.is_right_of(p0, p1)
    clockwise1 = join.is_right_of(p3, p2)

    normal0 = tangent0.right() if clockwise0 else tangent0.left()
    normal1 = tangent1.right() if clockwise1 else tangent1.left()

    arc0 = arc_from_points_and_normal(p0, join, normal0, clockwise0)
    arc1 = arc_from_points_and_normal(p3, join, normal1, clockwise1)

    arc1 = reversed(arc1)
    
    d2, t, p, q = curve.dist2(arc0, arc1)

    if depth == 0:
        print("Warning: max depth reached bug error still big")
        arcs.append(arc0)
        arcs.append(arc1)
    else:
        if d2 < max_err_squared:
            arcs.append(arc0)
            arcs.append(arc1)
        else:
            curve0, curve1 = curve.split(t)
            subdivide(curve0, max_err_squared, arcs, depth - 1)
            subdivide(curve1, max_err_squared, arcs, depth - 1)
