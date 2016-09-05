from __future__ import print_function
from approximate import *

def intersections_circle_circle(a_circle, b_circle):
    a = a_circle.center
    b = b_circle.center
    
    a_radius = a_circle.radius()
    b_radius = b_circle.radius()
    
    ba = b - a
    d = ba.length()

    # circles far away from each other
    if d > a_radius + b_radius:
        return []

    # circle inside
    if d < abs(a_radius - b_radius):
        return []

    # circles overlap
    if d == 0 and a_radius == b_radius:
        return []

    da = (a_radius*a_radius - b_radius*b_radius + d*d)*0.5/d

    h2 = a_radius*a_radius - da*da

    if h2 < 0:
        h2 = 0

    h = sqrt(h2)
    p = a + da/d*ba
    
    v = (h/d*ba).left()
    
    return [p + v, p - v]

def intersections_circle_ray(circle, start, direction):
    center, radius = circle.center, circle.radius()
    v = center - start
    t = v.dot(direction)
    d2 = t*t - v.dot(v) + radius*radius
    if d2 < 0:
        return []
    d = sqrt(d2)
    return [t - d, t + d]

def intersections_arc_arc(arc0, arc1):
    intersections = intersections_circle_circle(arc0, arc1)
    return [p for p in intersections if arc0.sees(p) and arc1.sees(p)]

def intersections_arc_segment(arc, segment):
    a, b = segment
    ba = b - a
    d = ba.length()
    direction = 1/d*ba
    distances = intersections_circle_ray(arc, a, direction)
    return [p for p in
        (a + t*direction for t in distances if t >= 0 and t <= d) if arc.sees(p)]

def intersections_segment_segment(seg_ab, seg_cd):
    a, b = seg_ab
    c, d = seg_cd
    
    ba = b - a
    dc = d - c
    ca = c - a
    
    ba_det_dc = ba.det(dc)
    ca_det_dc = ca.det(dc)
    ca_det_ba = ca.det(ba)

    # parallel segments
    if ba_det_dc == 0:
        
        if ca_det_dc == 0:
            intersections = []
            
            ba2 = ba.dot(ba)
            dc2 = dc.dot(dc)
            
            if 0 <= ba.dot(c - a) <= ba2: intersections.append(c)
            if 0 <= ba.dot(d - a) <= ba2: intersections.append(d)
            if 0 <= dc.dot(a - c) <= dc2: intersections.append(a)
            if 0 <= dc.dot(b - c) <= dc2: intersections.append(b)
            
            return intersections
        else:
            # parallel, but not on same line
            return []

    t = ca_det_ba/ba_det_dc
    s = ca_det_dc/ba_det_dc

    # intersection not on line segments
    if t < 0 or t > 1 or s < 0 or s > 1:
        return []
    
    return [a + s*ba]

def arc_arc_almost_intersections(arc0, arc1):
    intersections = []

    eps = 1e-5
    
    a, b, c, d = arc0.a, arc0.b, arc1.a, arc1.b
    
    if a != c and a != d and a.dist(arc1.clamp(a)) < eps: intersections.append(a)
    if b != c and b != d and b.dist(arc1.clamp(b)) < eps: intersections.append(b)
    if c != a and c != b and c.dist(arc0.clamp(c)) < eps: intersections.append(c)
    if d != a and d != b and d.dist(arc0.clamp(d)) < eps: intersections.append(d)
    
    return intersections

