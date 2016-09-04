from __future__ import print_function
from common import *
"""
def draw_segments(segments):
    random.seed(1)
    for i, segment in enumerate(segments):
        color = colors[i % len(colors)]
        write(str(i), segment.lerp(0.3), color)
        draw_line(segment[0], segment[1], color)
    
def segments_from_points(points):
    segments = []
    a = points[-1]
    for b in points:
        segments.append(Segment(a, b))
        a = b
    return segments

def handle_self_intersection(segments):
    split_points = [[] for _ in range(len(segments))]

    all_intersections = [Point(0, 0)]
    for i in range(len(segments)):
        seg_ab = segments[i]
        for j in range(i+1, len(segments)):
            seg_cd = segments[j]
            
            intersections = intersect(seg_ab, seg_cd)
            
            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    return split_things(segments, split_points, split_segment)

def sweep(segments):
    new_segments = []
    split_points = [[] for _ in range(len(segments))]
    
    vertices = list(sorted(set(a for a, b in segments), key=second))
    vertices_with_same_y = itertools.groupby(vertices, second)

    if 0:
        draw_segments(segments)
    
    for y, group in vertices_with_same_y:
        intersections = []

        for i, seg_ab in enumerate(segments):
            a, b = seg_ab
            
            # ignore horizontal segments
            if a.y == b.y:
                continue
            
            for intersection in sweep_intersect(y, seg_ab):
                # segments are half-open
                # so we ignore the upper segment point
                if intersection.y == max(a.y, b.y):
                    continue

                intersections.append((intersection, i))

        intersections.sort(key=first)
        group = sorted(group, key=first)

        if 0:
            next_color = itertools.cycle(colors)
            for s, i in intersections:
                print(s)
                write(str(i), s)
            print("")

        for p, q in zip(intersections[0::2], intersections[1::2]):
            a, i = p
            b, j = q
            between = [p for p in group if a.x <= p.x <= b.x]
            if between and a != b:
                split_points[i].append(a)
                split_points[j].append(b)
                points = [a] + between + [b]
                for a, b in zip(points, points[1:]):
                    ab = Segment(a, b)
                    ba = Segment(b, a)
                    if ab not in segments and ba not in segments:
                        new_segments.append(ab)
                        new_segments.append(ba)

                        if 0:
                            color = next(next_color)
                            draw_line(a, b, color=color)
                            draw_circle(a, 5, color=color)
                            draw_circle(b, 10, color=color)
                    a = b

    segments = split_things(segments, split_points, split_segment)
    
    segments.extend(new_segments)
    
    return segments

def remove_zero_length_segments(segments):
    return [segment for segment in segments if segment[0].dist2(segment[1]) > 0]

def eat_face(segments, vertex_edges, debug):
    points = (p for segment in segments for p in segment)
    p = min(points, key=second)
    start = p
    edges = vertex_edges[p]
    
    edge = edges[0]

    result = []

    max_face_size = 50
    for i in range(max_face_size):
        remove_exactly(edges, edge)
        remove_exactly(segments, edge)
        result.append(p)

        if debug and False:
            draw_line(edge[0], edge[1], color='gray')
            draw_circle(p, 5)
            write(str(i), edge.middle())

        p0 = p
        p = edge.other(p)
        edges = vertex_edges[p]
        index = find_exactly(edges, edge)
        assert(edges[index] is edge)
        del edges[index]
        
        if p == start:
            #print("Reached goal!")
            break

        # find a previous edge that does not go back
        for j in range(len(edges)):
            prev_edge = edges[index - j - 1]
            if prev_edge.other(p) != p0:
                break
        else:
            raise Exception("Could not find previous edge of", edge, "in", edges)
        # remove incomming edge
        edge = prev_edge
    else:
        raise Exception("max_face_size reached")

    return result

def group_segments_by_vertex(segments):
    vertex_edges = defaultdict(list)
    for segment in segments:
        a, b = segment
        vertex_edges[a].append(segment)
        vertex_edges[b].append(segment)

    # sort circular around vertex p
    for p, edges in vertex_edges.items():
        def key(edge):
            return (edge.other(p) - p).angle()
        edges.sort(key=key)

        # degree of each vertex must be even
        assert(len(edges) & 1 == 0)

    return vertex_edges

def unleash_face_eater(segments, vertex_edges):
    #for i in range(4):
    while segments:
        try:
            points = eat_face(segments, vertex_edges, 0)
        except Exception as e:
            traceback.print_exc()

        draw_polygon(points)
        
        if 0:
            a = points[-1]
            for i, b in enumerate(points):
                write(str(i), lerp(a, b, 0.3) + (b-a).scaled(20).left())
                draw_line(a, b)
                a = b
"""
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

    h = sqrt(a_radius*a_radius - da*da)
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
                intersections.extend(intersections_arc_arc(a_curve, b_curve))
                intersections.extend(arc_arc_almost_intersections(a_curve, b_curve))
            if type(a_curve) == Arc and type(b_curve) == Segment:
                intersections.extend(intersections_arc_segment(a_curve, b_curve))
            if type(a_curve) == Segment and type(b_curve) == Arc:
                intersections.extend(intersections_arc_segment(b_curve, a_curve))
            if type(a_curve) == Segment and type(b_curve) == Segment:
                intersections.extend(intersections_segment_segment(a_curve, b_curve))

            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    # TODO remove
    if 0:
        for intersections in split_points:
            for s in intersections:
                draw_circle(s, 5)

    new_curves = []
    for i, curve in enumerate(curves):
        curve.split_at_points(split_points[i], new_curves)
    return new_curves

def make_curves_x_monotone(curves):
    new_curves = []
    for curve in curves:
        if type(curve) == Arc:
            curve.split_x_monotone(new_curves)
        else:
            new_curves.append(curve)

    return [curve.left_oriented() for curve in new_curves]

def draw_curve(curve, color='white'):
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
"""
def improve_centers(curves):
    new_curves = []
    for curve in curves:
        if type(curve) == Arc:
            a, b, c, radius = curve.a, curve.b, curve.center, curve.radius()
            p = 0.5*(a + b)
            if a.y == b.y:
                if curve.clockwise:
                    p.y += radius
                else:
                    p.y -= radius
            else:
                p = (p - c).scaled(curve.radius()) + c
            p = snap(p)
            arc = arc_from_points(a, p, b)
            new_curves.append(arc)
            #new_curves.append(curve)
        else:
            new_curves.append(curve)
    return new_curves
"""
def arc_intersections_horizontal(arc, p):
    a, b, c = arc.a, arc.b, arc.center
    r2 = a.dist2(c)
    cp = c - p
    d2 = r2 - cp.y*cp.y
    
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

    if arc.sees(p0):
        if p0.x < p.x: left.append(p0)
        elif p0.x > p.x: right.append(p0)
        else: on.append(p0)

    if arc.sees(p1):
        if p1.x < p.x: left.append(p1)
        elif p1.x > p.x: right.append(p1)
        else: on.append(p1)
    
    return [left, on, right]

def get_endpoints(curves):
    return list(set(flatten_once(curve.endpoints() for curve in curves)))

def with_split_segments(curves):
    endpoints = get_endpoints(curves)

    n = len(curves)
    split_segments = []
    split_points = [[] for _ in range(n)]
    for p in endpoints:
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

        if len(all_left) % 2 == 1:
            s, i = max(all_left, key=lambda t: t[0][0])
            split_points[i].append(s)
            split_segments.append(Segment(s, p))
        
        if len(all_right) % 2 == 1:
            s, i = min(all_right, key=lambda t: t[0][0])
            split_points[i].append(s)
            split_segments.append(Segment(p, s))

    new_curves = []
    for i, curve in enumerate(curves):
        curve.split_at_points(split_points[i], new_curves)

    #reversed_segments = [reversed(segment) for segment in split_segments]
    
    #return new_curves + split_segments + reversed_segments

    return new_curves + split_segments + split_segments

def snap_indices(points, snap_dist2=1):
    n = len(points)
    groups = [set([i]) for i in range(n)]
    indices = [i for i in range(n)]
    
    for i in range(n):
        p = points[i]
        for j in range(i + 1, n):
            q = points[j]
            if p.dist2(q) <= snap_dist2:
                both = groups[i] | groups[j]
                for k in both:
                    indices[k] = i

    return indices