def self_intersect_curves(curves):
    n = len(curves)
    split_points = [[] for _ in range(n)]
    for i in range(n):
        a_curve = curves[i]
        for j in range(i + 1, n):
            b_curve = curves[j]

            intersections = []
            if type(a_curve) == Arc and type(b_curve) == Arc:
                temp = intersections_arc_arc(a_curve, b_curve)
                a, b, c, d = a_curve.a, a_curve.b, b_curve.a, b_curve.b
                eps = 1e-5
                temp = [p for p in temp if p.dist(a) > eps and p.dist(b) > eps and p.dist(c) > eps and p.dist(d) > eps]
                intersections.extend(temp)
                intersections.extend(arc_arc_almost_intersections(a_curve, b_curve))
            if type(a_curve) == Arc and type(b_curve) == Segment:
                intersections.extend(intersections_arc_segment(a_curve, b_curve))
            if type(a_curve) == Segment and type(b_curve) == Arc:
                intersections.extend(intersections_arc_segment(b_curve, a_curve))
            if type(a_curve) == Segment and type(b_curve) == Segment:
                intersections.extend(intersections_segment_segment(a_curve, b_curve))

            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    new_curves = []
    for i, curve in enumerate(curves):
        curve.split_at_points(split_points[i], new_curves)

    intersections = flatten_once(split_points)

    return (new_curves, intersections)

def make_curves_x_monotone(curves):
    new_curves = []
    for curve in curves:
        if type(curve) == Arc:
            curve.split_x_monotone(new_curves)
        else:
            new_curves.append(curve)

    return [curve.left_oriented() for curve in new_curves]

def draw_curve(curve, color='black'):
    if type(curve) == Arc:
        draw_arc(curve, color)
    elif type(curve) == Segment:
        a, b = curve
        draw_line(a, b, color)
    else:
        raise Exception("Unknown curve type: %s"%str(curve))

def without_short_curves(curves):
    return [curve for curve in curves if curve.length_squared() > 0.01]
    
def snap(p):
    return Point(round(p.x), round(p.y))

def arc_with_improved_center(arc):
    a, b, c, radius = arc.a, arc.b, arc.center, arc.radius()
    p = 0.5*(a + b)
    p = (p - c).scaled(radius) + c
    return arc_from_points(a, p, b)

def improve_centers(curves):
    new_curves = []
    for curve in curves:
        if type(curve) == Arc:
            new_curves.append(arc_with_improved_center(curve))
        else:
            new_curves.append(curve)
    return new_curves

def arc_intersections_horizontal(arc, p):
    a, b, c = arc.a, arc.b, arc.center
    r2 = a.dist2(c)
    cp = c - p
    d2 = r2 - cp.y*cp.y

    assert(a.x <= b.x)
    
    if d2 < 0:
        # above or below circle
        return [[], [], []]

    outside = cp.dot(cp) > r2
    
    d = sqrt(d2)
    d0 = cp.x - d
    d1 = cp.x + d

    p0 = Point(p.x + d0, p.y)
    p1 = Point(p.x + d1, p.y)

    # TODO make this exact

    left = []
    right = []
    on = []

    def is_endpoint(p):
        if p == a:
            if arc.clockwise:
                if p.x < c.x:
                    return True
            else:
                if c.x <= p.x:
                    return True
                
        if p == b:
            if arc.clockwise:
                if c.x <= p.x:
                    return True
            else:
                if p.x < c.x:
                    return True

        return False

    def add_point(q):
        if q.x < p.x: left.append(q)
        elif q.x > p.x: right.append(q)
        else: on.append(q)
            
    if p0 == a or p0 == b:
        if is_endpoint(p0): add_point(p0)
    else:
        if arc.sees(p0): add_point(p0)

    if p1 == a or p1 == b:
        if is_endpoint(p1): add_point(p1)
    else:
        if arc.sees(p1): add_point(p1)
    
    return [left, on, right]

def get_endpoints(curves):
    return list(set(flatten_once(curve.endpoints() for curve in curves)))

def with_split_segments(curves, intersections):
    n = len(curves)
    split_segments = []
    split_points = [[] for _ in range(n)]

    endpoints = []

    intersections = set(intersections)
    
    n = len(curves)
    for i in range(n):
        for j in range(i + 1, n):
            
            curve_a = curves[i]
            curve_b = curves[j]
            
            a, b = curve_a.a, curve_a.b
            c, d = curve_b.a, curve_b.b

            p = None

            eps = 1
            # figure out orientation
            if a.dist(c) < eps:
                p, m, q = b, a, d
            elif b.dist(c) < eps:
                p, m, q = a, b, d
            elif a.dist(d) < eps:
                p, m, q = b, a, c
            elif b.dist(d) < eps:
                p, m, q = a, b, c

            if not p: continue

            p_left = p.x < m.x
            q_left = q.x < m.x
            if p_left == q_left and min(r.dist(m) for r in intersections) > eps:
                endpoints.append((m, p_left))

    if 0:
        #draw_curves(curves)
        for curve in curves:
            draw_curve(curve)
        for s, _ in endpoints:
            draw_circle(s, 5)

        raise Exception("Debug")
    
    for p, right_turn in endpoints:
        all_left = []
        all_right = []
        for i, curve in enumerate(curves):
            if type(curve) == Arc:
                left, on, right = arc_intersections_horizontal(curve, p)
            else:
                left, on, right = curve.intersections_horizontal(p)

            for s in left:
                all_left.append((s, i))
            for s in right:
                all_right.append((s, i))

        if right_turn:
            if len(all_right) % 2 == 1:
                s, i = min(all_right, key=lambda t: t[0][0])
                split_points[i].append(s)
                split_segments.append(Segment(p, s))
        else:
            if len(all_left) % 2 == 1:
                s, i = max(all_left, key=lambda t: t[0][0])
                split_points[i].append(s)
                split_segments.append(Segment(s, p))

    new_curves = []
    for i, curve in enumerate(curves):
        curve.split_at_points(split_points[i], new_curves)

    return new_curves + split_segments + split_segments

def closest_point_pair(points):
    points = list(set(points))
    
    a = points[0]
    b = points[1]
    closest = (a.dist(b), a, b)

    for a in points:
        for b in points:
            if a != b:
                closest = min(closest, (a.dist(b), a, b))
    
    return closest

def get_snap_mapping(points, eps):
    snap_points = [points[0]]
    d = dict()
    
    for p in points:
        closest = min(snap_points, key=lambda q: p.dist(q))
        if closest.dist(p) < eps:
            d[p] = closest
        else:
            snap_points.append(p)
            d[p] = p
    return d

def snap_curves(curves, eps):
    endpoints = get_endpoints(curves)

    d = get_snap_mapping(endpoints, eps)

    for curve in curves:
        a, b = curve.endpoints()
        curve.set_endpoints(d[a], d[b])

    return curves

def draw_curves(curves):
    for i, curve in enumerate(curves):
        a, b, c = curve.points(3)
        write(str(i), b)
        draw_curve(curve, color=colors[i%len(colors)])

def group_curves_by_endpoint(curves):
    endpoints = get_endpoints(curves)
    curves_by_point = defaultdict(list)

    # add curves to corresponding endpoint in dict
    for curve in curves:
        a, b = curve.endpoints()
        curves_by_point[a].append(curve)
        curves_by_point[b].append(curve)

    # sort curves around endpoint center
    for center, curves2 in curves_by_point.items():
        def curve_tangent_vector_angle(curve):
            v = curve.tangent_vector(center)
            return v.angle() + pi
        curves2.sort(key=curve_tangent_vector_angle)

    return curves_by_point

def unleash_face_eater(curves):

    curves_by_point = group_curves_by_endpoint(curves)

    faces = []
    n = 1000
    # while there are still curves, eat face
    for k in range(n):
        
        if len(curves) == 0:
            break
    
        debug = k == n - 1

        if debug and 1:
            draw_curves(curves)
            pass

        face = []
        faces.append(face)
        a = min(get_endpoints(curves), key=first)
        start = a
        
        curves2 = curves_by_point[a]
        curve = curves2[0]
        previous_curve = curve
        b = curve.other(a)
        curves3 = curves_by_point[b]
        
        i = find_exactly(curves3, curve)

        remove_exactly(curves, curve)
        remove_exactly(curves2, curve)
        remove_exactly(curves3, curve)
        face.append((curve, a))

        if debug:
            draw_curve(curve)
            draw_circle(a, 20)
            draw_circle(b, 20)
        
        if debug:
            print("from", a, "to", b)

        # for each curve of face
        for l in range(len(curves)):
                
            curves2 = curves3

            assert(len(curves2) > 0)
            
            for _ in range(len(curves2)):
                i -= 1
                if i >= len(curves2):
                    i = 0
                curve = curves2[i]
                c = curve.other(b)

                if type(curve) != type(previous_curve):
                    break
                else:
                    if curve != previous_curve:
                        break
                    else:
                        # duplicate horizontal divider
                        if l == 0:
                            break
            else:
                raise Exception("Did not find next edge")

            a = b
            b = c

            previous_curve = curve
            curves3 = curves_by_point[b]
            i = find_exactly(curves3, curve)

            remove_exactly(curves, curve)
            remove_exactly(curves2, curve)
            remove_exactly(curves3, curve)
            face.append((curve, a))
            
            if debug:
                print("from", a, "to", b, math.degrees((b - a).angle()))
                draw_curve(curve)
                draw_circle(b, 5)
                write(str(l), b.polar(random.random()*2*pi, 10))

            # found start, be done
            if b == start:
                #print(k, len(face))
                break

    return faces