def snap_curves(curves):
    endpoints = get_endpoints(curves)
    indices = snap_indices(endpoints)
    snap_points = [endpoints[i] for i in indices]
    d = dict(zip(endpoints, snap_points))

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
    n = 25
    for k in range(n):
        if len(curves) == 0:
            print("Face eater finished")
            break
    
        debug = k == n - 1

        if debug:
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
            draw_circle(b, 10)
        
        if debug:
            print("from", a, "to", b)

        for l in range(len(curves)):
            curves2 = curves3
            
            for _ in range(len(curves2)):
                i -= 1
                if i >= len(curves2):
                    i = 0
                curve = curves2[i]
                c = curve.other(b)
                if curve != previous_curve:
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
                print("from", a, "to", b)
                draw_curve(curve)
                draw_circle(b, 10)

            # found start, be done
            if b == start:
                break
    
    return faces

def triangulate(mouse, points):

    curves = []
    a = points[-1]
    for i, b in enumerate(points):
        if i % 3 == 0:
            ba = b - a
            if i % 2 == 1:
                p = a + 0.5*ba + ba.left()
            else:
                p = a + 0.5*ba + ba.right()
            curves.append(arc_from_points(a, p, b))
        else:
            curves.append(Segment(a, b))
        a = b

    curves = make_curves_x_monotone(curves)
    curves = self_intersect_curves(curves)
    curves = without_short_curves(curves)
    #curves = improve_centers(curves)
    curves = with_split_segments(curves)
    curves = snap_curves(curves)
    curves = without_short_curves(curves)
    faces = unleash_face_eater(curves)

    final_curves = []

    for curves in faces:
        endpoints = get_endpoints(map(first, curves))

        new_curves = []
        n = len(curves)
        for i in range(n):
            curve, start = curves[i]
            intersections = []
            for p in endpoints:
                vertical_segment = Segment(Point(p.x, -1e5), Point(p.x, +1e5))
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

        draw_curves(curves)

        n = len(curves)
        for i in range(n/2):
            j = n - i - 1
            curve_a = curves[i]
            curve_b = curves[j]
            final_curves.append(curve_a)
            final_curves.append(curve_b)
            
    f = open("arcs.txt", "wb")
    strings = ["struct ar_arc arcs[] = {"]
    for curve in final_curves:
        strings.append("    " + str(curve) + ", ")
    strings.append("}")
    strings.append("\n")
    s = "\n".join(strings)
    f.write(s)
    f.close()
    

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
#points = [Point(x, y) for x, y in zip(xs, ys)]
"""
random.seed(1)
points = []
while len(points) < 5:
    x = random.randint(0, width)
    y = random.randint(0, height)
    p = Point(x, y)

    if not points or min(p.dist(q) for q in points) > 10:
        points.append(p)
"""
master = tk.Tk()
canvas = tk.Canvas(master, width=width, height=height)
canvas.pack()

def draw_grid(dx=100, dy=100, color='gray'):
    for x in range(0, width, dx):
        canvas.create_line(x, 0, x, height, dash=(4, 4), fill=color)
    for y in range(0, height, dy):
        canvas.create_line(0, y, width, y, dash=(4, 4), fill=color)

def write(text, p, color='white', font_size=10):
    font=("Purisa", font_size)
    canvas.create_text(float(p[0]), height - 1 - float(p[1]), text=text, fill=color, font=font)

def draw_circle(center, radius, color='white', n=100):
    a = center + Point(radius, 0)
    draw_arc(Arc(center, a, a), color)

def draw_line(a, b, color='white'):
    canvas.create_line(float(a[0]), height - 1 - float(a[1]), float(b[0]), height - 1 - float(b[1]), fill=color)

def draw_arc(arc, color='white', n = 100):
    points = arc.points(n)
    for a, b in zip(points, points[1:]):
        draw_line(a, b, color)

def draw_polygon(points, color='white'):
    xy = flatten_once((float(p.x), height - 1 - float(p.y)) for p in points)
    canvas.create_polygon(xy, fill=color)

def redraw(mouse=Point(width/2, height/2)):
    canvas.delete('all')
    canvas.create_rectangle(0, 0, width, height, fill='black')
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

    with open("points.txt", "wb") as f:
        s = "\n".join(str(p.x) + " " + str(p.y) for p in points)
        f.write(s)
    
    master.destroy()

master.protocol("WM_DELETE_WINDOW", on_closing)

canvas.bind("<B1-Motion>", on_drag_left)

tk.mainloop()