def save(path, content):
    with open(path, "wb") as f:
        f.write(content)

def save_svg(path, beziers):
    with open(path, "wb") as f:
        s = """<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   width="200.0mm"
   height="200.0mm"
   viewBox="0 0 800.0 800.0"
   id="svg1337"
   version="1.1">
   
  <g>
    <path style="fill:#000000;fill-rule:evenodd;fill-opacity:1;"
       d=" """
        f.write(s)

        bezier = beziers[0]
        a = bezier.points[0]
        path = ['M %f,%f'%(a.x, a.y)]
        for bezier in beziers:
            a, b, c, d = bezier.points
            path.append('C %f,%f %f,%f %f,%f'%(b.x, b.y, c.x, c.y, d.x, d.y))
        path.append('z')
        path = ' '.join(path)
        f.write(path)

        f.write('"/></g></svg>')

        return path

def triangulate(mouse, points):

    curves = []
    beziers = []

    print("Bounds:")
    print(min(map(first, points)))
    print(min(map(second, points)))
    print(max(map(first, points)))
    print(max(map(second, points)))

    for i in range(0, len(points), 3):
        control_points = points[i:i+4]
        if len(control_points) < 4: break
        
        bezier = CubicBezier(control_points)
        beziers.append(bezier)
        
        subdivide(bezier, 0.1, curves)

    path = save_svg("beziers.svg", beziers)
    save("path.txt", path)

    control_points = [bezier.points for bezier in beziers]
    s = ["%f %f %f %f %f %f %f %f"%(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y) for a, b, c, d in control_points]
    s = "\n".join(s)
    save("beziers.txt", "%d\n"%len(control_points) + s)

    curves.append(Segment(points[0], points[-1]))

    curves = make_curves_x_monotone(curves)
    curves, intersections = self_intersect_curves(curves)
    curves = without_short_curves(curves)
    curves = with_split_segments(curves, intersections)
    curves = snap_curves(curves, 1)
    curves = without_short_curves(curves)
    curves = improve_centers(curves)
    faces = unleash_face_eater(curves)

    final_curves = []

    for k, curves in enumerate(faces):
        endpoints = get_endpoints(map(first, curves))

        new_curves = []
        n = len(curves)
        for i in range(n):
            curve, start = curves[i]
            intersections = []
            for p in endpoints:
                vertical_segment = Segment(Point(p.x, -1000.0), Point(p.x, 1000.0))
                if type(curve) == Arc:
                    intersections.extend(intersections_arc_segment(curve, vertical_segment))
                else:
                    intersections.extend(intersections_segment_segment(curve, vertical_segment))
            split_curves = []
            curve.split_at_points(intersections, split_curves)
            if curve.a == start:
                new_curves.extend(split_curves)
            else:
                new_curves.extend(reversed(split_curves))

        curves = new_curves

        points = get_endpoints(curves)

        n = len(curves)
        for i in range(n/2):
            j = n - i - 1
            curve_a = curves[i]
            curve_b = curves[j]

            if curve_a.a.y > curve_b.a.y:
                curve_b, curve_a = curve_a, curve_b

            final_curves.append(curve_a)
            final_curves.append(curve_b)

            draw_curve(curve_a)
            draw_curve(curve_b)
            draw_line(curve_a.a, curve_b.a)
            draw_line(curve_a.b, curve_b.b)

            
        

    if 1:
        f = open("arcs.txt", "wb")
        strings = [str(len(final_curves))]
        for i, curve in enumerate(final_curves):
            if type(curve) == Segment:
                a, b = curve
                strings.append("1 %f %f %f %f"%(a.x, a.y, b.x, b.y))
            else:
                a, b, c = curve.a, curve.b, curve.center
                strings.append("0 %f %f %f %f %f %f %d"%(c.x, c.y, a.x, a.y, b.x, b.y, 1 if curve.clockwise else 0))
        strings.append("}")
        strings.append("\n")
        s = "\n".join(strings)
        f.write(s)
        f.close()

        print("Arcs written to disc")
    

import Tkinter as tk
import random
import colorsys
import traceback

width = 800
height = 800

with open("points.txt", "rb") as f:
    s = f.read()
    xy = list(map(float, s.split()))
    xs = xy[0::2]
    ys = xy[1::2]
    points = [Point(Fraction(x), Fraction(y)) for x, y in zip(xs, ys)]

if 0:
    xy = [
        100, 100,
        200, 200,
        300, 100,
        400, 200,
        500, 100,
        600, 200,
        700, 100,
        700, 500,
        100, 500,
    ]
    xs = xy[0::2]
    ys = xy[1::2]
    points = [Point(x, y) for x, y in zip(xs, ys)]

random.seed(0)

if 1:
    n = 30
    n = n*3 + 1
    center = 0.5*Point(width, height)
    
    points = []
    angle = 0.25
    for i in range(n):
        radius = 50 + 300.0*i/n
        p = center.polar(angle, radius)
        points.append(p)
        angle += 0.7

master = tk.Tk()
canvas = tk.Canvas(master, width=width, height=height)
canvas.pack()

def transform(p):
    p = Point(p.x, height - p.y - 1)
    p = Point(float(p.x), float(p.y))
    return p

def draw_grid(dx=100, dy=100, color='gray'):
    for x in range(0, width, dx):
        canvas.create_line(x, 0, x, height, dash=(4, 4), fill=color)
    for y in range(0, height, dy):
        canvas.create_line(0, y, width, y, dash=(4, 4), fill=color)

def write(text, p, color='black', font_size=10):
    font=("Purisa", font_size)
    p = transform(p)
    canvas.create_text(p.x, p.y, text=text, fill=color, font=font)

def draw_circle(center, radius, color='black', n=100):
    a = center + Point(radius, 0)
    draw_arc(Arc(center, a, a), color)

def draw_line(a, b, color='black'):
    a = transform(a)
    b = transform(b)
    canvas.create_line(a.x, a.y, b.x, b.y, fill=color)

def draw_arc(arc, color='black', n = 100):
    points = arc.points(n)
    for a, b in zip(points, points[1:]):
        draw_line(a, b, color)

def draw_polygon(points, color='black'):
    xy = flatten_once((p.x, p.y) for p in map(transform, points))
    canvas.create_polygon(xy, fill=color)

def redraw(mouse=Point(width/2, height/2)):
    canvas.delete('all')
    canvas.create_rectangle(0, 0, width, height, fill='white')
    draw_grid()

    try:
        triangulate(mouse, points)
    except Exception as e:
        traceback.print_exc()

def on_drag_left(event):
    mouse = Point(event.x, height - 1 - event.y)

    closest = min(points, key=lambda p: mouse.dist(p))
    closest.is_now(mouse)
    
    redraw(mouse)

redraw()

def on_closing():

    if 0:
        with open("points.txt", "wb") as f:
            s = "\n".join(str(p.x) + " " + str(p.y) for p in points)
            f.write(s)
    
    master.destroy()

master.protocol("WM_DELETE_WINDOW", on_closing)

canvas.bind("<B1-Motion>", on_drag_left)

tk.mainloop()
